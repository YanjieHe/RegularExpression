#ifndef TEST_DFA_HPP
#define TEST_DFA_HPP
#include "DFA.hpp"
#include "Test_NFA.hpp"

Json DFAToJson(const DFA& dfa);

Json DFATableToJson(const vector<DFATableRow>& rows);

void DFAToDotFile(const DFA& dfa, string path);

void TestDFA1();

void TestDFA2();

#endif // TEST_DFA_HPP