#ifndef REGULAREXPRESSION_HPP
#define REGULAREXPRESSION_HPP

#include <memory>
#include <string>
#include <cstdint>
#include <vector>
#include "DFA.hpp"

namespace regex
{
using std::vector;
using std::u32string;
using std::shared_ptr;

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

class RegularExpression : public std::enable_shared_from_this<RegularExpression>
{
public:
	typedef shared_ptr<RegularExpression> Ptr;

	virtual RegularExpressionKind Kind() const = 0;

	DFAMatrix Compile();
};

class AlternationExpression : public RegularExpression
{
public:
	typedef shared_ptr<AlternationExpression> Ptr;

	RegularExpression::Ptr left;
	RegularExpression::Ptr right;
	AlternationExpression(const RegularExpression::Ptr& left,
						  const RegularExpression::Ptr& right);
	RegularExpressionKind Kind() const override;
};

class ConcatenationExpression : public RegularExpression
{
public:
	typedef shared_ptr<ConcatenationExpression> Ptr;

	RegularExpression::Ptr left;
	RegularExpression::Ptr right;
	ConcatenationExpression(const RegularExpression::Ptr& left,
							const RegularExpression::Ptr& right);
	RegularExpressionKind Kind() const override;
};

class KleeneStarExpression : public RegularExpression
{
public:
	typedef shared_ptr<KleeneStarExpression> Ptr;

	RegularExpression::Ptr innerExp;
	KleeneStarExpression(const RegularExpression::Ptr& innerExp);
	RegularExpressionKind Kind() const override;
};

class SymbolExpression : public RegularExpression
{
public:
	typedef shared_ptr<SymbolExpression> Ptr;

	char32_t character;
	SymbolExpression(char32_t character);
	RegularExpressionKind Kind() const override;
};

RegularExpression::Ptr operator|(const RegularExpression::Ptr& x,
								 const RegularExpression::Ptr& y);
RegularExpression::Ptr operator+(const RegularExpression::Ptr& x,
								 const RegularExpression::Ptr& y);

RegularExpression::Ptr Symbol(char32_t c);
RegularExpression::Ptr Literal(const u32string& text);
RegularExpression::Ptr Many(const RegularExpression::Ptr& x);

} // namespace regex
#endif /* REGULAREXPRESSION_HPP */
