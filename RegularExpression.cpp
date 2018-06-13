#include "RegularExpression.hpp"

RegularExpression::RegularExpression(RegularExpressionKind kind)
	: kind{kind}
{
}

AlternationExpression::AlternationExpression(RegularExpPtr left,
											 RegularExpPtr right)
	: RegularExpression(RegularExpressionKind::Alternation)
	, left{left}
	, right{right}
{
}

ConcatenationExpression::ConcatenationExpression(RegularExpPtr left,
												 RegularExpPtr right)
	: RegularExpression(RegularExpressionKind::Concatenation)
	, left{left}
	, right{right}
{
}

KleeneStarExpression::KleeneStarExpression(RegularExpPtr innerExp)
	: RegularExpression(RegularExpressionKind::KleeneStar)
	, innerExp{innerExp}
{
}

SymbolExpression::SymbolExpression(wchar_t character)
	: RegularExpression(RegularExpressionKind::Symbol)
	, character(character)
{
}
RegularExpPtr operator|(RegularExpPtr x, RegularExpPtr y)
{
	return make_shared<AlternationExpression>(x, y);
}
RegularExpPtr operator>>(RegularExpPtr x, RegularExpPtr y)
{
	return make_shared<ConcatenationExpression>(x, y);
}
RegularExpPtr RE::Symbol(wchar_t c)
{
	return make_shared<SymbolExpression>(c);
}
RegularExpPtr RE::Literal(std::__cxx11::wstring text)
{
	if (text.size() == 1)
	{
		return Symbol(text[0]);
	}
	else
	{
		return Symbol(text[0]) >> Literal(text.substr(1));
	}
}
RegularExpPtr RE::Many(RegularExpPtr x)
{
	return make_shared<KleeneStarExpression>(x);
}