#include <iostream>
#include <stack>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include "NFA.hpp"
#include <stack>

namespace regex
{

using std::vector;
using std::unordered_set;
using std::unordered_map;
using std::static_pointer_cast;

NFAEdge::NFAEdge()
	: from{0}
	, to{0}
	, pattern{0}
{
}
NFAEdge::NFAEdge(int from, int to, int pattern)
	: from{from}
	, to{to}
	, pattern(pattern)
{
}
NFAGraph::NFAGraph()
{
}
int NFAGraph::AddNode()
{
	int n = adj.size();
	adj.emplace_back();
	return n;
}
void NFAGraph::AddEdge(const NFAEdge& edge)
{
	int n = NodeCount();
	while (n <= edge.from || n <= edge.to)
	{
		adj.emplace_back();
		n = NodeCount();
	}
	adj.at(edge.from).push_back(edge);
}
int NFAGraph::NodeCount() const
{
	return static_cast<int>(adj.size());
}
vector<NFAEdge> NFAGraph::GetEdges() const
{
	vector<NFAEdge> edges;
	for (const auto& edgeVec : adj)
	{
		for (const auto& edge : edgeVec)
		{
			edges.push_back(edge);
		}
	}
	return edges;
}
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
	intervalMap[Interval(-1, -1)] = -1;
	CreateGraph(exp);
}
void NFA::CreateGraph(const RegularExpression::Ptr& exp)
{
	auto subgraph = Convert(exp);
	G.start = subgraph.start;
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
	G.AddEdge(NFAEdge(in, graph1.start, NFAEdge::EPSILON));
	G.AddEdge(NFAEdge(in, graph2.start, NFAEdge::EPSILON));
	G.AddEdge(NFAEdge(graph1.end, out, NFAEdge::EPSILON));
	G.AddEdge(NFAEdge(graph2.end, out, NFAEdge::EPSILON));
	return NFASubgraph(in, out);
}
NFASubgraph NFA::ConvertConcatenation(const ConcatenationExpression::Ptr& exp)
{
	NFASubgraph src = Convert(exp->left);
	NFASubgraph dest = Convert(exp->right);
	G.AddEdge(NFAEdge(src.end, dest.start, NFAEdge::EPSILON));
	return NFASubgraph(src.start, dest.end);
}
NFASubgraph NFA::ConvertKleenStar(const KleeneStarExpression::Ptr& exp)
{
	NFASubgraph graph = Convert(exp->innerExp);
	int in = G.AddNode();
	int out = G.AddNode();
	G.AddEdge(NFAEdge(in, graph.start, NFAEdge::EPSILON));
	G.AddEdge(NFAEdge(out, graph.start, NFAEdge::EPSILON));
	G.AddEdge(NFAEdge(graph.end, out, NFAEdge::EPSILON));
	G.AddEdge(NFAEdge(in, out, NFAEdge::EPSILON));
	return NFASubgraph(in, out);
}
NFASubgraph NFA::ConvertSymbol(const SymbolExpression::Ptr& exp)
{
	int start = G.AddNode();
	int end = G.AddNode();
	int pattern = RecordInterval(exp->character, exp->character);
	G.AddEdge(NFAEdge(start, end, pattern));
	return NFASubgraph(start, end);
}

int32_t NFA::RecordInterval(char32_t lower, char32_t upper)
{
	Interval interval(lower, upper);
	if (intervalMap.count(interval))
	{
		return intervalMap[interval];
	}
	else
	{
		int n = intervalMap.size();
		intervalMap[interval] = n;
		return n;
	}
}

void NFA::Search(int node, int pattern,
				 vector<unordered_map<int32_t, unordered_set<int>>>& table,
				 vector<bool>& visited)
{
	const int EPSILON = -1;
	if (visited.at(node))
	{
		return;
	}
	else
	{
		visited.at(node) = true;
		for (const auto& adjEdge : G.Adj(node))
		{
			if (adjEdge.pattern == EPSILON)
			{
				table[node][EPSILON].insert(adjEdge.to);
				Search(adjEdge.to, pattern, table, visited);
			}
			else if (pattern == EPSILON)
			{
				table[node][adjEdge.pattern].insert(adjEdge.to);
				Search(adjEdge.to, adjEdge.pattern, table, visited);
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
	const int EPSILON = -1;
	auto epsilonNextStates = table[node][EPSILON];
	unordered_map<int32_t, unordered_set<int>> nextStatesMap;
	for (auto[patternID, nextStates] : table[node])
	{
		if (patternID != -1)
		{
			nextStatesMap[patternID] = nextStates;
			for (auto nextState : epsilonNextStates)
			{
				nextStatesMap[patternID].insert(nextState);
			}
		}
	}
	return nextStatesMap;
}
void NFA::EpsilonClosure()
{
	const int EPSILON = -1;
	int N = G.NodeCount();
	vector<unordered_map<int32_t, unordered_set<int>>> table(N);
	for (int i = 0; i < N; i++)
	{
		vector<bool> visited(N, false);
		Search(i, EPSILON, table, visited);
	}
	using namespace std;
	for (auto[interval, patternID] : intervalMap)
	{
		if (interval.lower != -1)
		{
			cout << "[" << static_cast<char>(interval.lower) << ", "
				 << static_cast<char>(interval.upper) << "] : " << patternID
				 << endl;
		}
		else
		{
			cout << "[" << interval.lower << ", " << interval.upper
				 << "] : " << patternID << endl;
		}
	}
	for (int i = 0; i < N; i++)
	{
		cout << "i = " << i << endl;
		for (auto[patternID, set] : table.at(i))
		{
			cout << "pattern ID = " << patternID << " : {";
			for (int item : set)
			{
				cout << " " << item;
			}
			cout << " }" << endl;
		}
	}
	int start = G.start;
	vector<DFATableRow> rows;
	auto nextStatesMap = ComputeRow(start, table);

	struct VectorHasher
	{
		size_t operator()(const vector<int32_t>& V) const
		{
			size_t hash = V.size();
			for (auto& i : V)
			{
				hash ^= i + 0x9e3779b9 + (hash << 6) + (hash >> 2);
			}
			return hash;
		}
	};

	vector<unordered_set<int32_t>> nextStates(
		static_cast<int>(intervalMap.size()) - 1);
	unordered_map<vector<int32_t>, bool, VectorHasher> registered;

	for (auto[patternID, nextStatesSet] : nextStatesMap)
	{
		nextStates[patternID] = nextStatesSet;
	}
	vector<int32_t> index = {start};
	std::sort(index.begin(), index.end());
	rows.push_back(DFATableRow({start}, nextStates));
	auto viewRow = [](const DFATableRow& row)
	{
		for (auto item : row.index)
		{
			cout << item << " " << endl;
		}
		for (size_t i = 0; i < row.nextStates.size(); i++)
		{
			auto state = row.nextStates.at(i);
			cout << "STATE " << i << " ";
			cout << "{";
			for (auto item : state)
			{
				cout << item << " ";
			}
			cout << "} ";
		}
		cout << endl;
	};
	viewRow(rows.front());
	// rows.push_back(DFATableRow({start}, ))
}
// void NFA::EliminateEpsilonEdges() {
// 	// TO DO
// 	vector<bool> visited(G.NodeCount(), false);
// 	vector<unordered_map<int, unordered_set<int>>> table;
// 	for(int node = 0; node < G.NodeCount(); node++) {
// 		unordered_set<int> nodeSet;
// 		for(const NFAEdge& edge : G.Adj(node)) {
// 			if(edge.pattern == NFAEdge::EPSILON) {
// 				std::fill(visited.begin(), visited.end(), false);
// 			} else {
// 			}
// 		}
// 	}
// 	std::fill(visited.begin(), visited.end(), false);
// }

// void NFA::Search(vector<bool>& visited, unordered_set<int>& nodeSet, int
// start, int pattern) {
// 	for(const NFAEdge& edge : G.Adj(start)) {
// 		if(not visited[edge.To()]) {
// 			visited[edge.To()] = true;
// 			if (pattern == edge.Pattern()) {
// 				if(pattern == NFAEdge::EPSILON) {
// 					nodeSet.insert(edge.To());
// 					Search(visited, nodeSet, edge.To(), pattern);
// 				} else {
// 					nodeSet.insert(edge.To());
// 				}
// 			}
// 		}
// 	}
// }

// NFAGraph EliminateEpsilonEdges(const NFAGraph& input)
// {
// 	vector<bool> visited(input.NodeCount());
// 	for (int i = 0, n = visited.size(); i < n; i++)
// 	{
// 		visited[i] = false;
// 	}
// 	NFAGraph output;
// 	output.start = input.start;
// 	CollectValidEdges(input, output, input.start, input.start, visited);
// 	return output;
// }
// void CollectValidEdges(NFAGraph& input, NFAGraph& output, int from, int node,
// 					   vector<bool>& visited)
// {
// 	if (visited.at(node))
// 	{
// 		return;
// 	}
// 	else
// 	{
// 		visited.at(node) = true;
// 		auto edges = input.adj.at(node);
// 		for (auto& edge : edges)
// 		{
// 			if (edge.IsEpsilon())
// 			{
// 				CollectValidEdges(input, output, from, edge.to, visited);
// 			}
// 			else
// 			{
// 				output.AddEdge(NFAEdge(from, edge.to, edge.pattern));
// 				CollectValidEdges(input, output, edge.to, edge.to, visited);
// 			}
// 		}
// 	}
// }
// NFAGraph RenumberGraph(NFAGraph& input)
// {
// 	unordered_map<int, int> map;
// 	vector<bool> visited(input.NodeCount());
// 	for (int i = 0, n = visited.size(); i < n; i++)
// 	{
// 		visited.at(i) = false;
// 	}
// 	RenumberSubgraph(input, input.start, visited, map);
// 	auto inputEdges = input.GetEdges();
// 	NFAGraph output;
// 	output.start = map[input.start];
// 	for (auto& edge : inputEdges)
// 	{
// 		output.AddEdge(NFAEdge(map[edge.from], map[edge.to], edge.pattern));
// 	}
// 	return output;
// }
// void RenumberSubgraph(NFAGraph& input, int node, vector<bool>& visited,
// 					  unordered_map<int, int>& map)
// {
// 	if (visited.at(node))
// 	{
// 		return;
// 	}
// 	else
// 	{
// 		visited.at(node) = true;
// 		bool found = map.find(node) != map.end();
// 		if (!found)
// 		{
// 			int n = map.size();
// 			map.insert({node, n});
// 		}
// 		auto edges = input.adj.at(node);
// 		for (auto& edge : edges)
// 		{
// 			RenumberSubgraph(input, edge.to, visited, map);
// 		}
// 	}
// }

} // namespace regex
