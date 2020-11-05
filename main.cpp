#include "RegularExpression.hpp"
#include "NFA.hpp"
#include "NFAViewer.hpp"
#include "Encoding.hpp"
#include <iostream>
using std::cout;
using std::endl;
using regex::Symbol;

// regex::u32string operator "" _w(const char* str, size_t n) {
// 	regex::u32string res;
// 	res.reserve(n);
// 	for (size_t i = 0; i < n; i++) {
// 		res.push_back(str[i]);
// 	}
// 	return res;
// }

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

	// auto exp2 = regex::Literal(U"0123456789");
	// regex::NFA nfa2{exp2};
	// regex::NFAViewer::ViewNFA(nfa2);

	return 0;
}
