#ifndef TEST_REGULAR_EXPRESSION_TREE_HPP
#define TEST_REGULAR_EXPRESSION_TREE_HPP
#include <nlohmann/json.hpp>
#include "RegularExpression.hpp"

using namespace regex;
using std::string;
using Json = nlohmann::json;
using JsonMap = unordered_map<string, Json>;

class REJsonSerializer : public RegularExpressionVisitor<Json>
{
public:
	virtual Json VisitAlternation(const AlternationExpression::Ptr& exp);
	virtual Json VisitConcatenation(const ConcatenationExpression::Ptr& exp);
	virtual Json VisitKleeneStar(const KleeneStarExpression::Ptr& exp);
	virtual Json VisitSymbol(const SymbolExpression::Ptr& exp);
};

void TestRE1();
void TestRE2();
void TestRE3();

#endif // TEST_REGULAR_EXPRESSION_TREE_HPP