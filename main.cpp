#include "RegularExpression.hpp"
#include "NFA.hpp"
#include "NFAViewer.hpp"
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

	auto exp2 = regex::Literal(U"0123456789");
	regex::NFA nfa2{exp2};
	regex::NFAViewer::ViewNFA(nfa2);

	return 0;
}
