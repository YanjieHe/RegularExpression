#ifndef NFA_HPP
#define NFA_HPP
#include "RegularExpression.hpp"
#include <string>
#include <vector>
#include <unordered_map>
using std::unordered_map;
using std::vector;
using std::wstring;

class NFAEdge
{
public:
	int from;
	int to;
	int pattern; // epsilon = -1
	NFAEdge(int from, int to, int pattern);
    bool IsEpsilon();
};

class NFAGraph
{
public:
	vector<vector<NFAEdge>> adj;
	vector<bool> isFinal;
	int start;
	NFAGraph();
	int AddNode();
	void AddEdge(NFAEdge edge);
	int NodeCount();
    vector<NFAEdge> GetEdges();
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
	void CreateGraph(RegularExpPtr exp);

private:
	NFASubgraph Convert(RegularExpPtr exp);
	NFASubgraph ConvertAlternation(AlternationExpPtr exp);
	NFASubgraph ConvertConcatenation(ConcatenationExpPtr exp);
	NFASubgraph ConvertKleenStar(KleeneStarExpPtr exp);
	NFASubgraph ConvertSymbol(SymbolExpPtr exp);
};

NFAGraph EliminateEpsilonEdges(NFAGraph& input);
void CollectValidEdges(NFAGraph& input, NFAGraph& output, int from, int node, vector<bool>& visited);
NFAGraph RenumberGraph(NFAGraph& input);
void RenumberSubgraph(NFAGraph& input, int node, vector<bool>& visited, unordered_map<int, int>& map);
#endif // NFA_HPP
