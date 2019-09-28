#include "RegularExpression.hpp"
#include "NFA.hpp"
#include "NFAViewer.hpp"
#include <iostream>
using std::cout;
using std::endl;

regex::String operator "" _w(const char* str, size_t n) {
	regex::String res;
	res.reserve(n);
	for (size_t i = 0; i < n; i++) {
		res.push_back(str[i]);
	}
	return res;
}

int main()
{
	auto exp = (regex::Symbol('a') + regex::Symbol('b')) | (regex::Symbol('b') + regex::Symbol('a'));
	regex::NFA nfa { exp };
	regex::NFAViewer::ViewNFA(nfa);

	auto exp2 = regex::Literal("0123456789"_w);
	regex::NFA nfa2 { exp2 };
	regex::NFAViewer::ViewNFA(nfa2);
//	std::locale::global(std::locale(""));
//	Test1();
//	Test2();
//	Test3();
//	Test4();
//	Test5();
//	Test6();
//  Test7();
}
