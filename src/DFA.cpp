#include "DFA.hpp"

namespace regex
{
DFAEdge::DFAEdge(int from, int to, int pattern)
	: from{from}
	, to{to}
	, pattern{pattern}
{
}
void DFA::AddEdge(const DFAEdge& edge)
{
	int n = NodeCount();
	while (n <= edge.from || n <= edge.to)
	{
		adj.emplace_back();
		n = NodeCount();
	}
	adj.at(edge.from).push_back(edge);
}

int DFA::NodeCount() const
{
	return static_cast<int>(adj.size());
}

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
	const unordered_map<int32_t, Interval>& patternIDToInterval)
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
			if (IsEndState(nextStates))
			{
				int id = RecordState(statesID, row.index);
				graph.endStates.push_back(id);
			}
			else
			{
				int from = RecordState(statesID, row.index);
				int pattern = 0;
				for (auto nextState : nextStates)
				{
					if (!nextState.empty())
					{
						int to = RecordState(statesID, nextState);
						graph.dfa.AddEdge(DFAEdge(from, to, pattern));
					}
					pattern++;
				}
			}
		}
	}
	return graph;
}
bool IsEndState(const vector<vector<int32_t>>& nextStates)
{
	for (auto nextState : nextStates)
	{
		if (!nextState.empty())
		{
			return false;
		}
	}
	return true;
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
} // namespace regex