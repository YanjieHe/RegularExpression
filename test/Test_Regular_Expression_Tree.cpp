#include "REJsonSerializer.hpp"
#include <catch2/catch.hpp>
#include <iostream>
#include <iomanip>
#include <fstream>

using std::cout;
using std::endl;
using std::ifstream;
using namespace regex::notations;

TEST_CASE("test regaulr expression syntax tree", "[RegularExpression]")
{
	string folder = "../test/json/";
	SECTION("regular expression 1")
	{
		auto e1 = (Symbol(U'a') + Symbol(U'b')) | (Symbol(U'b') + Symbol(U'a'));
		REJsonSerializer serializer;
		auto actual = serializer.VisitRegularExpression(e1);
		Json expected;
		ifstream stream(folder + "TestRE1.json");
		stream >> expected;

		REQUIRE(actual == expected);
	}
	SECTION("regular expression 2")
	{
		auto e2 = Symbol(U'a')->Many() + Symbol(U'b')->Many();
		REJsonSerializer serializer;
		auto actual = serializer.VisitRegularExpression(e2);
		Json expected;
		ifstream stream(folder + "TestRE2.json");
		stream >> expected;

		REQUIRE(actual == expected);
	}
	SECTION("regular expression 3")
	{
		auto e3 = (Range(U'0', U'9') | Symbol(U'a'))->Many();
		REJsonSerializer serializer;
		auto actual = serializer.VisitRegularExpression(e3);
		Json expected;
		ifstream stream(folder + "TestRE3.json");
		stream >> expected;

		REQUIRE(actual == expected);
	}
}