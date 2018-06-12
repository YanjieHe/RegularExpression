#include "Test.hpp"
#include "NFA.hpp"

void ShowGraph(NFAGraph& G, int node, vector<bool>& visited)
{
	if (visited.at(node))
	{
		return;
	}
	else
	{
		wcout << node << L": ";
		for (auto& edge : G.out.at(node))
		{
			wchar_t character = edge.pattern == -1 ? L'ε' : edge.pattern;
			wcout << edge.to << L"(" << character << L")" << L" ";
		}
		wcout << endl;
		visited.at(node) = true;
		for (auto& edge : G.out.at(node))
		{
			ShowGraph(G, edge.to, visited);
		}
	}
}
void DrawGraph(NFAGraph& G, int node, vector<bool>& visited)
{
	if (visited.at(node))
	{
		return;
	}
	else
	{
		for (auto& edge : G.out.at(node))
		{
			wchar_t character = edge.pattern == -1 ? L'ε' : edge.pattern;
			wcout << edge.from << L" -> " << edge.to << L" [ label = \""
				  << character << L"\" ];" << endl;
		}
		visited.at(node) = true;
		for (auto& edge : G.out.at(node))
		{
			DrawGraph(G, edge.to, visited);
		}
	}
}
void Test1()
{
	RegularExpPtr re = make_shared<ConcatenationExpression>(
		make_shared<SymbolExpression>(L'a'),
		make_shared<SymbolExpression>(L'b'));
	NFA nfa;
	NFASubgraph subgraph = nfa.Convert(re);
	vector<bool> visited(nfa.G.NodeCount());
	for (int i = 0, n = visited.size(); i < n; i++)
	{
		visited[i] = false;
	}
	DrawGraph(nfa.G, subgraph.start, visited);
}
void Test2()
{
	RegularExpPtr re = make_shared<AlternationExpression>(
		make_shared<ConcatenationExpression>(
			make_shared<SymbolExpression>(L'a'),
			make_shared<SymbolExpression>(L'b')),
		make_shared<ConcatenationExpression>(
			make_shared<SymbolExpression>(L'c'),
			make_shared<SymbolExpression>(L'd')));
	NFA nfa;
	NFASubgraph subgraph = nfa.Convert(re);
	vector<bool> visited(nfa.G.NodeCount());
	for (int i = 0, n = visited.size(); i < n; i++)
	{
		visited[i] = false;
	}
	DrawGraph(nfa.G, subgraph.start, visited);
}
void Test3()
{
	RegularExpPtr re = make_shared<ConcatenationExpression>(
		make_shared<KleeneStarExpression>(make_shared<AlternationExpression>(
			make_shared<SymbolExpression>(L'a'),
			make_shared<SymbolExpression>(L'b'))),
		make_shared<ConcatenationExpression>(
			make_shared<SymbolExpression>(L'a'),
			make_shared<ConcatenationExpression>(
				make_shared<SymbolExpression>(L'b'),
				make_shared<SymbolExpression>(L'b'))));
	NFA nfa;
	NFASubgraph subgraph = nfa.Convert(re);
	vector<bool> visited(nfa.G.NodeCount());
	for (int i = 0, n = visited.size(); i < n; i++)
	{
		visited[i] = false;
	}
	DrawGraph(nfa.G, subgraph.start, visited);
}
void Test4()
{
	RegularExpPtr re = make_shared<ConcatenationExpression>(
		make_shared<KleeneStarExpression>(make_shared<AlternationExpression>(
			make_shared<SymbolExpression>(L'a'),
			make_shared<SymbolExpression>(L'b'))),
		make_shared<SymbolExpression>(L'a'));
	NFA nfa;
	NFASubgraph subgraph = nfa.Convert(re);
	vector<bool> visited(nfa.G.NodeCount());
	for (int i = 0, n = visited.size(); i < n; i++)
	{
		visited[i] = false;
	}
	wcout << L"start = " << subgraph.start << L", end = " << subgraph.end
		  << endl;
	DrawGraph(nfa.G, subgraph.start, visited);
}