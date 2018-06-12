#ifndef TEST_HPP
#define TEST_HPP
#include "NFA.hpp"
#include <iostream>
using std::endl;
using std::wcout;
void ShowGraph(NFAGraph& G, int node, vector<bool>& visited);
void DrawGraph(NFAGraph& G, int node, vector<bool>& visited);
void Test1();
void Test2();
void Test3();
void Test4();
#endif // TEST_HPP