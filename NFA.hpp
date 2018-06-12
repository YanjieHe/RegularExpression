#ifndef NFA_HPP
#define NFA_HPP
#include "RegularExpression.hpp"
#include <string>
#include <vector>
using std::vector;
using std::wstring;

class NFAEdge
{
public:
	int from;
	int to;
	int pattern; // epsilon = -1
	NFAEdge(int from, int to, int pattern);
};

class NFAGraph
{
public:
	vector<vector<NFAEdge>> out;
	vector<bool> isFinal;
	NFAGraph();
	int AddNode();
	void AddEdge(NFAEdge edge);
	int NodeCount();
};

class NFASubgraph
{
public:
	int start;
	int end;
	NFASubgraph(int start, int end);
};

class NFA
{
public:
	NFAGraph G;
	NFA();
	NFASubgraph Convert(RegularExpPtr exp);
	NFASubgraph ConvertAlternation(AlternationExpPtr exp);
	NFASubgraph ConvertConcatenation(ConcatenationExpPtr exp);
	NFASubgraph ConvertKleenStar(KleeneStarExpPtr exp);
	NFASubgraph ConvertSymbol(SymbolExpPtr exp);
};
#endif // NFA_HPP