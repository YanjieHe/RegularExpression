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
NFAGraph::NFAGraph()
{
}
int NFAGraph::AddNode()
{
	int n = out.size();
	out.push_back(vector<NFAEdge>());
	isFinal.push_back(true);
	return n;
}
void NFAGraph::AddEdge(NFAEdge edge)
{
	out.at(edge.from).push_back(edge);
}
int NFAGraph::NodeCount()
{
	return out.size();
}
NFASubgraph::NFASubgraph(int start, int end)
	: start{start}
	, end{end}
{
}
NFA::NFA()
{
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