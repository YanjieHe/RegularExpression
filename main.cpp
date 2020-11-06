#include "RegularExpression.hpp"
#include "NFA.hpp"
#include "NFAViewer.hpp"
#include "Encoding.hpp"
#include <iostream>
using std::cout;
using std::endl;
using regex::Symbol;

void TestMatch(const regex::DFAMatrix& dfa, std::u32string text, bool expected)
{
	auto actual = dfa.Match(text);
	if (expected != actual)
	{
		cout << "ERROR!!!   ";
	}
	cout << "Test Match \"" << UTF32ToUTF8(text);
	cout << "\" expected: " << std::boolalpha << expected;
	cout << " actual: " << std::boolalpha << actual << endl;
}

void Test()
{
	auto e1 = (Symbol(U'a') + Symbol(U'b')) | (Symbol(U'b') + Symbol(U'a'));
	auto m1 = e1->Compile();
	TestMatch(m1, U"ab", true);
	TestMatch(m1, U"aa", false);
	TestMatch(m1, U"aba", false);
	TestMatch(m1, U"bab", false);
	TestMatch(m1, U"ba", true);
}

int main()
{
	auto exp = (Symbol(U'a') + Symbol(U'b')) | (Symbol(U'b') + Symbol(U'a'));
	regex::NFA nfa{exp};
	regex::NFAViewer::ViewNFA(nfa);
	auto dfaTable = nfa.EpsilonClosure();
	auto dfaGraph = regex::DFATableToDFAGraph(
		dfaTable, regex::GetPatternIDIntervalMap(nfa.intervalMap));
	cout << "# of nodes = " << dfaGraph.dfa.NodeCount() << endl;
	for (auto endState : dfaGraph.endStates)
	{
		cout << "end state: " << endState << endl;
	}
	for (auto edges : dfaGraph.dfa.adj)
	{
		for (auto edge : edges)
		{
			cout << edge.from << " -> " << edge.to << " : "
				 << UTF32ToUTF8(dfaGraph.patternIDToIntervals.at(edge.pattern)
									.ToString())
				 << endl;
		}
	}
	auto dfaMatrix = CreateDFAMatrix(dfaGraph);

	cout << std::boolalpha << dfaMatrix.Match(U"ab") << endl;
	cout << std::boolalpha << dfaMatrix.Match(U"aa") << endl;
	cout << std::boolalpha << dfaMatrix.Match(U"aba") << endl;
	cout << std::boolalpha << dfaMatrix.Match(U"bab") << endl;
	cout << std::boolalpha << dfaMatrix.Match(U"ba") << endl;

	cout << dfaMatrix.MatchFromBeginning(U"ab") << endl;
	cout << dfaMatrix.MatchFromBeginning(U"aa") << endl;
	cout << dfaMatrix.MatchFromBeginning(U"aba") << endl;
	cout << dfaMatrix.MatchFromBeginning(U"bab") << endl;
	cout << dfaMatrix.MatchFromBeginning(U"ba") << endl;
	cout << dfaMatrix.MatchFromBeginning(U"acb") << endl;
	cout << dfaMatrix.MatchFromBeginning(U"cab") << endl;

	cout << "find" << endl;
	cout << dfaMatrix.Find(U"aaba") << endl;
	// auto exp2 = regex::Literal(U"0123456789");
	// regex::NFA nfa2{exp2};
	// regex::NFAViewer::ViewNFA(nfa2);

	Test();

	return 0;
}
