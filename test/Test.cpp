#include "Test_Regular_Expression_Tree.hpp"
#include "Test_NFA.hpp"
#include "Test_DFA.hpp"
#include "Test_Match.hpp"
#include <iostream>

using std::cout;
using std::endl;

int main()
{
	TestRE1();
	TestRE2();
	TestRE3();

	TestNFA1();
	TestNFA2();
	TestNFA3();

	TestDFA1();
	TestDFA2();
	TestDFA3();

	TestMatch1();
	TestMatch2();
	TestMatch3();

	return 0;
}