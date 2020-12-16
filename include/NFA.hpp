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
	typedef unordered_map<UnicodeRange, unordered_set<StateID>> Row;
	typedef vector<Row> Table;

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

	void FindNextStates(int start, int vertex, UnicodeRange pattern,
						Table& table, vector<bool>& visited);

	Row ComputeNextRow(size_t vertex, Table& table);
	Row ComputeVerticesNextRow(std::set<StateID> vertices, Table& table);
	vector<DFATableRow> EpsilonClosure();

private:
	void CollectPatterns();
};
void ViewRow();

} // namespace regex
#endif // NFA_HPP
