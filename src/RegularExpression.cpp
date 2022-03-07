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
        auto dfaGraph = DFATableRowsToDFAGraph(dfaTable, nfa.patterns, nfa.G, nfa.endVertex);
        return DFAMatrix(dfaGraph);
    }

    RegularExpression::Ptr RegularExpression::Many()
    {
        RegularExpression::Ptr exp = shared_from_this();
        return make_shared<KleeneStarExpression>(exp);
    }

    AlternationExpression::AlternationExpression(
        const RegularExpression::Ptr &left, const RegularExpression::Ptr &right)
        : left{left}, right{right} {}

    RegularExpressionKind AlternationExpression::Kind() const
    {
        return RegularExpressionKind::Alternation;
    }

    ConcatenationExpression::ConcatenationExpression(
        const RegularExpression::Ptr &left, const RegularExpression::Ptr &right)
        : left{left}, right{right} {}

    RegularExpressionKind ConcatenationExpression::Kind() const
    {
        return RegularExpressionKind::Concatenation;
    }

    KleeneStarExpression::KleeneStarExpression(const RegularExpression::Ptr &innerExp) : innerExp{innerExp} {}

    RegularExpressionKind KleeneStarExpression::Kind() const
    {
        return RegularExpressionKind::KleeneStar;
    }

    SymbolExpression::SymbolExpression(UnicodeRange range) : range{range} {}

    RegularExpressionKind SymbolExpression::Kind() const
    {
        return RegularExpressionKind::Symbol;
    }

    namespace notations
    {
        RegularExpression::Ptr operator|(const RegularExpression::Ptr &x, const RegularExpression::Ptr &y)
        {
            return make_shared<AlternationExpression>(x, y);
        }
        RegularExpression::Ptr operator+(const RegularExpression::Ptr &x, const RegularExpression::Ptr &y)
        {
            return make_shared<ConcatenationExpression>(x, y);
        }
        RegularExpression::Ptr Symbol(char32_t c)
        {
            return make_shared<SymbolExpression>(UnicodeRange(RangeType::CharacterRange, c, c));
        }
        RegularExpression::Ptr Literal(const u32string &text)
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
            return make_shared<SymbolExpression>(UnicodeRange(RangeType::CharacterRange, lower, upper));
        }
        RegularExpression::Ptr LineBegin()
        {
            return make_shared<SymbolExpression>(UnicodeRange(RangeType::LineBegin, char32_t{0}, char32_t{0}));
        }
        RegularExpression::Ptr LineEnd()
        {
            return make_shared<SymbolExpression>(UnicodeRange(RangeType::LineEnd, char32_t{0}, char32_t{0}));
        }
        RegularExpression::Ptr RepeatExactly(const RegularExpression::Ptr &x, int times)
        {
            auto result = x;
            for (int i = 0; i < times - 1; i++)
            {
                result = result + x;
            }
            return result;
        }
        RegularExpression::Ptr RepeatAtLeast(const RegularExpression::Ptr &x, int times)
        {
            return RepeatExactly(x, times) + x->Many();
        }
        RegularExpression::Ptr Repeat(const RegularExpression::Ptr &x, int atLeast, int atMost)
        {
            auto result = RepeatExactly(x, atLeast);
            for (int i = atLeast + 1; i <= atMost; i++)
            {
                result = result | RepeatExactly(x, i);
            }
            return result;
        }
    } // namespace notations
} // namespace regex
