#include "DFA.hpp"
#include <iostream>
#include <stack>

namespace regex
{

u32string UnicodeRange::ToString() const
{
	if (lower == EPSILON && upper == EPSILON)
	{
		return U"ε";
	}
	else if (lower == upper)
	{
		return u32string({static_cast<char32_t>(lower)});
	}
	else
	{
		return u32string(U"[") + static_cast<char32_t>(lower) + U" - " +
			   static_cast<char32_t>(upper) + U"]";
	}
}
DFA DFATableToDFAGraph(const vector<DFATableRow>& rows,
					   const UnicodePatterns& patterns, const Graph& nfaGraph,
					   int nfaEndState)
{
	unordered_map<std::set<StateID>, StateID, StateIDSetHash> statesID;
	DFA graph;
	for (int i = 0; i < static_cast<int>(patterns.Size()) - 1; i++)
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
	}
	return false;
}
StateID RecordState(unordered_map<std::set<StateID>, StateID, StateIDSetHash>& stateMap,
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
bool DFAMatrix::Match(const u32string& str) const
{
	return MatchFromBeginning(str, 0, true) == static_cast<int>(str.size());
}

int DFAMatrix::Find(const u32string& str) const
{
	if (matrix.size() > 0)
	{
		for (size_t start = 0; start < str.size(); start++)
		{
			int length = MatchFromBeginning(str, start, false);
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

int DFAMatrix::MatchFromBeginning(const u32string& str, size_t startIndex,
								  bool greedyMode) const
{
	if (matrix.size() > 0)
	{
		int state = 0;
		int lastMatchedLength = -1;
		for (size_t i = startIndex; i < str.size(); i++)
		{
			char32_t c = str[i];
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
						if (pattern.value().InBetween(c))
						{
							// move to the next state
							state = matrix.at(state).at(j);
							matched = true;
							break;
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
	DFAMatrix matrix;
	matrix.matrix = vector<vector<int>>(
		dfaGraph.G.NodeCount(), vector<int>(dfaGraph.patterns.Size(), -1));
	matrix.patterns = dfaGraph.patterns;
	matrix.endStates = dfaGraph.endStates;
	for (auto edges : dfaGraph.G.adj)
	{
		for (auto edge : edges)
		{
			matrix.matrix.at(edge.from).at(
				matrix.patterns.GetIDByPattern(edge.pattern).value()) = edge.to;
		}
	}
	return matrix;
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