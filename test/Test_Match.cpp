#include "Test_Match.hpp"
#include "NFA.hpp"
#include <iostream>
#include <iomanip>
#include "Encoding.hpp"

using std::cout;
using std::endl;

void TestMatch(string functionName, const DFAMatrix& pattern, u32string input,
			   bool expected)
{
	bool actual = pattern.Match(input);
	cout << functionName << " ";
	if (actual == expected)
	{
		cout << "passed!";
	}
	else
	{
		cout << "failed!";
	}
	cout << " input: " << encoding::utf32_to_utf8(input);
	cout << "   expected: " << (expected ? "matched" : "not matched") << endl;
}

void TestMatch1()
{
	auto e1 = (Symbol(U'a') + Symbol(U'b')) | (Symbol(U'b') + Symbol(U'a'));
	auto matrix1 = e1->Compile();
	TestMatch(__FUNCTION__, matrix1, U"ab", true);
	TestMatch(__FUNCTION__, matrix1, U"aa", false);
	TestMatch(__FUNCTION__, matrix1, U"aba", false);
	TestMatch(__FUNCTION__, matrix1, U"bab", false);
	TestMatch(__FUNCTION__, matrix1, U"ba", true);
}

void TestMatch2()
{
	auto e2 = Symbol(U'a')->Many() + Symbol(U'b')->Many();
	auto matrix2 = e2->Compile();
	TestMatch(__FUNCTION__, matrix2, U"aaaaa", true);
	TestMatch(__FUNCTION__, matrix2, U"aaaaabb", true);
	TestMatch(__FUNCTION__, matrix2, U"aaabbaa", false);
}