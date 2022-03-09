#include "DFA.hpp"

#include <stack>

namespace regex
{
    u32string UnicodeRange::ToString() const
    {
        if (IsEpsilon())
        {
            return U"ε";
        }
        else if (rangeType == RangeType::LineBegin)
        {
            return U"^";
        }
        else if (rangeType == RangeType::LineEnd)
        {
            return U"$";
        }
        else if (lower == upper)
        {
            return u32string({lower});
        }
        else
        {
            return U"[" + u32string({lower}) + U" - " + u32string({upper}) + U"]";
        }
    }

    UnicodeRange UnicodeRange::EPSILON = UnicodeRange();

    DFA DFATableRowsToDFAGraph(
        const vector<DFATableRow> &rows, const UnicodePatterns &patterns, const Graph &nfaGraph, int nfaEndState)
    {
        unordered_map<std::set<StateID>, StateID, StateIDSetHash> statesID;
        DFA graph;
        for (int i = 0, n = static_cast<int>(patterns.Size()); i < n - 1; i++)
        {
            // add all the patterns except epsilon
            auto pattern = patterns.GetPatternByID(i);
            graph.patterns.Add(pattern, i);
        }
        for (auto row : rows)
        {
            const auto &nextStates = row.nextStates;
            if (!row.index.empty())
            {
                if (IsEndState(row.index, nfaGraph, nfaEndState))
                {
                    StateID id = RecordState(statesID, row.index);
                    graph.endStates.insert(id);
                }
                StateID from = RecordState(statesID, row.index);
                int patternID = 0;
                for (auto nextState : nextStates)
                {
                    if (!nextState.empty())
                    {
                        StateID to = RecordState(statesID, nextState);
                        auto pattern = patterns.GetPatternByID(patternID);
                        graph.G.AddEdge(Edge(from, to, pattern));
                    }
                    patternID++;
                }
            }
        }
        return graph;
    }
    bool IsEndState(const std::set<StateID> &index, const Graph &nfaGraph, StateID nfaEndState)
    {
        for (StateID i : index)
        {
            if (i == nfaEndState)
            {
                return true;
            }
            else if (CanTransit(nfaGraph, i, nfaEndState))
            {
                return true;
            }
            else
            {
                // pass
            }
        }
        return false;
    }
    StateID RecordState(
        unordered_map<std::set<StateID>, StateID, StateIDSetHash> &stateMap, const std::set<StateID> &state)
    {
        if (stateMap.count(state))
        {
            return stateMap[state];
        }
        else
        {
            size_t n = stateMap.size();
            stateMap[state] = n;
            return n;
        }
    }
    DFAMatrix::DFAMatrix(const DFA &dfaGraph)
        : matrix(dfaGraph.G.NodeCount(), vector<int>(dfaGraph.patterns.Size(), -1)), patterns{dfaGraph.patterns},
          endStates{dfaGraph.endStates}
    {
        for (auto edges : dfaGraph.G.adj)
        {
            for (auto edge : edges)
            {
                int patternID = dfaGraph.patterns.GetIDByPattern(edge.pattern);
                matrix.at(edge.from).at(patternID) = edge.to;
            }
        }
    }
    bool DFAMatrix::FullMatch(const u32string &str) const
    {
        return Match(str, 0, str.size(), true) == static_cast<int>(str.size());
    }

    int DFAMatrix::Search(const u32string &str) const
    {
        if (matrix.size() > 0)
        {
            for (size_t start = 0; start < str.size(); start++)
            {
                int length = Match(str, start, str.size(), false);
                if (length != -1)
                {
                    return start;
                }
            }
            return -1;
        }
        else
        {
            return 0;
        }
    }

    int DFAMatrix::Match(const u32string &str, size_t startPos, size_t endPos, bool greedyMode) const
    {
        if (endPos == u32string::npos)
        {
            endPos = str.size();
        }
        else if (endPos <= startPos)
        {
            return 0;
        }
        if (matrix.size() > 0)
        {
            int state = 0;
            int lastMatchedLength = -1;
            size_t i = startPos;
            while (i < endPos)
            {
                char32_t c = str.at(i);
                bool matched = false;
                if (IsEndState(state))
                {
                    if (greedyMode)
                    {
                        /* if in greedy mode, keep matching */
                        /* try to find the longest match */
                        lastMatchedLength = i - startPos;
                    }
                    else
                    {
                        /* if not in greedy mode, return the matched length */
                        return i - startPos;
                    }
                }
                for (size_t j = 0; j < matrix.at(0).size(); j++)
                {
                    if (matrix.at(state).at(j) != -1)
                    {
                        /* can transit from the current state to the next state if pattern j is matched  */
                        auto pattern = patterns.GetPatternByID(static_cast<int>(j));
                        matched = MatchPattern(state, pattern, c, i, j, startPos, endPos);
                        if (matched)
                        {
                            /* if found a viable transition, jump out of the searching loop. */
                            break;
                        }
                        else
                        {
                            /* keep searching the next possible transition */
                        }
                    }
                }
                if (!matched)
                {
                    /* if cannot match any pattern */
                    if (IsEndState(state))
                    {
                        /* if the current state is acceptable, return the current match. */
                        return i - startPos;
                    }
                    else
                    {
                        /* if the current state is not acceptable, return the previous longest match */
                        return lastMatchedLength - startPos;
                    }
                }
            }
            /* reaches to the end of the string */
            if (IsEndState(state))
            {
                /* if the current state is acceptable, return the current match. */
                return endPos - startPos;
            }
            else
            {
                /* if the current state is not acceptable, return the previous longest match */
                return lastMatchedLength - startPos;
            }
        }
        else
        {
            return endPos - startPos;
        }
    }

    bool DFAMatrix::MatchPattern(int &state, const UnicodeRange &pattern, char32_t c, size_t &i, size_t j,
                                 size_t startPos, size_t endPos) const
    {
        if (pattern.rangeType == RangeType::CharacterRange)
        {
            if (pattern.InBetween(c))
            {
                // move to the next state
                state = matrix.at(state).at(j);
                i++;
                return true;
            }
            else
            {
                return false;
            }
        }
        else if (pattern.rangeType == RangeType::LineBegin)
        {
            if (i == startPos)
            {
                // move to the next state
                state = matrix.at(state).at(j);
                return true;
            }
            else
            {
                return false;
            }
        }
        else if (pattern.rangeType == RangeType::LineEnd)
        {
            if (i + 1 == endPos)
            {
                // move to the next state
                state = matrix.at(state).at(j);
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    bool DFAMatrix::IsEndState(int state) const
    {
        return endStates.count(state);
    }

    bool CanTransit(const Graph &G, StateID s1, StateID s2)
    {
        std::stack<StateID> stack;
        unordered_set<StateID> visited;
        stack.push(s1);
        while (!stack.empty())
        {
            StateID current = stack.top();
            stack.pop();
            if (current == s2)
            {
                return true;
            }
            else
            {
                if (!visited.count(current))
                {
                    visited.insert(current);
                    for (auto edge : G.Adj(current))
                    {
                        if (edge.pattern.IsEpsilon())
                        {
                            stack.push(edge.to);
                        }
                    }
                }
            }
        }
        return false;
    }
} // namespace regex