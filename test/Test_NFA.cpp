#include "REJsonSerializer.hpp"
#include "NFA.hpp"
#include "Encoding.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <catch2/catch.hpp>

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using namespace regex::notations;

TEST_CASE("test NFA", "[NFA]")
{
	string folder = "../test/json/";
	SECTION("NFA 1")
	{
		auto e1 = (Symbol(U'a') + Symbol(U'b')) | (Symbol(U'b') + Symbol(U'a'));
		NFA nfa(e1);
		auto actual = NFAToJson(nfa);
		Json expected;
		ifstream stream(folder + "TestNFA1.json");
		stream >> expected;

		REQUIRE(actual == expected);

		NFAToDotFile(nfa, "NFA1.dot");
	}
	SECTION("NFA 2")
	{
		auto e2 = Symbol(U'a')->Many() + Symbol(U'b')->Many();
		NFA nfa(e2);
		auto actual = NFAToJson(nfa);
		Json expected;
		ifstream stream(folder + "TestNFA2.json");
		stream >> expected;

		REQUIRE(actual == expected);

		NFAToDotFile(nfa, "NFA2.dot");
	}
	SECTION("NFA 3")
	{
		auto e3 = (Range(U'0', U'9') | Symbol(U'a'))->Many();
		NFA nfa(e3);
		auto actual = NFAToJson(nfa);
		Json expected;
		ifstream stream(folder + "TestNFA3.json");
		stream >> expected;

		REQUIRE(actual == expected);

		NFAToDotFile(nfa, "NFA3.dot");
	}
	SECTION("NFA 4")
	{
		auto e4 =
			(Range(U'0', U'9')->Many() | (LineBegin() + Symbol(U'a')->Many()));
		NFA nfa(e4);
		auto actual = NFAToJson(nfa);
		Json expected;
		ifstream stream(folder + "TestNFA4.json");
		stream >> expected;

		REQUIRE(actual == expected);

		NFAToDotFile(nfa, "NFA4.dot");
	}
}