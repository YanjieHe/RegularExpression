#ifndef NFA_HPP
#define NFA_HPP
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "RegularExpression.hpp"
#include <cstdint>
#include "DFA.hpp"

namespace regex
{

using std::unordered_map;
using std::unordered_set;
using std::vector;
using PatternID = int;

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
	int end;

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
	const int EPSILON = -1;
	unordered_map<Interval, PatternID, IntervalHash> intervalMap;
	NFAGraph G;

	explicit NFA(const RegularExpression::Ptr& exp);
	void CreateGraph(const RegularExpression::Ptr& exp);

private:
	NFASubgraph Convert(const RegularExpression::Ptr& exp);
	NFASubgraph ConvertAlternation(const AlternationExpression::Ptr& exp);
	NFASubgraph ConvertConcatenation(const ConcatenationExpression::Ptr& exp);
	NFASubgraph ConvertKleenStar(const KleeneStarExpression::Ptr& exp);
	NFASubgraph ConvertSymbol(const SymbolExpression::Ptr& exp);

public:
	PatternID RecordInterval(char32_t lower, char32_t upper);

	void Search(int start, int node, int pattern,
				vector<unordered_map<PatternID, unordered_set<int>>>& table,
				vector<bool>& visited);
	unordered_map<int32_t, unordered_set<int>>
		ComputeRow(int node,
				   vector<unordered_map<int32_t, unordered_set<int>>>& table);
	unordered_map<int32_t, unordered_set<int>> ComputeRowOfNodes(
		vector<int> nodes,
		vector<unordered_map<int32_t, unordered_set<int>>>& table);
	vector<DFATableRow> EpsilonClosure();
};

unordered_map<PatternID, Interval> GetPatternIDIntervalMap(
	const unordered_map<Interval, PatternID, IntervalHash>& intervalMap);

void ViewNFA(const NFA& nfa);
} // namespace regex
#endif // NFA_HPP
