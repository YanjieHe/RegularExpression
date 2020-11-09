#include "RegularExpression.hpp"
#include "NFA.hpp"

namespace regex
{
using std::make_shared;

DFAMatrix RegularExpression::Compile()
{
	shared_ptr<RegularExpression> exp = shared_from_this();
	NFA nfa{exp};
	auto dfaTable = nfa.EpsilonClosure();
	auto dfaGraph = regex::DFATableToDFAGraph(
		dfaTable, regex::GetPatternIDIntervalMap(nfa.intervalMap), nfa.endVertex);
	return CreateDFAMatrix(dfaGraph);
}

AlternationExpression::AlternationExpression(
	const RegularExpression::Ptr& left, const RegularExpression::Ptr& right)
	: left{left}
	, right{right}
{
}

RegularExpressionKind AlternationExpression::Kind() const
{
	return RegularExpressionKind::Alternation;
}

ConcatenationExpression::ConcatenationExpression(
	const RegularExpression::Ptr& left, const RegularExpression::Ptr& right)
	: left{left}
	, right{right}
{
}

RegularExpressionKind ConcatenationExpression::Kind() const
{
	return RegularExpressionKind::Concatenation;
}

KleeneStarExpression::KleeneStarExpression(
	const RegularExpression::Ptr& innerExp)
	: innerExp{innerExp}
{
}

RegularExpressionKind KleeneStarExpression::Kind() const
{
	return RegularExpressionKind::KleeneStar;
}

SymbolExpression::SymbolExpression(char32_t character)
	: character(character)
{
}

RegularExpressionKind SymbolExpression::Kind() const
{
	return RegularExpressionKind::Symbol;
}

RegularExpression::Ptr operator|(const RegularExpression::Ptr& x,
								 const RegularExpression::Ptr& y)
{
	return make_shared<AlternationExpression>(x, y);
}
RegularExpression::Ptr operator+(const RegularExpression::Ptr& x,
								 const RegularExpression::Ptr& y)
{
	return make_shared<ConcatenationExpression>(x, y);
}
RegularExpression::Ptr Symbol(char32_t c)
{
	return make_shared<SymbolExpression>(c);
}
RegularExpression::Ptr Literal(const u32string& text)
{
	RegularExpression::Ptr res;
	int N = text.size();
	for (int i = N - 1; i >= 0; i--)
	{
		char32_t c = text[i];
		if (res)
		{
			res = Symbol(c) + res;
		}
		else
		{
			res = Symbol(c);
		}
	}
	return res;
}

RegularExpression::Ptr Many(const RegularExpression::Ptr& x)
{
	return make_shared<KleeneStarExpression>(x);
}

} // namespace regex
