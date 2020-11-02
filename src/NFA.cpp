#include <iostream>
#include <stack>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include "NFA.hpp"

namespace regex {

using std::vector;
using std::unordered_set;
using std::unordered_map;
using std::static_pointer_cast;

NFAEdge::NFAEdge(): from{0}, to{0}, pattern{0} {
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
NFASubgraph::NFASubgraph(): start{0}, end{0} {
}
NFASubgraph::NFASubgraph(int start, int end)
	: start{start}
	, end{end}
{
}
NFA::NFA()
{
}
NFA::NFA(const RegularExpression::Ptr& exp) {
	CreateGraph(exp);
}
void NFA::CreateGraph(const RegularExpression::Ptr& exp)
{
	auto subgraph = Convert(exp);
	G.start= subgraph.start;
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
		return ConvertKleenStar(
			static_pointer_cast<KleeneStarExpression>(exp));
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
	G.AddEdge(NFAEdge(start, end, exp->character));
	return NFASubgraph(start, end);
}

void NFA::EliminateEpsilonEdges() {
	// TO DO
	vector<bool> visited(G.NodeCount(), false);
	vector<unordered_map<int, unordered_set<int>>> table;
	for(int node = 0; node < G.NodeCount(); node++) {
		unordered_set<int> nodeSet;
		for(const NFAEdge& edge : G.Adj(node)) {
			if(edge.pattern == NFAEdge::EPSILON) {
				std::fill(visited.begin(), visited.end(), false);
			} else {
			}
		}
	}
	std::fill(visited.begin(), visited.end(), false);
}

// void NFA::Search(vector<bool>& visited, unordered_set<int>& nodeSet, int start, int pattern) {
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
