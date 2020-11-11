#include <iostream>
#include <stack>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include "Encoding.hpp"
#include "NFA.hpp"
#include <stack>
#include "DFA.hpp"

namespace regex
{

using std::vector;
using std::unordered_set;
using std::unordered_map;
using std::static_pointer_cast;
using std::cout;
using std::endl;

NFA::NFA(const RegularExpression::Ptr& exp)
{
	auto subgraph = VisitRegularExpression(exp);
	this->startVertex = subgraph.start;
	this->endVertex = subgraph.end;
	CollectPatterns();
}
NFASubgraph NFA::VisitAlternation(const AlternationExpression::Ptr& exp)
{
	NFASubgraph graph1 = VisitRegularExpression(exp->left);
	NFASubgraph graph2 = VisitRegularExpression(exp->right);
	int in = G.AddNode();
	int out = G.AddNode();
	auto eps = UnicodeRange(EPSILON, EPSILON);
	G.AddEdge(Edge(in, graph1.start, eps));
	G.AddEdge(Edge(in, graph2.start, eps));
	G.AddEdge(Edge(graph1.end, out, eps));
	G.AddEdge(Edge(graph2.end, out, eps));
	return NFASubgraph(in, out);
}
NFASubgraph NFA::VisitConcatenation(const ConcatenationExpression::Ptr& exp)
{
	NFASubgraph src = VisitRegularExpression(exp->left);
	NFASubgraph dest = VisitRegularExpression(exp->right);
	auto eps = UnicodeRange(EPSILON, EPSILON);
	G.AddEdge(Edge(src.end, dest.start, eps));
	return NFASubgraph(src.start, dest.end);
}
NFASubgraph NFA::VisitKleeneStar(const KleeneStarExpression::Ptr& exp)
{
	NFASubgraph graph = VisitRegularExpression(exp->innerExp);
	int in = G.AddNode();
	int out = G.AddNode();
	auto eps = UnicodeRange(EPSILON, EPSILON);
	G.AddEdge(Edge(in, graph.start, eps));
	G.AddEdge(Edge(out, graph.start, eps));
	G.AddEdge(Edge(graph.end, out, eps));
	G.AddEdge(Edge(in, out, eps));
	return NFASubgraph(in, out);
}
NFASubgraph NFA::VisitSymbol(const SymbolExpression::Ptr& exp)
{
	int start = G.AddNode();
	int end = G.AddNode();
	UnicodeRange pattern(exp->lower, exp->upper);
	G.AddEdge(Edge(start, end, pattern));
	return NFASubgraph(start, end);
}

void NFA::CollectPatterns()
{
	patterns.Add(UnicodeRange(EPSILON, EPSILON), EPSILON);
	for (auto edge : G.GetEdges())
	{
		if (!patterns.GetIDByPattern(edge.pattern))
		{
			int n = static_cast<int>(patterns.Size());
			patterns.Add(edge.pattern, n - 1);
		}
	}
}

void NFA::Search(
	int start, int node, UnicodeRange pattern,
	vector<unordered_map<UnicodeRange, unordered_set<StateID>>>& table,
	vector<bool>& visited)
{
	if (visited.at(node))
	{
		return;
	}
	else
	{
		visited.at(node) = true;
		for (const auto& adjEdge : G.Adj(node))
		{
			if (adjEdge.pattern.IsEpsilon() && !pattern.IsEpsilon())
			{
				table[start][pattern].insert(adjEdge.to);
				Search(start, adjEdge.to, pattern, table, visited);
			}
			else if (pattern.IsEpsilon())
			{
				if (!adjEdge.pattern.IsEpsilon())
				{
					table[start][adjEdge.pattern].insert(adjEdge.to);
				}
				Search(start, adjEdge.to, adjEdge.pattern, table, visited);
			}
			else
			{
				// both adjEdge.pattern and pattern are not epsilon.
			}
		}
	}
}

unordered_map<UnicodeRange, unordered_set<StateID>> NFA::ComputeRow(
	size_t node,
	vector<unordered_map<UnicodeRange, unordered_set<StateID>>>& table)
{
	auto eps = UnicodeRange(EPSILON, EPSILON);
	auto epsilonNextStates = table[node][eps];
	unordered_map<UnicodeRange, unordered_set<StateID>> nextStatesMap;
	for (auto[patternID, nextStates] : table[node])
	{
		if (!patternID.IsEpsilon())
		{
			nextStatesMap[patternID] = nextStates;
			// a -> aε*
			for (auto nextState : epsilonNextStates)
			{
				nextStatesMap[patternID].insert(nextState);
			}
		}
	}
	return nextStatesMap;
}

unordered_map<UnicodeRange, unordered_set<StateID>> NFA::ComputeRowOfNodes(
	vector<StateID> nodes,
	vector<unordered_map<UnicodeRange, unordered_set<StateID>>>& table)
{
	unordered_map<UnicodeRange, unordered_set<StateID>> nodeStates;
	for (auto node : nodes)
	{
		auto result = ComputeRow(node, table);
		for (auto[nodeIndex, stateSet] : result)
		{
			for (auto state : stateSet)
			{
				nodeStates[nodeIndex].insert(state);
			}
		}
	}
	return nodeStates;
}

vector<DFATableRow> NFA::EpsilonClosure()
{
	size_t N = G.NodeCount();
	vector<unordered_map<UnicodeRange, unordered_set<StateID>>> table(N);
	for (size_t node = 0; node < N; node++)
	{
		vector<bool> visited(N, false);
		Search(node, node, UnicodeRange(EPSILON, EPSILON), table, visited);
	}
	using namespace std;
	// auto patternIDToInterval = GetPatternIDIntervalMap(intervalMap);
	// for (auto[patternID, interval] : patternIDToInterval)
	// {
	// 	cout << encoding::utf32_to_utf8(interval.ToString()) << " : "
	// 		 << patternID << endl;
	// }
	for (size_t node = 0; node < N; node++)
	{
		cout << "node = " << node << endl;
		for (auto[pattern, set] : table.at(node))
		{
			cout << "pattern = " << encoding::utf32_to_utf8(pattern.ToString())
				 << " : {";
			for (int item : set)
			{
				cout << " " << item;
			}
			cout << " }" << endl;
		}
	}
	size_t start = this->startVertex;
	vector<DFATableRow> rows;

	unordered_map<vector<StateID>, bool, Int32VectorHash> registeredStates;
	vector<size_t> index = {start};
	bool allVisited = false;
	while (!allVisited)
	{
		std::sort(index.begin(), index.end());
		registeredStates[index] = true;
		auto nextStatesMap = ComputeRowOfNodes(index, table);
		cout << "pattern size - 1 = " << (static_cast<int>(patterns.Size()) - 1)
			 << endl;
		vector<vector<StateID>> nextStates(static_cast<int>(patterns.Size()) -
										   1);

		for (auto[pattern, nextStatesSet] : nextStatesMap)
		{
			cout << "pattern " << encoding::utf32_to_utf8(pattern.ToString())
				 << ", next states set ";
			for (auto item : nextStatesSet)
			{
				cout << item << " ";
			}
			cout << endl;
			if (auto index = patterns.GetIDByPattern(pattern))
			{
				nextStates[index.value()] =
					vector<StateID>(nextStatesSet.begin(), nextStatesSet.end());
				std::sort(nextStates[index.value()].begin(),
						  nextStates[index.value()].end());
			}
			cout << "!!!" << endl;
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
		for (auto[nextState, isVisited] : registeredStates)
		{
			if (isVisited == false)
			{
				index = nextState;
				allVisited = false;
			}
		}
	}

	cout << "start = " << start << endl;
	auto viewRow = [&](const DFATableRow& row)
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
			if (auto pattern = patterns.GetPatternByID(i))
			{
				cout << encoding::utf32_to_utf8(pattern.value().ToString());
			}
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
	for (auto row : rows)
	{
		viewRow(row);
	}
	return rows;
}

} // namespace regex
