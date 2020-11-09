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

NFASubgraph::NFASubgraph()
	: start{0}
	, end{0}
{
}
NFASubgraph::NFASubgraph(int start, int end)
	: start{start}
	, end{end}
{
}

NFA::NFA(const RegularExpression::Ptr& exp)
{
	intervalMap[Interval(EPSILON, EPSILON)] = EPSILON;
	CreateGraph(exp);
}
void NFA::CreateGraph(const RegularExpression::Ptr& exp)
{
	auto subgraph = Convert(exp);
	this->startVertex = subgraph.start;
	this->endVertex = subgraph.end;
}
NFASubgraph NFA::Convert(const RegularExpression::Ptr& exp)
{
	switch (exp->Kind())
	{
	case RegularExpressionKind::Alternation:
	{
		return ConvertAlternation(
			static_pointer_cast<AlternationExpression>(exp));
	}
	case RegularExpressionKind::Concatenation:
	{
		return ConvertConcatenation(
			static_pointer_cast<ConcatenationExpression>(exp));
	}
	case RegularExpressionKind::KleeneStar:
	{
		return ConvertKleenStar(static_pointer_cast<KleeneStarExpression>(exp));
	}
	default:
	case RegularExpressionKind::Symbol:
	{
		return ConvertSymbol(static_pointer_cast<SymbolExpression>(exp));
	}
	}
}
NFASubgraph NFA::ConvertAlternation(const AlternationExpression::Ptr& exp)
{
	NFASubgraph graph1 = Convert(exp->left);
	NFASubgraph graph2 = Convert(exp->right);
	int in = G.AddNode();
	int out = G.AddNode();
	G.AddEdge(Edge(in, graph1.start, EPSILON));
	G.AddEdge(Edge(in, graph2.start, EPSILON));
	G.AddEdge(Edge(graph1.end, out, EPSILON));
	G.AddEdge(Edge(graph2.end, out, EPSILON));
	return NFASubgraph(in, out);
}
NFASubgraph NFA::ConvertConcatenation(const ConcatenationExpression::Ptr& exp)
{
	NFASubgraph src = Convert(exp->left);
	NFASubgraph dest = Convert(exp->right);
	G.AddEdge(Edge(src.end, dest.start, EPSILON));
	return NFASubgraph(src.start, dest.end);
}
NFASubgraph NFA::ConvertKleenStar(const KleeneStarExpression::Ptr& exp)
{
	NFASubgraph graph = Convert(exp->innerExp);
	int in = G.AddNode();
	int out = G.AddNode();
	G.AddEdge(Edge(in, graph.start, EPSILON));
	G.AddEdge(Edge(out, graph.start, EPSILON));
	G.AddEdge(Edge(graph.end, out, EPSILON));
	G.AddEdge(Edge(in, out, EPSILON));
	return NFASubgraph(in, out);
}
NFASubgraph NFA::ConvertSymbol(const SymbolExpression::Ptr& exp)
{
	int start = G.AddNode();
	int end = G.AddNode();
	PatternID pattern = RecordInterval(exp->character, exp->character);
	G.AddEdge(Edge(start, end, pattern));
	return NFASubgraph(start, end);
}

PatternID NFA::RecordInterval(char32_t lower, char32_t upper)
{
	Interval interval(lower, upper);
	if (intervalMap.count(interval))
	{
		return intervalMap[interval];
	}
	else
	{
		int n = intervalMap.size();
		intervalMap[interval] = n - 1; // minus epsilon
		return n - 1;
	}
}

void NFA::Search(int start, int node, int pattern,
				 vector<unordered_map<PatternID, unordered_set<int>>>& table,
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
			if (adjEdge.pattern == EPSILON && pattern != EPSILON)
			{
				table[start][pattern].insert(adjEdge.to);
				Search(start, adjEdge.to, pattern, table, visited);
			}
			else if (pattern == EPSILON)
			{
				if (adjEdge.pattern != EPSILON)
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

unordered_map<int32_t, unordered_set<int>>
	NFA::ComputeRow(int node,
					vector<unordered_map<int32_t, unordered_set<int>>>& table)
{
	auto epsilonNextStates = table[node][EPSILON];
	unordered_map<int32_t, unordered_set<int>> nextStatesMap;
	for (auto[patternID, nextStates] : table[node])
	{
		if (patternID != EPSILON)
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

unordered_map<int32_t, unordered_set<int>> NFA::ComputeRowOfNodes(
	vector<int> nodes,
	vector<unordered_map<int32_t, unordered_set<int>>>& table)
{
	unordered_map<int32_t, unordered_set<int>> nodeStates;
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
	const int EPSILON = -1;
	int N = G.NodeCount();
	vector<unordered_map<int32_t, unordered_set<int>>> table(N);
	for (int node = 0; node < N; node++)
	{
		vector<bool> visited(N, false);
		Search(node, node, EPSILON, table, visited);
	}
	using namespace std;
	auto patternIDToInterval = GetPatternIDIntervalMap(intervalMap);
	for (auto[patternID, interval] : patternIDToInterval)
	{
		cout << UTF32ToUTF8(interval.ToString()) << " : " << patternID << endl;
	}
	for (int node = 0; node < N; node++)
	{
		cout << "node = " << node << endl;
		for (auto[patternID, set] : table.at(node))
		{
			auto interval = patternIDToInterval[patternID];
			cout << "pattern ID = " << UTF32ToUTF8(interval.ToString())
				 << " : {";
			for (int item : set)
			{
				cout << " " << item;
			}
			cout << " }" << endl;
		}
	}
	int start = this->startVertex;
	vector<DFATableRow> rows;

	cout << "intervalMap.size() = " << intervalMap.size() << endl;

	unordered_map<vector<int32_t>, bool, Int32VectorHash> registeredStates;
	vector<int32_t> index = {start};
	bool allVisited = false;
	while (!allVisited)
	{
		std::sort(index.begin(), index.end());
		registeredStates[index] = true;
		auto nextStatesMap = ComputeRowOfNodes(index, table);
		vector<vector<int32_t>> nextStates(
			static_cast<int>(intervalMap.size()) - 1);

		for (auto[patternID, nextStatesSet] : nextStatesMap)
		{
			cout << "pattern id " << patternID << ", next states set ";
			for (auto item : nextStatesSet)
			{
				cout << item << " ";
			}
			cout << endl;
			nextStates[patternID] =
				vector<int32_t>(nextStatesSet.begin(), nextStatesSet.end());
			std::sort(nextStates[patternID].begin(),
					  nextStates[patternID].end());
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
		for (PatternID patternID = 0;
			 patternID < static_cast<int>(row.nextStates.size()); patternID++)
		{
			auto state = row.nextStates.at(patternID);
			cout << "STATE ";
			auto interval = patternIDToInterval[patternID];
			cout << UTF32ToUTF8(
				u32string({static_cast<char32_t>(interval.lower)}));
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

unordered_map<PatternID, Interval> GetPatternIDIntervalMap(
	const unordered_map<Interval, PatternID, IntervalHash>& intervalMap)
{
	unordered_map<PatternID, Interval> result;
	for (auto[interval, patternID] : intervalMap)
	{
		result[patternID] = interval;
	}
	return result;
}

void ViewNFA(const NFA& nfa)
{
	const auto& G = nfa.G;
	auto edges = G.GetEdges();
	cout << "digraph {" << endl;
	cout << "rankdir=LR;" << endl;
	cout << "size=\"8,5;\"" << endl;
	cout << "node [shape = doublecircle];";
	for (size_t i = 0; i < G.NodeCount(); i++)
	{
		if (G.Adj(i).size() == 0)
		{
			cout << i << ";";
		}
	}
	cout << endl;
	cout << "node [shape = point ]; start;" << endl;
	cout << "node [shape = circle];" << endl;
	cout << "start -> " << nfa.startVertex << ";" << endl;
	unordered_map<int32_t, Interval> converter;
	for (auto[interval, patternID] : nfa.intervalMap)
	{
		converter[patternID] = interval;
	}
	for (const auto& edge : edges)
	{
		cout << edge.from << " -> " << edge.to;
		if (edge.IsEpsilon())
		{
			cout << "[label=\"@\"];" << endl;
		}
		else
		{
			auto interval = converter[edge.pattern];
			cout << "[label=\"" << UTF32ToUTF8(u32string(
									   {static_cast<char32_t>(interval.lower)}))
				 << "\"];" << endl;
		}
	}
	cout << "}" << endl;
}

} // namespace regex
