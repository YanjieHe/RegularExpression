#include "Test_Regular_Expression_Tree.hpp"
#include "Test_NFA.hpp"
#include "Test_DFA.hpp"
#include "Test_Match.hpp"
#include <iostream>

int main()
{
	TestRE1();
	TestRE2();
	TestRE3();

	TestNFA1();
	TestNFA2();
	TestNFA3();
	TestNFA4();

	TestDFA1();
	TestDFA2();
	TestDFA3();
	TestDFA4();

	TestMatch1();
	TestMatch2();
	TestMatch3();
	TestMatch4();
	return 0;
}