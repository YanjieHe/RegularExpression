#include <fstream>
#include <iomanip>
#include <iostream>

#include "REJsonSerializer.hpp"
#include <catch2/catch.hpp>

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using namespace regex::notations;

TEST_CASE("test DFA", "[DFA]")
{
    string folder = "../test/json/";
    SECTION("DFA 1")
    {
        auto e1 = (Symbol(U'a') + Symbol(U'b')) | (Symbol(U'b') + Symbol(U'a'));
        NFA nfa(e1);
        auto dfaTable = nfa.EpsilonClosure();
        auto dfa = DFATableRowsToDFAGraph(dfaTable, nfa.patterns, nfa.G, nfa.endVertex);
        DFAToDotFile(dfa, "DFA1.dot");
        auto actual = DFAToJson(dfa);
        Json expected;
        ifstream stream(folder + "TestDFA1.json");
        stream >> expected;

        REQUIRE(actual == expected);
    }
    SECTION("DFA 2")
    {
        auto e2 = Symbol(U'a')->Many() + Symbol(U'b')->Many();
        NFA nfa(e2);
        auto dfaTable = nfa.EpsilonClosure();
        auto dfa = DFATableRowsToDFAGraph(dfaTable, nfa.patterns, nfa.G, nfa.endVertex);
        DFAToDotFile(dfa, "DFA2.dot");
        auto actual = DFAToJson(dfa);
        Json expected;
        ifstream stream(folder + "TestDFA2.json");
        stream >> expected;

        REQUIRE(actual == expected);
    }
    SECTION("DFA 3")
    {
        auto e3 = (Range(U'0', U'9') | Symbol('a'))->Many();
        NFA nfa(e3);
        auto dfaTable = nfa.EpsilonClosure();
        auto dfa = DFATableRowsToDFAGraph(dfaTable, nfa.patterns, nfa.G, nfa.endVertex);
        DFAToDotFile(dfa, "DFA3.dot");
        auto actual = DFAToJson(dfa);
        Json expected;
        ifstream stream(folder + "TestDFA3.json");
        stream >> expected;
        REQUIRE(actual == expected);
    }
    SECTION("DFA 4")
    {
        auto e4 = (Range(U'0', U'9')->Many() | (LineBegin() + Symbol(U'a')->Many()));
        NFA nfa(e4);
        auto dfaTable = nfa.EpsilonClosure();
        auto dfa = DFATableRowsToDFAGraph(dfaTable, nfa.patterns, nfa.G, nfa.endVertex);
        DFAToDotFile(dfa, "DFA4.dot");
        auto actual = DFAToJson(dfa);
        Json expected;
        ifstream stream(folder + "TestDFA4.json");
        stream >> expected;
        REQUIRE(actual == expected);
    }
}
