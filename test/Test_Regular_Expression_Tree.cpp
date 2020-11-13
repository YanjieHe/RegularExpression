#include "Test_Regular_Expression_Tree.hpp"
#include "Encoding.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>

using std::cout;
using std::endl;
using std::ifstream;
using namespace regex::notations;

Json REJsonSerializer::VisitAlternation(const AlternationExpression::Ptr& exp)
{
	return {{"kind", "Alternation"},
			{"left", VisitRegularExpression(exp->left)},
			{"right", VisitRegularExpression(exp->right)}};
}
Json REJsonSerializer::VisitConcatenation(
	const ConcatenationExpression::Ptr& exp)
{
	return {{"kind", "Concatenation"},
			{"left", VisitRegularExpression(exp->left)},
			{"right", VisitRegularExpression(exp->right)}};
}
Json REJsonSerializer::VisitKleeneStar(const KleeneStarExpression::Ptr& exp)
{
	return {{"kind", "KleeneStar"},
			{"inner_exp", VisitRegularExpression(exp->innerExp)}};
}
Json REJsonSerializer::VisitSymbol(const SymbolExpression::Ptr& exp)
{
	// exp->range.rangeType -> string
	return {{"kind", "Symbol"},
			{"lower", encoding::utf32_to_utf8(u32string({exp->range.lower}))},
			{"upper", encoding::utf32_to_utf8(u32string({exp->range.upper}))}};
}

void TestRE1()
{
	auto e1 = (Symbol(U'a') + Symbol(U'b')) | (Symbol(U'b') + Symbol(U'a'));
	REJsonSerializer serializer;
	auto actual = serializer.VisitRegularExpression(e1);
	Json expected;
	ifstream stream("../test/TestRE1.json");
	stream >> expected;
	cout << __FUNCTION__ << " "
		 << ((actual == expected) ? "passed!"
								  : "failed! <<<<<<<<<<<<<<<<<<<<<<<")
		 << endl;
}

void TestRE2()
{
	auto e2 = Symbol(U'a')->Many() + Symbol(U'b')->Many();
	REJsonSerializer serializer;
	auto actual = serializer.VisitRegularExpression(e2);
	Json expected;
	ifstream stream("../test/TestRE2.json");
	stream >> expected;
	cout << __FUNCTION__ << " "
		 << ((actual == expected) ? "passed!"
								  : "failed! <<<<<<<<<<<<<<<<<<<<<<<")
		 << endl;
}

void TestRE3()
{
	auto e3 = (Range(U'0', U'9') | Symbol(U'a'))->Many();
	REJsonSerializer serializer;
	auto actual = serializer.VisitRegularExpression(e3);
	Json expected;
	ifstream stream("../test/TestRE3.json");
	stream >> expected;
	cout << __FUNCTION__ << " "
		 << ((actual == expected) ? "passed!"
								  : "failed! <<<<<<<<<<<<<<<<<<<<<<<")
		 << endl;
}