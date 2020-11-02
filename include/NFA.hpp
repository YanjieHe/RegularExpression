#ifndef NFA_HPP
#define NFA_HPP
#include <string>
#include <vector>
#include <unordered_map>
#include "RegularExpression.hpp"

namespace regex
{

using std::unordered_map;
using std::vector;

class NFAEdge
{
public:
	int from;
	int to;
	int pattern; // epsilon = -1

	NFAEdge();
	NFAEdge(int from, int to, int pattern);
	inline bool IsEpsilon() const
	{
		return pattern == -1;
	}
	static const int EPSILON = -1;
};

class NFAGraph
{
public:
	vector<vector<NFAEdge>> adj;
	int start;

	NFAGraph();
	int AddNode();
	void AddEdge(const NFAEdge& edge);
	int NodeCount() const;
	vector<NFAEdge> GetEdges() const;
	const vector<NFAEdge>& Adj(int index) const
	{
		return adj.at(index);
	}
};

class NFASubgraph
{
public:
	int start;
	int end;

	NFASubgraph();
	NFASubgraph(int start, int end);
};

class NFA
{
public:
	NFAGraph G;

	NFA();
	explicit NFA(const RegularExpression::Ptr& exp);
	void CreateGraph(const RegularExpression::Ptr& exp);

	void EliminateEpsilonEdges();

private:
	NFASubgraph Convert(const RegularExpression::Ptr& exp);
	NFASubgraph ConvertAlternation(const AlternationExpression::Ptr& exp);
	NFASubgraph ConvertConcatenation(const ConcatenationExpression::Ptr& exp);
	NFASubgraph ConvertKleenStar(const KleeneStarExpression::Ptr& exp);
	NFASubgraph ConvertSymbol(const SymbolExpression::Ptr& exp);

	void Search(vector<bool>& visited, int edge);
};

NFAGraph EliminateEpsilonEdges(const NFAGraph& input);

void CollectValidEdges(NFAGraph& input, NFAGraph& output, int from, int node,
					   vector<bool>& visited);
NFAGraph RenumberGraph(NFAGraph& input);
void RenumberSubgraph(NFAGraph& input, int node, vector<bool>& visited,
					  unordered_map<int, int>& map);

} // namespace regex
#endif // NFA_HPP
