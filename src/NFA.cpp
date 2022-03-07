#include "NFA.hpp"

#include <algorithm>
#include <iostream>
#include <stack>
#include <unordered_set>
#include <utfcpp/utf8/cpp11.h>

#include "DFA.hpp"

namespace regex
{

    using std::cout;
    using std::endl;
    using std::static_pointer_cast;
    using std::unordered_set;

    NFA::NFA(const RegularExpression::Ptr &exp)
    {
        auto subgraph = VisitRegularExpression(exp);
        this->startVertex = subgraph.start;
        this->endVertex = subgraph.end;
        CollectPatterns();
    }
    NFASubgraph NFA::VisitAlternation(const AlternationExpression::Ptr &exp)
    {
        NFASubgraph graph1 = VisitRegularExpression(exp->left);
        NFASubgraph graph2 = VisitRegularExpression(exp->right);
        int in = G.AddNode();
        int out = G.AddNode();
        G.AddEdge(Edge(in, graph1.start, UnicodeRange::EPSILON));
        G.AddEdge(Edge(in, graph2.start, UnicodeRange::EPSILON));
        G.AddEdge(Edge(graph1.end, out, UnicodeRange::EPSILON));
        G.AddEdge(Edge(graph2.end, out, UnicodeRange::EPSILON));
        return NFASubgraph(in, out);
    }
    NFASubgraph NFA::VisitConcatenation(const ConcatenationExpression::Ptr &exp)
    {
        NFASubgraph src = VisitRegularExpression(exp->left);
        NFASubgraph dest = VisitRegularExpression(exp->right);
        G.AddEdge(Edge(src.end, dest.start, UnicodeRange::EPSILON));
        return NFASubgraph(src.start, dest.end);
    }
    NFASubgraph NFA::VisitKleeneStar(const KleeneStarExpression::Ptr &exp)
    {
        NFASubgraph graph = VisitRegularExpression(exp->innerExp);
        int in = G.AddNode();
        int out = G.AddNode();
        G.AddEdge(Edge(in, graph.start, UnicodeRange::EPSILON));
        G.AddEdge(Edge(out, graph.start, UnicodeRange::EPSILON));
        G.AddEdge(Edge(graph.end, out, UnicodeRange::EPSILON));
        G.AddEdge(Edge(in, out, UnicodeRange::EPSILON));
        return NFASubgraph(in, out);
    }
    NFASubgraph NFA::VisitSymbol(const SymbolExpression::Ptr &exp)
    {
        int start = G.AddNode();
        int end = G.AddNode();
        UnicodeRange pattern = exp->range;
        G.AddEdge(Edge(start, end, pattern));
        return NFASubgraph(start, end);
    }

    void NFA::CollectPatterns()
    {
        patterns.Add(UnicodeRange::EPSILON, EPSILON);
        for (auto edge : G.GetEdges())
        {
            if (!patterns.HasPattern(edge.pattern))
            {
                int n = static_cast<int>(patterns.Size());
                patterns.Add(edge.pattern, n - 1);
            }
        }
    }
    /**
     * NFA
     *
     * @param  {int} start            : start vertex id
     * @param  {int} vertex           : current vertex id
     * @param  {UnicodeRange} pattern : current unicode pattern
     * @param  {NFA::Table} table     : the table for recording next states after
     * transitions
     * @param  {vector<bool>} visited : keep track of visited vertices
     */
    void NFA::FindNextStates(int start, int vertex, UnicodeRange pattern, NFA::Table &table, vector<bool> &visited)
    {
        if (!visited.at(vertex))
        {
            visited.at(vertex) = true;
            for (const auto &adjEdge : G.Adj(vertex))
            {
                if (adjEdge.pattern.IsEpsilon() && !pattern.IsEpsilon())
                {
                    table[start][pattern].insert(adjEdge.to);
                    FindNextStates(start, adjEdge.to, pattern, table, visited);
                }
                else if (pattern.IsEpsilon())
                {
                    if (!adjEdge.pattern.IsEpsilon())
                    {
                        table[start][adjEdge.pattern].insert(adjEdge.to);
                    }
                    FindNextStates(start, adjEdge.to, adjEdge.pattern, table, visited);
                }
                else
                {
                    // both adjEdge.pattern and pattern are not epsilon.
                }
            }
        }
        else
        {
            // the program has already visited the current vertex
        }
    }
    /**
     * NFA
     *
     * @param  {size_t} vertex    : current vertex
     * @param  {NFA::Table} table : the table for recording next states after
     * transitions
     * @return {NFA::Row}         : next row containing states after transitions
     */
    NFA::Row NFA::ComputeNextRow(size_t vertex, NFA::Table &table)
    {
        auto epsilonNextStates = table[vertex][UnicodeRange::EPSILON];
        Row nextRow;
        for (auto [patternID, nextStates] : table[vertex])
        {
            if (!patternID.IsEpsilon())
            {
                nextRow[patternID] = nextStates;
                // a -> aε*
                for (auto nextState : epsilonNextStates)
                {
                    nextRow[patternID].insert(nextState);
                }
            }
        }
        return nextRow;
    }

    NFA::Row NFA::ComputeVerticesNextRow(std::set<StateID> vertices, NFA::Table &table)
    {
        Row verticesNextRow;
        for (auto vertex : vertices)
        {
            auto nextRow = ComputeNextRow(vertex, table);
            for (auto [vertexIndex, stateSet] : nextRow)
            {
                for (auto state : stateSet)
                {
                    verticesNextRow[vertexIndex].insert(state);
                }
            }
        }
        return verticesNextRow;
    }

    vector<DFATableRow> NFA::EpsilonClosure()
    {
        size_t N = G.NodeCount();
        Table table(N);
        for (size_t node = 0; node < N; node++)
        {
            vector<bool> visited(N, false);
            FindNextStates(node, node, UnicodeRange::EPSILON, table, visited);
        }
        vector<DFATableRow> rows;

        unordered_map<std::set<StateID>, bool, StateIDSetHash> registeredStates;
        std::set<StateID> index = {startVertex};
        bool allVisited = false;
        while (!allVisited)
        {
            registeredStates[index] = true;
            auto nextStatesMap = ComputeVerticesNextRow(index, table);
            vector<std::set<StateID>> nextStates(static_cast<int>(patterns.Size()) - 1);

            for (auto [pattern, nextStatesSet] : nextStatesMap)
            {
                int index = patterns.GetIDByPattern(pattern);
                nextStates.at(index) = std::set<StateID>(nextStatesSet.begin(), nextStatesSet.end());
            }
            rows.push_back(DFATableRow(index, nextStates));
            for (auto state : nextStates)
            {
                if (registeredStates.count(state) == 0) // not found
                {
                    registeredStates[state] = false;
                }
            }
            allVisited = true;
            for (auto [nextState, isVisited] : registeredStates)
            {
                if (isVisited == false)
                {
                    index = nextState;
                    allVisited = false;
                }
            }
        }
        return rows;
    }

    void ViewRow(const DFATableRow &row, const UnicodePatterns &patterns)
    {
        cout << "index { ";
        for (auto item : row.index)
        {
            cout << item << " ";
        }
        cout << "} ";
        for (size_t i = 0; i < row.nextStates.size(); i++)
        {
            auto state = row.nextStates.at(i);
            cout << "STATE ";
            auto pattern = patterns.GetPatternByID(i);
            cout << utf8::utf32to8(pattern.ToString());
            cout << " ";
            cout << "{";
            for (auto item : state)
            {
                cout << item << " ";
            }
            cout << "} ";
        }
        cout << endl;
    };

} // namespace regex
