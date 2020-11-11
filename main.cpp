#include "RegularExpression.hpp"
#include "NFA.hpp"
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
	cout << "Test Match \"" << encoding::utf32_to_utf8(text);
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

	auto e2 = regex::Many(Symbol(U'a'));
	auto nfa = regex::NFA(e2);
	auto dfaTable = nfa.EpsilonClosure();
	cout << "end vertices" << endl;
	auto dfaGraph =
		regex::DFATableToDFAGraph(dfaTable, nfa.patterns, nfa.endVertex);
	auto m2 = e2->Compile();
	cout << "match \"aaaaa\" " << m2.MatchFromBeginning(U"aaaaa", 0, true)
		 << endl;
	for (size_t i = 0; i < m2.matrix.size(); i++)
	{
		for (size_t j = 0; j < m2.matrix.front().size(); j++)
		{
			cout << m2.matrix.at(i).at(j) << " ";
		}
		cout << endl;
	}
	cout << "=============" << endl;
	TestMatch(m2, U"aaaaa", true);
	TestMatch(m2, U"aaaaabb", false);
}

// int main()
// {
// 	auto exp = (Symbol(U'a') + Symbol(U'b')) | (Symbol(U'b') + Symbol(U'a'));
// 	regex::NFA nfa{exp};
// 	regex::ViewNFA(nfa);
// 	auto dfaTable = nfa.EpsilonClosure();
// 	// auto dfaGraph = regex::DFATableToDFAGraph(
// 	// 	dfaTable, regex::GetPatternIDIntervalMap(nfa.intervalMap),
// 	// 	nfa.endVertex);
// 	// cout << "# of nodes = " << dfaGraph.dfa.NodeCount() << endl;
// 	// for (auto endState : dfaGraph.endStates)
// 	// {
// 	// 	cout << "end state: " << endState << endl;
// 	// }
// 	// for (auto edges : dfaGraph.dfa.adj)
// 	// {
// 	// 	for (auto edge : edges)
// 	// 	{
// 	// 		cout << edge.from << " -> " << edge.to << " : "
// 	// 			 <<
// 	// encoding::utf32_to_utf8(dfaGraph.patternIDToIntervals.at(edge.pattern)
// 	// 								.ToString())
// 	// 			 << endl;
// 	// 	}
// 	// }
// 	// auto dfaMatrix = regex::CreateDFAMatrix(dfaGraph);

// 	// cout << std::boolalpha << dfaMatrix.Match(U"ab") << endl;
// 	// cout << std::boolalpha << dfaMatrix.Match(U"aa") << endl;
// 	// cout << std::boolalpha << dfaMatrix.Match(U"aba") << endl;
// 	// cout << std::boolalpha << dfaMatrix.Match(U"bab") << endl;
// 	// cout << std::boolalpha << dfaMatrix.Match(U"ba") << endl;

// 	// // cout << dfaMatrix.MatchFromBeginning(U"ab", 0) << endl;
// 	// // cout << dfaMatrix.MatchFromBeginning(U"aa", 0) << endl;
// 	// // cout << dfaMatrix.MatchFromBeginning(U"aba", 0) << endl;
// 	// // cout << dfaMatrix.MatchFromBeginning(U"bab", 0) << endl;
// 	// // cout << dfaMatrix.MatchFromBeginning(U"ba", 0) << endl;
// 	// // cout << dfaMatrix.MatchFromBeginning(U"acb", 0) << endl;
// 	// // cout << dfaMatrix.MatchFromBeginning(U"cab", 0) << endl;

// 	// cout << "find" << endl;
// 	// cout << dfaMatrix.Find(U"aaba") << endl;
// 	// // auto exp2 = regex::Literal(U"0123456789");
// 	// // regex::NFA nfa2{exp2};
// 	// // regex::NFAViewer::ViewNFA(nfa2);

// 	Test();

// 	return 0;
// }
