#ifndef NFA_HPP
#define NFA_HPP
#include <string>
#include <vector>
#include <unordered_map>
#include "RegularExpression.hpp"

namespace regex {

using std::unordered_map;
using std::vector;

class NFAEdge
{
private:
	int from;
	int to;
	int pattern; // epsilon = -1
public:
	NFAEdge();
	NFAEdge(int from, int to, int pattern);
    inline bool IsEpsilon() const {
		return pattern == -1;
	}
	inline int From() const {
		return from;
	}
	inline int To() const {
		return to;
	}
	inline int Pattern() const {
		return pattern;
	}
	static const int EPSILON = -1;
};

class NFAGraph
{
private:
	vector<vector<NFAEdge>> adj;
	int start;
public:
	NFAGraph();
	int AddNode();
	void AddEdge(const NFAEdge& edge);
	int NodeCount() const;
    vector<NFAEdge> GetEdges() const;
	int& Start() {
		return start;
	}
	const int& Start() const {
		return start;
	}
	const vector<NFAEdge>& Adj(int index) const {
		return adj.at(index);
	}
};

class NFASubgraph
{
private:
	int start;
	int end;
public:
	NFASubgraph();
	NFASubgraph(int start, int end);
	inline int Start() const {
		return start;
	}
	inline int End() const {
		return end;
	}
};

class NFA
{
private:
	NFAGraph G;
public:
	NFA();
	explicit NFA(const Ptr<RegularExpression>& exp);
	void CreateGraph(const Ptr<RegularExpression>& exp);

	const NFAGraph& Graph() const {
		return G;
	}

	void EliminateEpsilonEdges();
private:
	NFASubgraph Convert(const Ptr<RegularExpression>& exp);
	NFASubgraph ConvertAlternation(const Ptr<AlternationExpression>& exp);
	NFASubgraph ConvertConcatenation(const Ptr<ConcatenationExpression>& exp);
	NFASubgraph ConvertKleenStar(const Ptr<KleeneStarExpression>& exp);
	NFASubgraph ConvertSymbol(const Ptr<SymbolExpression>& exp);

	void Search(vector<bool>& visited, int edge);
};

NFAGraph EliminateEpsilonEdges(const NFAGraph& input);

void CollectValidEdges(NFAGraph& input, NFAGraph& output, int from, int node, vector<bool>& visited);
NFAGraph RenumberGraph(NFAGraph& input);
void RenumberSubgraph(NFAGraph& input, int node, vector<bool>& visited, unordered_map<int, int>& map);

} // namespace regex
#endif // NFA_HPP
