#include "Test_NFA.hpp"
#include "NFA.hpp"
#include "Encoding.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;

Json NFAToJson(const NFA& nfa)
{
	return JsonMap({{"start_vertex", nfa.startVertex},
					{"end_vertex", nfa.endVertex},
					{"graph", GraphToJson(nfa.G)}});
}

Json GraphToJson(const Graph& graph)
{
	vector<Json> edges;
	for (auto edge : graph.GetEdges())
	{
		edges.push_back(EdgeToJson(edge));
	}
	return JsonMap({{"type", "Graph"}, {"edges", edges}});
}

Json EdgeToJson(const Edge& edge)
{
	return JsonMap(
		{{"from", edge.from},
		 {"to", edge.to},
		 {"pattern", encoding::utf32_to_utf8(edge.pattern.ToString())}});
}

void NFAToDotFile(const NFA& nfa, string path)
{
	const auto& G = nfa.G;
	auto edges = G.GetEdges();
	ofstream out(path);
	out << "digraph {" << endl;
	out << "rankdir=LR;" << endl;
	out << "size=\"8,5;\"" << endl;
	out << "node [shape = doublecircle];" << nfa.endVertex << ";" << endl;
	out << "node [shape = point ]; start;" << endl;
	out << "node [shape = circle];" << endl;
	out << "start -> " << nfa.startVertex << ";" << endl;
	for (const auto& edge : edges)
	{
		out << edge.from << " -> " << edge.to;
		out << "[label=\"" << encoding::utf32_to_utf8(edge.pattern.ToString())
			<< "\"];" << endl;
	}
	out << "}" << endl;
}

void TestNFA1()
{
	auto e1 = (Symbol(U'a') + Symbol(U'b')) | (Symbol(U'b') + Symbol(U'a'));
	NFA nfa(e1);
	auto actual = NFAToJson(nfa);
	Json expected;
	ifstream stream("../test/TestNFA1.json");
	stream >> expected;
	cout << __FUNCTION__ << " "
		 << ((actual == expected) ? "passed!"
								  : "failed! <<<<<<<<<<<<<<<<<<<<<<<")
		 << endl;
	NFAToDotFile(nfa, "NFA1.dot");
}

void TestNFA2()
{
	auto e2 = Many(Symbol(U'a')) + Many(Symbol(U'b'));
	NFA nfa(e2);
	auto actual = NFAToJson(nfa);
	Json expected;
	ifstream stream("../test/TestNFA2.json");
	stream >> expected;
	cout << __FUNCTION__ << " "
		 << ((actual == expected) ? "passed!"
								  : "failed! <<<<<<<<<<<<<<<<<<<<<<<")
		 << endl;
	NFAToDotFile(nfa, "NFA2.dot");
}