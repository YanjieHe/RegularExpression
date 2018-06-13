#include "Test.hpp"
#include "NFA.hpp"
#include <iostream>
using std::endl;
using std::wcout;

void ShowGraph(NFAGraph& G, int node, vector<bool>& visited)
{
	if (visited.at(node))
	{
		return;
	}
	else
	{
		wcout << node << L": ";
		for (auto& edge : G.adj.at(node))
		{
			wchar_t character = edge.pattern == -1 ? L'ε' : edge.pattern;
			wcout << edge.to << L"(" << character << L")" << L" ";
		}
		wcout << endl;
		visited.at(node) = true;
		for (auto& edge : G.adj.at(node))
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
		for (auto& edge : G.adj.at(node))
		{
			wchar_t character = edge.pattern == -1 ? L'ε' : edge.pattern;
			wcout << edge.from << L" -> " << edge.to << L" [ label = \""
				  << character << L"\" ];" << endl;
		}
		visited.at(node) = true;
		for (auto& edge : G.adj.at(node))
		{
			DrawGraph(G, edge.to, visited);
		}
	}
}
void ViewRegularExpression(RegularExpPtr re)
{
	NFA nfa;
	nfa.CreateGraph(re);
	vector<bool> visited(nfa.G.NodeCount());
	for (int i = 0, n = visited.size(); i < n; i++)
	{
		visited[i] = false;
	}
	wcout << L"start = " << nfa.G.start << endl;
	DrawGraph(nfa.G, nfa.G.start, visited);
}
void Test1()
{
	wcout << "test1" << endl;
	RegularExpPtr re = make_shared<ConcatenationExpression>(
		make_shared<SymbolExpression>(L'a'),
		make_shared<SymbolExpression>(L'b'));
	ViewRegularExpression(re);
}
void Test2()
{
	wcout << "test2" << endl;
	RegularExpPtr re = make_shared<AlternationExpression>(
		make_shared<ConcatenationExpression>(
			make_shared<SymbolExpression>(L'a'),
			make_shared<SymbolExpression>(L'b')),
		make_shared<ConcatenationExpression>(
			make_shared<SymbolExpression>(L'c'),
			make_shared<SymbolExpression>(L'd')));
	ViewRegularExpression(re);
}
void Test3()
{
	wcout << "test3" << endl;
	RegularExpPtr re = make_shared<ConcatenationExpression>(
		make_shared<KleeneStarExpression>(make_shared<AlternationExpression>(
			make_shared<SymbolExpression>(L'a'),
			make_shared<SymbolExpression>(L'b'))),
		make_shared<ConcatenationExpression>(
			make_shared<SymbolExpression>(L'a'),
			make_shared<ConcatenationExpression>(
				make_shared<SymbolExpression>(L'b'),
				make_shared<SymbolExpression>(L'b'))));
	ViewRegularExpression(re);
}
void Test4()
{
	wcout << "test4" << endl;
	RegularExpPtr re = make_shared<ConcatenationExpression>(
		make_shared<KleeneStarExpression>(make_shared<AlternationExpression>(
			make_shared<SymbolExpression>(L'a'),
			make_shared<SymbolExpression>(L'b'))),
		make_shared<SymbolExpression>(L'a'));
	ViewRegularExpression(re);
}
void Test5()
{
	wcout << "test5" << endl;

	auto re = RE::Literal(L"knight");
	ViewRegularExpression(re);
}
void Test6()
{
	wcout << "test6" << endl;

	auto re = RE::Symbol(L'1') | RE::Symbol(L'2') | RE::Symbol(L'3') |
			  RE::Symbol(L'4') | RE::Symbol(L'5');
	ViewRegularExpression(re);
}
void Test7()
{
	wcout << "test7" << endl;
	auto re = RE::Symbol(L'1') | RE::Symbol(L'2') | RE::Symbol(L'3') |
			  RE::Symbol(L'4') | RE::Symbol(L'5');

	NFA nfa;
	nfa.CreateGraph(re);
	auto output = EliminateEpsilonEdges(nfa.G);
	auto G = RenumberGraph(output);
	wcout << L"count = " << G.NodeCount() << endl;
	vector<bool> visited(G.NodeCount());
	for (int i = 0, n = visited.size(); i < n; i++)
	{
		visited[i] = false;
	}
	wcout << L"start = " << G.start << endl;
	for (int i = 0, n = G.NodeCount(); i < n; i++)
	{
		if (visited.at(i) != true)
		{
			DrawGraph(G, i, visited);
		}
	}
}
