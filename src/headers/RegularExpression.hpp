#ifndef REGULAREXPRESSION_HPP
#define REGULAREXPRESSION_HPP

#include <memory>
#include <string>
#include <cstdint>
#include <vector>

namespace regex
{
using std::vector;
using String = std::basic_string<int32_t>;

template <typename T>
using Ptr = std::shared_ptr<T>;

class RegularExpression;
class AlternationExpression;
class ConcatenationExpression;
class KleeneStarExpression;
class SymbolExpression;

enum class RegularExpressionKind
{
	Alternation,
	Concatenation,
	KleeneStar,
	Symbol
};

class RegularExpression
{
public:
	RegularExpressionKind kind;
	RegularExpression(RegularExpressionKind kind);
	virtual ~RegularExpression() = default;
};
class AlternationExpression : public RegularExpression
{
public:
	Ptr<RegularExpression> left;
	Ptr<RegularExpression> right;
	AlternationExpression(const Ptr<RegularExpression>& left, const Ptr<RegularExpression>& right);
};
class ConcatenationExpression : public RegularExpression
{
public:
	Ptr<RegularExpression> left;
	Ptr<RegularExpression> right;
	ConcatenationExpression(const Ptr<RegularExpression>& left, const Ptr<RegularExpression>& right);
};
class KleeneStarExpression : public RegularExpression
{
public:
	Ptr<RegularExpression> innerExp;
	KleeneStarExpression(const Ptr<RegularExpression>& innerExp);
};
class SymbolExpression : public RegularExpression
{
public:
	int32_t character;
	SymbolExpression(int32_t character);
};

Ptr<RegularExpression> operator|(const Ptr<RegularExpression>& x, const Ptr<RegularExpression>& y);
Ptr<RegularExpression> operator+(const Ptr<RegularExpression>& x, const Ptr<RegularExpression>& y);

Ptr<RegularExpression> Symbol(int32_t c);
Ptr<RegularExpression> Literal(const String& text);
Ptr<RegularExpression> Many(const Ptr<RegularExpression>& x);

} // namespace regex
#endif /* REGULAREXPRESSION_HPP */
