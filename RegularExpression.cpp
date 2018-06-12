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