#include "Test_Regular_Expression_Tree.hpp"
#include "Test_NFA.hpp"
#include "Test_DFA.hpp"
#include "Test_Match.hpp"
#include <iostream>

using std::cout;
using std::endl;

int main()
{
	cout << "Hello World" << endl;
	TestRE1();
	TestRE2();

	TestNFA1();
	TestNFA2();

	TestDFA1();
	TestDFA2();

	TestMatch1();
	TestMatch2();
	return 0;
}