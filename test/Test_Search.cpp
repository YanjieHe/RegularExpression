#include <iomanip>
#include <iostream>

#include "NFA.hpp"
#include "REJsonSerializer.hpp"
#include <catch2/catch.hpp>

using std::cout;
using std::endl;
using namespace regex::notations;

TEST_CASE("Test Searching", "[DFAMatrix]")
{
    SECTION("Test Searching Chinese")
    {
        auto e1 = Literal(U"落霞");
        auto e2 = Literal(U"秋水");
        auto matrix1 = e1->Compile();
        auto matrix2 = e2->Compile();
        u32string s = U"落霞与孤鹜齐飞，秋水共长天一色。";

        REQUIRE(matrix1.Search(s.begin(), s.end()) == s.begin());
        REQUIRE(matrix2.Search(s.begin(), s.end()) == s.begin() + 8);
    }
}