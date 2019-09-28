#include "RegularExpression.hpp"

namespace regex {
RegularExpression::RegularExpression(RegularExpressionKind kind)
	: kind{kind}
{
}

AlternationExpression::AlternationExpression(const Ptr<RegularExpression>& left,
											 const Ptr<RegularExpression>& right)
	: RegularExpression(RegularExpressionKind::Alternation)
	, left{left}
	, right{right}
{
}

ConcatenationExpression::ConcatenationExpression(const Ptr<RegularExpression>& left,
												 const Ptr<RegularExpression>& right)
	: RegularExpression(RegularExpressionKind::Concatenation)
	, left{left}
	, right{right}
{
}

KleeneStarExpression::KleeneStarExpression(const Ptr<RegularExpression>& innerExp)
	: RegularExpression(RegularExpressionKind::KleeneStar)
	, innerExp{innerExp}
{
}

SymbolExpression::SymbolExpression(int32_t character)
	: RegularExpression(RegularExpressionKind::Symbol)
	, character(character)
{
}
Ptr<RegularExpression> operator|(const Ptr<RegularExpression>& x, const Ptr<RegularExpression>& y)
{
	return std::make_shared<AlternationExpression>(x, y);
}
Ptr<RegularExpression> operator+(const Ptr<RegularExpression>& x, const Ptr<RegularExpression>& y)
{
	return std::make_shared<ConcatenationExpression>(x, y);
}
Ptr<RegularExpression> Symbol(int32_t c)
{
	return std::make_shared<SymbolExpression>(c);
}
Ptr<RegularExpression> Literal(const String& text)
{
	Ptr<RegularExpression> res;
	int N = text.size();
	for(int i = N - 1; i >= 0; i--) {
		auto c = text[i];
		if(res) {
			res = Symbol(c) + res;
		} else {
			res = Symbol(c);
		}
	}
	return res;
}
Ptr<RegularExpression> Many(const Ptr<RegularExpression>& x)
{
	return std::make_shared<KleeneStarExpression>(x);
}

} // namespace regex
