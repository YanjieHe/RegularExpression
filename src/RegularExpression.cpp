#include "RegularExpression.hpp"
#include "NFA.hpp"

namespace regex
{
using std::make_shared;

DFAMatrix RegularExpression::Compile()
{
	RegularExpression::Ptr exp = shared_from_this();
	NFA nfa{exp};
	auto dfaTable = nfa.EpsilonClosure();
	auto dfaGraph =
		regex::DFATableRowsToDFAGraph(dfaTable, nfa.patterns, nfa.G, nfa.endVertex);
	return CreateDFAMatrix(dfaGraph);
}

RegularExpression::Ptr RegularExpression::Many()
{
	RegularExpression::Ptr exp = shared_from_this();
	return make_shared<KleeneStarExpression>(exp);
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

SymbolExpression::SymbolExpression(UnicodeRange range)
	: range{range}
{
}

RegularExpressionKind SymbolExpression::Kind() const
{
	return RegularExpressionKind::Symbol;
}

namespace notations
{
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
	return make_shared<SymbolExpression>(
		UnicodeRange(RangeType::CharacterRange, c, c));
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
RegularExpression::Ptr Range(char32_t lower, char32_t upper)
{
	return make_shared<SymbolExpression>(
		UnicodeRange(RangeType::CharacterRange, lower, upper));
}
RegularExpression::Ptr LineBegin()
{
	return make_shared<SymbolExpression>(
		UnicodeRange(RangeType::LineBegin, char32_t{0}, char32_t{0}));
}
RegularExpression::Ptr LineEnd()
{
	return make_shared<SymbolExpression>(
		UnicodeRange(RangeType::LineEnd, char32_t{0}, char32_t{0}));
}
} // namespace notations
} // namespace regex
