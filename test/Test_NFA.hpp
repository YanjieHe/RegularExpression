#ifndef TEST_NFA_HPP
#define TEST_NFA_HPP
#include "NFA.hpp"
#include <nlohmann/json.hpp>

using namespace regex;
using std::string;
using Json = nlohmann::json;
using JsonMap = unordered_map<string, Json>;

Json NFAToJson(const NFA& nfa);

Json GraphToJson(const Graph& graph);

Json EdgeToJson(const Edge& edge);

void NFAToDotFile(const NFA& nfa, string path);

void TestNFA1();

void TestNFA2();

#endif // TEST_NFA_HPP