#include "REJsonSerializer.hpp"
#include <catch2/catch.hpp>
#include "NFA.hpp"
#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;
using namespace regex::notations;

TEST_CASE("Test Matching", "[DFAMatrix]")
{
	SECTION("Test Matching 1")
	{
		auto e1 = (Symbol(U'a') + Symbol(U'b')) | (Symbol(U'b') + Symbol(U'a'));
		auto matrix1 = e1->Compile();

		REQUIRE(matrix1.FullMatch(U"ab") == true);
		REQUIRE(matrix1.FullMatch(U"aa") == false);
		REQUIRE(matrix1.FullMatch(U"aba") == false);
		REQUIRE(matrix1.FullMatch(U"bab") == false);
		REQUIRE(matrix1.FullMatch(U"ba") == true);
	}
	SECTION("Test Matching 2")
	{
		auto e2 = Symbol(U'a')->Many() + Symbol(U'b')->Many();
		auto matrix2 = e2->Compile();

		REQUIRE(matrix2.FullMatch(U"aaaaa") == true);
		REQUIRE(matrix2.FullMatch(U"aaaaabb") == true);
		REQUIRE(matrix2.FullMatch(U"aaabbaa") == false);
	}
	SECTION("Test Matching 3")
	{
		auto e3 = (Range(U'0', U'9') | Symbol(U'a'))->Many();
		auto matrix3 = e3->Compile();

		REQUIRE(matrix3.FullMatch(U"1229a32") == true);
		REQUIRE(matrix3.FullMatch(U"aaa38a812a") == true);
		REQUIRE(matrix3.FullMatch(U"aa122bb23a") == false);
	}
	SECTION("Test Matching 4")
	{
		auto e4 =
			(Range(U'0', U'9')->Many() | (LineBegin() + Symbol(U'a')->Many()));
		auto matrix4 = e4->Compile();

		REQUIRE(matrix4.FullMatch(U"123ab") == false);
		REQUIRE(matrix4.FullMatch(U"aaa") == true);
		REQUIRE(matrix4.FullMatch(U"123") == true);
	}
	SECTION("Test RepeatExactly")
	{
		auto e =
			(Range(U'0', U'9')->Many()) + RepeatExactly(Literal(U"ABC"), 3);
		auto matrix = e->Compile();

		REQUIRE(matrix.FullMatch(U"321ABCABCABC") == true);
		REQUIRE(matrix.FullMatch(U"321ABCABC") == false);
	}
	SECTION("Test RepeatAtLeast")
	{
		auto e =
			(Range(U'0', U'9')->Many()) + RepeatAtLeast(Literal(U"ABC"), 3);
		auto matrix = e->Compile();

		REQUIRE(matrix.FullMatch(U"321ABCABCABC") == true);
		REQUIRE(matrix.FullMatch(U"321ABCABC") == false);
		REQUIRE(matrix.FullMatch(U"321ABCABCABCABC") == true);
	}
	SECTION("Test Repeat")
	{
		auto e = (Range(U'0', U'9')->Many()) + Repeat(Literal(U"ABC"), 2, 5);
		auto matrix = e->Compile();

		REQUIRE(matrix.FullMatch(U"321ABCABCABC") == true);
		REQUIRE(matrix.FullMatch(U"321ABCABC") == true);
		REQUIRE(matrix.FullMatch(U"321ABC") == false);
		REQUIRE(matrix.FullMatch(U"321ABCABCABCABC") == true);
		REQUIRE(matrix.FullMatch(U"321ABCABCABCABCABC") == true);
		REQUIRE(matrix.FullMatch(U"321ABCABCABCABCABCABC") == false);
	}
	SECTION("Test Matching from the middle of the string")
	{
		auto e = RepeatExactly(Range(U'0', U'9'), 3);
		auto matrix = e->Compile();

		REQUIRE(matrix.Match(U"abc321", 3, 6, true) == 3);
		REQUIRE(matrix.Match(U"abcefg321", 6, 9, true) == 3);
	}
}