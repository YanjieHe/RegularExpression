#include "DFA.hpp"
#include <iostream>

namespace regex
{

u32string Interval::ToString() const
{
	if (lower == -1 && upper == -1)
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
DFAGraph DFATableToDFAGraph(
	const vector<DFATableRow>& rows,
	const unordered_map<int32_t, Interval>& patternIDToInterval,
	int nfaEndState)
{
	unordered_map<vector<int32_t>, int32_t, Int32VectorHash> statesID;
	DFAGraph graph;
	for (int i = 0; i < static_cast<int>(patternIDToInterval.size()) - 1; i++)
	{
		graph.patternIDToIntervals.push_back(patternIDToInterval.at(i));
	}
	for (auto row : rows)
	{
		const auto& nextStates = row.nextStates;
		if (!row.index.empty())
		{
			if (IsEndState(row.index, nfaEndState))
			{
				int id = RecordState(statesID, row.index);
				graph.endStates.insert(id);
			}
			int from = RecordState(statesID, row.index);
			int pattern = 0;
			for (auto nextState : nextStates)
			{
				if (!nextState.empty())
				{
					int to = RecordState(statesID, nextState);
					graph.dfa.AddEdge(Edge(from, to, pattern));
				}
				pattern++;
			}
		}
	}
	return graph;
}
bool IsEndState(const vector<int32_t>& index, int nfaEndState)
{
	for (int i : index)
	{
		if (i == nfaEndState)
		{
			return true;
		}
	}
	return false;
}
int32_t RecordState(
	unordered_map<vector<int32_t>, int32_t, Int32VectorHash>& stateMap,
	const vector<int32_t>& state)
{
	if (stateMap.count(state))
	{
		return stateMap[state];
	}
	else
	{
		int n = stateMap.size();
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
				std::cout << "end states count state" << std::endl;
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
					const Interval& interval = patterns.at(j);
					if (static_cast<char32_t>(interval.lower) <= c &&
						c <= static_cast<char32_t>(interval.upper))
					{
						state =
							matrix.at(state).at(j); // move to the next state
						matched = true;
						break;
					}
				}
			}
			if (!matched)
			{
				std::cout << "not matched" << std::endl;
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
		std::cout << "current state " << state << std::endl;
		std::cout << "end states:" << std::endl;
		for (auto endState : endStates)
		{
			std::cout << "end state = " << endState << std::endl;
		}
		if (endStates.count(state))
		{
			std::cout << "current state is end state" << std::endl;
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

DFAMatrix CreateDFAMatrix(const DFAGraph& dfaGraph)
{
	DFAMatrix matrix;
	matrix.matrix = vector<vector<int>>(
		dfaGraph.dfa.NodeCount(),
		vector<int>(dfaGraph.patternIDToIntervals.size(), -1));
	matrix.patterns = dfaGraph.patternIDToIntervals;
	matrix.endStates = dfaGraph.endStates;
	for (auto edges : dfaGraph.dfa.adj)
	{
		for (auto edge : edges)
		{
			matrix.matrix.at(edge.from).at(edge.pattern) = edge.to;
		}
	}
	return matrix;
}

} // namespace regex