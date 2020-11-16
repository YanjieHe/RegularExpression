#ifndef TEST_MATCH_HPP
#define TEST_MATCH_HPP
#include "RegularExpression.hpp"
using namespace regex;
using std::string;

void TestMatch(string functionName, const DFAMatrix& pattern, u32string input, bool expected);
void TestMatch1();
void TestMatch2();
void TestMatch3();
void TestMatch4();

#endif // TEST_MATCH_HPP