#include "Test_NFA.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include "Encoding.hpp"

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using namespace regex::notations;

Json DFAToJson(const DFA& dfa)
{
	vector<Json> endStatesJson;
	vector<Json> patternsJson;
	for (auto endState : dfa.endStates)
	{
		endStatesJson.push_back(endState);
	}
	return JsonMap(
		{{"end_states", endStatesJson}, {"graph", GraphToJson(dfa.G)}});
}

Json DFATableToJson(const vector<DFATableRow>& rows)
{
	vector<Json> rowsJson;
	for (auto row : rows)
	{
		vector<Json> indexJson;
		vector<vector<Json>> nextStatesJson;
		for (auto i : row.index)
		{
			indexJson.push_back(i);
		}
		for (auto nextState : row.nextStates)
		{
			nextStatesJson.emplace_back();
			for (auto i : nextState)
			{
				nextStatesJson.back().push_back(i);
			}
		}
		rowsJson.push_back(
			JsonMap({{"index", indexJson}, {"next_states", nextStatesJson}}));
	}
	return rowsJson;
}

void DFAToDotFile(const DFA& dfa, string path)
{
	const auto& G = dfa.G;
	auto edges = G.GetEdges();
	ofstream out(path);
	out << "digraph {" << endl;
	out << "rankdir=LR;" << endl;
	out << "size=\"8,5;\"" << endl;
	out << "node [shape = doublecircle];";

	for (auto endState : dfa.endStates)
	{
		out << endState << ";";
	}
	out << endl;
	out << "node [shape = point ]; start;" << endl;
	out << "node [shape = circle];" << endl;
	out << "start -> 0;" << endl;
	for (const auto& edge : edges)
	{
		out << edge.from << " -> " << edge.to;
		out << "[label=\"" << encoding::utf32_to_utf8(edge.pattern.ToString())
			<< "\"];" << endl;
	}
	out << "}" << endl;
}

void TestDFA1()
{
	auto e1 = (Symbol(U'a') + Symbol(U'b')) | (Symbol(U'b') + Symbol(U'a'));
	NFA nfa(e1);
	auto dfaTable = nfa.EpsilonClosure();
	auto dfa = DFATableToDFAGraph(dfaTable, nfa.patterns, nfa.G, nfa.endVertex);
	DFAToDotFile(dfa, "DFA1.dot");
	auto actual = DFAToJson(dfa);
	Json expected;
	ifstream stream("../test/TestDFA1.json");
	stream >> expected;
	cout << __FUNCTION__ << " "
		 << ((actual == expected) ? "passed!"
								  : "failed! <<<<<<<<<<<<<<<<<<<<<<<")
		 << endl;
}

void TestDFA2()
{
	auto e2 = Symbol(U'a')->Many() + Symbol(U'b')->Many();
	NFA nfa(e2);
	auto dfaTable = nfa.EpsilonClosure();
	auto dfa = DFATableToDFAGraph(dfaTable, nfa.patterns, nfa.G, nfa.endVertex);
	DFAToDotFile(dfa, "DFA2.dot");
	auto actual = DFAToJson(dfa);
	Json expected;
	ifstream stream("../test/TestDFA2.json");
	stream >> expected;
	cout << __FUNCTION__ << " "
		 << ((actual == expected) ? "passed!"
								  : "failed! <<<<<<<<<<<<<<<<<<<<<<<")
		 << endl;
}

void TestDFA3()
{
	auto e3 = (Range(U'0', U'9') | Symbol('a'))->Many();
	NFA nfa(e3);
	auto dfaTable = nfa.EpsilonClosure();
	auto dfa = DFATableToDFAGraph(dfaTable, nfa.patterns, nfa.G, nfa.endVertex);
	DFAToDotFile(dfa, "DFA3.dot");
	auto actual = DFAToJson(dfa);
	Json expected;
	ifstream stream("../test/TestDFA3.json");
	stream >> expected;
	cout << __FUNCTION__ << " "
		 << ((actual == expected) ? "passed!"
								  : "failed! <<<<<<<<<<<<<<<<<<<<<<<")
		 << endl;
}

void TestDFA4()
{
	auto e4 =
		(Range(U'0', U'9')->Many() | (LineBegin() + Symbol(U'a')->Many()));
	NFA nfa(e4);
	auto dfaTable = nfa.EpsilonClosure();
	auto dfa = DFATableToDFAGraph(dfaTable, nfa.patterns, nfa.G, nfa.endVertex);
	DFAToDotFile(dfa, "DFA4.dot");
	auto actual = DFAToJson(dfa);
	Json expected;
	ifstream stream("../test/TestDFA4.json");
	stream >> expected;
	cout << __FUNCTION__ << " "
		 << ((actual == expected) ? "passed!"
								  : "failed! <<<<<<<<<<<<<<<<<<<<<<<")
		 << endl;
}