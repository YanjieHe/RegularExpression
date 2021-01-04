#include "DFA.hpp"
#include <iostream>
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

DFA DFATableRowsToDFAGraph(const vector<DFATableRow>& rows,
						   const UnicodePatterns& patterns,
						   const Graph& nfaGraph, int nfaEndState)
{
	unordered_map<std::set<StateID>, StateID, StateIDSetHash> statesID;
	DFA graph;
	for (int i = 0, n = static_cast<int>(patterns.Size()); i < n - 1; i++)
	{
		// add all the patterns except epsilon
		if (auto pattern = patterns.GetPatternByID(i))
		{
			graph.patterns.Add(pattern.value(), i);
		}
	}
	for (auto row : rows)
	{
		const auto& nextStates = row.nextStates;
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
					if (auto pattern = patterns.GetPatternByID(patternID))
					{
						graph.G.AddEdge(Edge(from, to, pattern.value()));
					}
				}
				patternID++;
			}
		}
	}
	return graph;
}
bool IsEndState(const std::set<StateID>& index, const Graph& nfaGraph,
				StateID nfaEndState)
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
	unordered_map<std::set<StateID>, StateID, StateIDSetHash>& stateMap,
	const std::set<StateID>& state)
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
bool DFAMatrix::FullMatch(const u32string& str) const
{
	return Match(str, 0, str.size(), true) == static_cast<int>(str.size());
}

int DFAMatrix::Search(const u32string& str) const
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

int DFAMatrix::Match(const u32string& str, size_t startPos, size_t endPos,
					 bool greedyMode) const
{
	if (matrix.size() > 0)
	{
		int state = 0;
		int lastMatchedLength = -1;
		size_t i = startPos;
		while (i < endPos)
		{
			char32_t c = str.at(i);
			bool matched = false;
			if (endStates.count(state))
			{
				if (greedyMode)
				{
					lastMatchedLength = i;
				}
				else
				{
					return i;
				}
			}
			for (size_t j = 0; j < matrix.at(0).size(); j++)
			{
				if (matrix.at(state).at(j) != -1)
				{
					if (auto pattern =
							patterns.GetPatternByID(static_cast<int>(j)))
					{
						if (pattern.value().rangeType ==
							RangeType::CharacterRange)
						{
							if (pattern.value().InBetween(c))
							{
								// move to the next state
								state = matrix.at(state).at(j);
								matched = true;
								i++;
								break;
							}
							else
							{
								// pass
							}
						}
						else if (pattern.value().rangeType ==
								 RangeType::LineBegin)
						{
							if (i == startPos)
							{
								// move to the next state
								state = matrix.at(state).at(j);
								matched = true;
								break;
							}
							else
							{
								// pass
							}
						}
						else if (pattern.value().rangeType ==
								 RangeType::LineEnd)
						{
							if (i + 1 == endPos)
							{
								// move to the next state
								state = matrix.at(state).at(j);
								matched = true;
								break;
							}
							else
							{
								// pass
							}
						}
						else
						{
							// pass
						}
					}
				}
			}
			if (!matched)
			{
				if (endStates.count(state))
				{
					return i;
				}
				else
				{
					return lastMatchedLength;
				}
			}
		}
		if (endStates.count(state))
		{
			return str.size();
		}
		else
		{
			return lastMatchedLength;
		}
	}
	else
	{
		return str.size();
	}
}

DFAMatrix CreateDFAMatrix(const DFA& dfaGraph)
{
	vector<vector<int>> transitionMatrix(
		dfaGraph.G.NodeCount(), vector<int>(dfaGraph.patterns.Size(), -1));
	for (auto edges : dfaGraph.G.adj)
	{
		for (auto edge : edges)
		{
			int patternID =
				dfaGraph.patterns.GetIDByPattern(edge.pattern).value();
			transitionMatrix.at(edge.from).at(patternID) = edge.to;
		}
	}
	return DFAMatrix(transitionMatrix, dfaGraph.patterns, dfaGraph.endStates);
}

bool CanTransit(const Graph& G, StateID s1, StateID s2)
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