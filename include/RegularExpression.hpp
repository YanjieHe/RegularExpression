#ifndef REGULAR_EXPRESSION_HPP
#define REGULAR_EXPRESSION_HPP

#include <cstdint>
#include <exception>
#include <memory>
#include <string>
#include <vector>

#include "DFA.hpp"

namespace regex
{
    using std::shared_ptr;
    using std::static_pointer_cast;
    using std::u32string;
    using std::vector;

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

        RegularExpression::Ptr Many();
    };

    class AlternationExpression : public RegularExpression
    {
    public:
        typedef shared_ptr<AlternationExpression> Ptr;

        RegularExpression::Ptr left;
        RegularExpression::Ptr right;
        AlternationExpression(const RegularExpression::Ptr &left, const RegularExpression::Ptr &right);
        RegularExpressionKind Kind() const override;
    };

    class ConcatenationExpression : public RegularExpression
    {
    public:
        typedef shared_ptr<ConcatenationExpression> Ptr;

        RegularExpression::Ptr left;
        RegularExpression::Ptr right;
        ConcatenationExpression(const RegularExpression::Ptr &left, const RegularExpression::Ptr &right);
        RegularExpressionKind Kind() const override;
    };

    class KleeneStarExpression : public RegularExpression
    {
    public:
        typedef shared_ptr<KleeneStarExpression> Ptr;

        RegularExpression::Ptr innerExp;
        KleeneStarExpression(const RegularExpression::Ptr &innerExp);
        RegularExpressionKind Kind() const override;
    };

    class SymbolExpression : public RegularExpression
    {
    public:
        typedef shared_ptr<SymbolExpression> Ptr;

        UnicodeRange range;
        explicit SymbolExpression(UnicodeRange range);
        RegularExpressionKind Kind() const override;
    };

    template <typename ReturnType, typename... ArgTypes>
    class RegularExpressionVisitor
    {
    public:
        ReturnType VisitRegularExpression(const RegularExpression::Ptr &exp, ArgTypes... args)
        {
            switch (exp->Kind())
            {
            case RegularExpressionKind::Alternation:
            {
                return VisitAlternation(static_pointer_cast<AlternationExpression>(exp), args...);
            }
            case RegularExpressionKind::Concatenation:
            {
                return VisitConcatenation(static_pointer_cast<ConcatenationExpression>(exp), args...);
            }
            case RegularExpressionKind::KleeneStar:
            {
                return VisitKleeneStar(static_pointer_cast<KleeneStarExpression>(exp), args...);
            }
            case RegularExpressionKind::Symbol:
            {
                return VisitSymbol(static_pointer_cast<SymbolExpression>(exp), args...);
            }
            default:
            {
                // make the compiler happy
                throw std::runtime_error("unreachable case branch");
            }
            }
        }
        virtual ReturnType VisitAlternation(const AlternationExpression::Ptr &exp, ArgTypes... args) = 0;
        virtual ReturnType VisitConcatenation(const ConcatenationExpression::Ptr &exp, ArgTypes... args) = 0;
        virtual ReturnType VisitKleeneStar(const KleeneStarExpression::Ptr &exp, ArgTypes... args) = 0;
        virtual ReturnType VisitSymbol(const SymbolExpression::Ptr &exp, ArgTypes... args) = 0;
    };

    namespace notations
    {
        RegularExpression::Ptr operator|(const RegularExpression::Ptr &x, const RegularExpression::Ptr &y);
        RegularExpression::Ptr operator+(const RegularExpression::Ptr &x, const RegularExpression::Ptr &y);

        RegularExpression::Ptr Symbol(char32_t c);
        RegularExpression::Ptr Literal(const u32string &text);
        RegularExpression::Ptr Range(char32_t lower, char32_t upper);
        RegularExpression::Ptr LineBegin();
        RegularExpression::Ptr LineEnd();
        RegularExpression::Ptr RepeatExactly(const RegularExpression::Ptr &x, int times);
        RegularExpression::Ptr RepeatAtLeast(const RegularExpression::Ptr &x, int times);
        RegularExpression::Ptr Repeat(const RegularExpression::Ptr &x, int atLeast, int atMost);
    } // namespace notations

} // namespace regex
#endif /* REGULAR_EXPRESSION_HPP */