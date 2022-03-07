#include "REJsonSerializer.hpp"

#include <fstream>
#include <iostream>
#include <utfcpp/utf8/cpp11.h>

using std::endl;
using std::ofstream;
using namespace regex::notations;

Json REJsonSerializer::VisitAlternation(const AlternationExpression::Ptr &exp)
{
    return {{"kind", "Alternation"}, {"left", VisitRegularExpression(exp->left)}, {"right", VisitRegularExpression(exp->right)}};
}

Json REJsonSerializer::VisitConcatenation(const ConcatenationExpression::Ptr &exp)
{
    return {{"kind", "Concatenation"}, {"left", VisitRegularExpression(exp->left)}, {"right", VisitRegularExpression(exp->right)}};
}

Json REJsonSerializer::VisitKleeneStar(const KleeneStarExpression::Ptr &exp)
{
    return {{"kind", "KleeneStar"}, {"inner_exp", VisitRegularExpression(exp->innerExp)}};
}

Json REJsonSerializer::VisitSymbol(const SymbolExpression::Ptr &exp)
{
    // exp->range.rangeType -> string
    return {{"kind", "Symbol"}, {"lower", utf8::utf32to8(u32string({exp->range.lower}))}, {"upper", utf8::utf32to8(u32string({exp->range.upper}))}};
}

Json NFAToJson(const NFA &nfa)
{
    return JsonMap({{"start_vertex", nfa.startVertex}, {"end_vertex", nfa.endVertex}, {"graph", GraphToJson(nfa.G)}});
}

Json GraphToJson(const Graph &graph)
{
    vector<Json> edges;
    for (auto edge : graph.GetEdges())
    {
        edges.push_back(EdgeToJson(edge));
    }
    return JsonMap({{"type", "Graph"}, {"edges", edges}});
}

Json EdgeToJson(const Edge &edge)
{
    return JsonMap({{"from", edge.from}, {"to", edge.to}, {"pattern", utf8::utf32to8(edge.pattern.ToString())}});
}

void NFAToDotFile(const NFA &nfa, string path)
{
    const auto &G = nfa.G;
    auto edges = G.GetEdges();
    ofstream out(path);
    out << "digraph {" << endl;
    out << "rankdir=LR;" << endl;
    out << "size=\"8,5;\"" << endl;
    out << "node [shape = doublecircle];" << nfa.endVertex << ";" << endl;
    out << "node [shape = point ]; start;" << endl;
    out << "node [shape = circle];" << endl;
    out << "start -> " << nfa.startVertex << ";" << endl;
    for (const auto &edge : edges)
    {
        out << edge.from << " -> " << edge.to;
        out << "[label=\"" << utf8::utf32to8(edge.pattern.ToString()) << "\"];" << endl;
    }
    out << "}" << endl;
}

Json DFAToJson(const DFA &dfa)
{
    vector<Json> endStatesJson;
    vector<Json> patternsJson;
    for (auto endState : dfa.endStates)
    {
        endStatesJson.push_back(endState);
    }
    return JsonMap({{"end_states", endStatesJson}, {"graph", GraphToJson(dfa.G)}});
}

Json DFATableToJson(const vector<DFATableRow> &rows)
{
    vector<Json> rowsJson;
    for (auto row : rows)
    {
        vector<Json> indexJson;
        vector<vector<Json>> nextStatesJson;
        for (auto i : row.index)
        {
            indexJson.push_back(i);
        }
        for (auto nextState : row.nextStates)
        {
            nextStatesJson.emplace_back();
            for (auto i : nextState)
            {
                nextStatesJson.back().push_back(i);
            }
        }
        rowsJson.push_back(JsonMap({{"index", indexJson}, {"next_states", nextStatesJson}}));
    }
    return rowsJson;
}

void DFAToDotFile(const DFA &dfa, string path)
{
    const auto &G = dfa.G;
    auto edges = G.GetEdges();
    ofstream out(path);
    out << "digraph {" << endl;
    out << "rankdir=LR;" << endl;
    out << "size=\"8,5;\"" << endl;
    out << "node [shape = doublecircle];";

    for (auto endState : dfa.endStates)
    {
        out << endState << ";";
    }
    out << endl;
    out << "node [shape = point ]; start;" << endl;
    out << "node [shape = circle];" << endl;
    out << "start -> 0;" << endl;
    for (const auto &edge : edges)
    {
        out << edge.from << " -> " << edge.to;
        out << "[label=\"" << utf8::utf32to8(edge.pattern.ToString()) << "\"];" << endl;
    }
    out << "}" << endl;
}