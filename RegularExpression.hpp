#ifndef REGULAREXPRESSION_HPP
#define REGULAREXPRESSION_HPP

#include <memory>
#include <string>
#include <vector>

using std::make_shared;
using std::shared_ptr;
using std::vector;
using std::wstring;

class RegularExpression;
class AlternationExpression;
class ConcatenationExpression;
class KleeneStarExpression;
class SymbolExpression;

using RegularExpPtr = shared_ptr<RegularExpression>;
using AlternationExpPtr = shared_ptr<AlternationExpression>;
using ConcatenationExpPtr = shared_ptr<ConcatenationExpression>;
using KleeneStarExpPtr = shared_ptr<KleeneStarExpression>;
using SymbolExpPtr = shared_ptr<SymbolExpression>;

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
	RegularExpPtr left;
	RegularExpPtr right;
	AlternationExpression(RegularExpPtr left, RegularExpPtr right);
};
class ConcatenationExpression : public RegularExpression
{
public:
	RegularExpPtr left;
	RegularExpPtr right;
	ConcatenationExpression(RegularExpPtr left, RegularExpPtr right);
};
class KleeneStarExpression : public RegularExpression
{
public:
	RegularExpPtr innerExp;
	KleeneStarExpression(RegularExpPtr innerExp);
};
class SymbolExpression : public RegularExpression
{
public:
	wchar_t character;
	SymbolExpression(wchar_t character);
};

RegularExpPtr operator|(RegularExpPtr x, RegularExpPtr y);
RegularExpPtr operator>>(RegularExpPtr x, RegularExpPtr y);
namespace RE
{
RegularExpPtr Symbol(wchar_t c);
RegularExpPtr Literal(wstring text);
RegularExpPtr Many(RegularExpPtr x);
} // namespace RE
#endif /* REGULAREXPRESSION_HPP */