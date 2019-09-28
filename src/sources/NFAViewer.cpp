#include "NFAViewer.hpp"
#include <iostream>

namespace regex {
using std::cout;
using std::endl;
void NFAViewer::ViewNFA(const NFA& nfa) {
	const auto& G = nfa.Graph();
	auto edges = G.GetEdges();
	cout << "digraph {" << endl;
	cout << "rankdir=LR;" << endl;
    cout << "size=\"8,5;\"" << endl;
	cout << "node [shape = doublecircle];";
	for(int i = 0; i < G.NodeCount(); i++) {
		if(G.Adj(i).size() == 0) {
			cout << i << ";";
		}
	}
	cout << endl;
	cout << "node [shape = point ]; start;" << endl;
	cout << "node [shape = circle];" << endl;
	cout << "start -> " << G.Start() << ";" << endl;
	for(const auto& edge : edges) {
		cout << edge.From() << " -> " << edge.To();
		if(edge.IsEpsilon()) {
			cout << "[label=\"@\"];" << endl;
		} else {
			cout << "[label=\"" << static_cast<char>(edge.Pattern()) << "\"];" << endl;
		}
	}
	cout << "}" << endl;
}
} // namespace regex
