#include "NFA.hpp"
#include <iostream>
using std::dynamic_pointer_cast;
using std::endl;
using std::wcout;

NFAEdge::NFAEdge(int from, int to, int pattern)
	: from{from}
	, to{to}
	, pattern(pattern)
{
}
bool NFAEdge::IsEpsilon()
{
	return pattern == -1;
}
NFAGraph::NFAGraph()
{
}
int NFAGraph::AddNode()
{
	int n = adj.size();
	adj.push_back(vector<NFAEdge>());
	isFinal.push_back(true);
	return n;
}
void NFAGraph::AddEdge(NFAEdge edge)
{
	int n = static_cast<int>(adj.size());
	while (n <= edge.from || n <= edge.to)
	{
		adj.push_back(vector<NFAEdge>());
		n = static_cast<int>(adj.size());
	}
	adj.at(edge.from).push_back(edge);
}
int NFAGraph::NodeCount()
{
	return adj.size();
}
vector<NFAEdge> NFAGraph::GetEdges()
{
	vector<NFAEdge> edges;
	for (auto& edgeVec : adj)
	{
		for (auto& edge : edgeVec)
		{
			edges.push_back(edge);
		}
	}
	return edges;
}
NFASubgraph::NFASubgraph(int start, int end)
	: start{start}
	, end{end}
{
}
NFA::NFA()
{
}
void NFA::CreateGraph(RegularExpPtr exp)
{
	auto subgraph = Convert(exp);
	G.start = subgraph.start;
}
NFASubgraph NFA::Convert(RegularExpPtr exp)
{
	switch (exp->kind)
	{
	case RegularExpressionKind::Alternation:
	{
		return ConvertAlternation(
			dynamic_pointer_cast<AlternationExpression>(exp));
	}
	case RegularExpressionKind::Concatenation:
	{
		return ConvertConcatenation(
			dynamic_pointer_cast<ConcatenationExpression>(exp));
	}
	case RegularExpressionKind::KleeneStar:
	{
		return ConvertKleenStar(
			dynamic_pointer_cast<KleeneStarExpression>(exp));
	}
	default:
	case RegularExpressionKind::Symbol:
	{
		return ConvertSymbol(dynamic_pointer_cast<SymbolExpression>(exp));
	}
	}
}
NFASubgraph NFA::ConvertAlternation(AlternationExpPtr exp)
{
	NFASubgraph graph1 = Convert(exp->left);
	NFASubgraph graph2 = Convert(exp->right);
	int in = G.AddNode();
	int out = G.AddNode();
	G.AddEdge(NFAEdge(in, graph1.start, -1));
	G.AddEdge(NFAEdge(in, graph2.start, -1));
	G.AddEdge(NFAEdge(graph1.end, out, -1));
	G.AddEdge(NFAEdge(graph2.end, out, -1));
	return NFASubgraph(in, out);
}
NFASubgraph NFA::ConvertConcatenation(ConcatenationExpPtr exp)
{
	NFASubgraph src = Convert(exp->left);
	NFASubgraph dest = Convert(exp->right);
	G.AddEdge(NFAEdge(src.end, dest.start, -1));
	return NFASubgraph(src.start, dest.end);
}
NFASubgraph NFA::ConvertKleenStar(KleeneStarExpPtr exp)
{
	NFASubgraph graph = Convert(exp->innerExp);
	int in = G.AddNode();
	int out = G.AddNode();
	G.AddEdge(NFAEdge(in, graph.start, -1));
	G.AddEdge(NFAEdge(graph.end, graph.start, -1));
	G.AddEdge(NFAEdge(graph.end, out, -1));
	G.AddEdge(NFAEdge(in, out, -1));
	return NFASubgraph(in, out);
}
NFASubgraph NFA::ConvertSymbol(SymbolExpPtr exp)
{
	int start = G.AddNode();
	int end = G.AddNode();
	G.AddEdge(NFAEdge(start, end, exp->character));
	return NFASubgraph(start, end);
}
NFAGraph EliminateEpsilonEdges(NFAGraph& input)
{
	vector<bool> visited(input.NodeCount());
	for (int i = 0, n = visited.size(); i < n; i++)
	{
		visited[i] = false;
	}
	NFAGraph output;
	output.start = input.start;
	CollectValidEdges(input, output, input.start, input.start, visited);
	return output;
}
void CollectValidEdges(NFAGraph& input, NFAGraph& output, int from, int node,
					   vector<bool>& visited)
{
	if (visited.at(node))
	{
		return;
	}
	else
	{
		visited.at(node) = true;
		auto edges = input.adj.at(node);
		for (auto& edge : edges)
		{
			if (edge.IsEpsilon())
			{
				CollectValidEdges(input, output, from, edge.to, visited);
			}
			else
			{
				output.AddEdge(NFAEdge(from, edge.to, edge.pattern));
				CollectValidEdges(input, output, edge.to, edge.to, visited);
			}
		}
	}
}
NFAGraph RenumberGraph(NFAGraph& input)
{
	unordered_map<int, int> map;
	vector<bool> visited(input.NodeCount());
	for (int i = 0, n = visited.size(); i < n; i++)
	{
		visited.at(i) = false;
	}
	RenumberSubgraph(input, input.start, visited, map);
	auto inputEdges = input.GetEdges();
	NFAGraph output;
	output.start = map[input.start];
	for (auto& edge : inputEdges)
	{
		output.AddEdge(NFAEdge(map[edge.from], map[edge.to], edge.pattern));
	}
	return output;
}
void RenumberSubgraph(NFAGraph& input, int node, vector<bool>& visited,
					  unordered_map<int, int>& map)
{
	if (visited.at(node))
	{
		return;
	}
	else
	{
		visited.at(node) = true;
		bool found = map.find(node) != map.end();
		if (!found)
		{
			int n = map.size();
			map.insert({node, n});
		}
		auto edges = input.adj.at(node);
		for (auto& edge : edges)
		{
			RenumberSubgraph(input, edge.to, visited, map);
		}
	}
}
