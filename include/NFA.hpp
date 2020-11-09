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

class NFASubgraph
{
public:
	size_t start;
	size_t end;

	NFASubgraph()
		: start{0}
		, end{0}
	{
	}
	NFASubgraph(size_t start, size_t end)
		: start{start}
		, end{end}
	{
	}
};

class NFA : public RegularExpressionVisitor<NFASubgraph>
{
public:
	unordered_map<Interval, PatternID, IntervalHash> intervalMap;
	Graph G;

	int startVertex;
	int endVertex;

	explicit NFA(const RegularExpression::Ptr& exp);

	NFASubgraph
		VisitAlternation(const AlternationExpression::Ptr& exp) override;
	NFASubgraph
		VisitConcatenation(const ConcatenationExpression::Ptr& exp) override;
	NFASubgraph VisitKleeneStar(const KleeneStarExpression::Ptr& exp) override;
	NFASubgraph VisitSymbol(const SymbolExpression::Ptr& exp) override;

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
