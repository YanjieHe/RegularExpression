#ifndef NFA_HPP
#define NFA_HPP
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "RegularExpression.hpp"
#include <cstdint>
#include "DFA.hpp"
#include <optional>

namespace regex
{

using std::unordered_map;
using std::unordered_set;
using std::vector;
using std::optional;

class NFASubgraph
{
public:
	StateID start;
	StateID end;

	NFASubgraph()
		: start{0}
		, end{0}
	{
	}
	NFASubgraph(StateID start, StateID end)
		: start{start}
		, end{end}
	{
	}
};

class NFA : public RegularExpressionVisitor<NFASubgraph>
{
public:
	Graph G;
	UnicodePatterns patterns;
	size_t startVertex;
	size_t endVertex;

	explicit NFA(const RegularExpression::Ptr& exp);

	NFASubgraph
		VisitAlternation(const AlternationExpression::Ptr& exp) override;
	NFASubgraph
		VisitConcatenation(const ConcatenationExpression::Ptr& exp) override;
	NFASubgraph VisitKleeneStar(const KleeneStarExpression::Ptr& exp) override;
	NFASubgraph VisitSymbol(const SymbolExpression::Ptr& exp) override;

	void Search(
		int start, int node, UnicodeRange pattern,
		vector<unordered_map<UnicodeRange, unordered_set<StateID>>>& table,
		vector<bool>& visited);

	unordered_map<UnicodeRange, unordered_set<StateID>> ComputeRow(
		size_t node,
		vector<unordered_map<UnicodeRange, unordered_set<StateID>>>& table);
	unordered_map<UnicodeRange, unordered_set<StateID>> ComputeRowOfNodes(
		std::set<StateID> nodes,
		vector<unordered_map<UnicodeRange, unordered_set<StateID>>>& table);
	vector<DFATableRow> EpsilonClosure();

private:
	void CollectPatterns();
};
void ViewRow();

} // namespace regex
#endif // NFA_HPP
