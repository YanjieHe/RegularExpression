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
        auto sunset = Literal(U"落霞");
        auto autumnWater = Literal(U"秋水");
        auto matrixSunset = sunset->Compile();
        auto matrixAutumnWater = autumnWater->Compile();
        u32string s = U"落霞与孤鹜齐飞，秋水共长天一色。";

        REQUIRE(matrixSunset.Search(s.begin(), s.end()) == s.begin());
        REQUIRE(matrixAutumnWater.Search(s.begin(), s.end()) == s.begin() + 8);
    }
}