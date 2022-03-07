#ifndef RE_JSON_SERIALIZER_HPP
#define RE_JSON_SERIALIZER_HPP
#include "NFA.hpp"
#include "RegularExpression.hpp"
#include <nlohmann/json.hpp>

using namespace regex;
using std::string;
using Json = nlohmann::json;
using JsonMap = unordered_map<string, Json>;

class REJsonSerializer : public RegularExpressionVisitor<Json>
{
public:
    virtual Json VisitAlternation(const AlternationExpression::Ptr &exp);
    virtual Json VisitConcatenation(const ConcatenationExpression::Ptr &exp);
    virtual Json VisitKleeneStar(const KleeneStarExpression::Ptr &exp);
    virtual Json VisitSymbol(const SymbolExpression::Ptr &exp);
};

Json NFAToJson(const NFA &nfa);

Json GraphToJson(const Graph &graph);

Json EdgeToJson(const Edge &edge);

void NFAToDotFile(const NFA &nfa, string path);

Json DFAToJson(const DFA &dfa);

Json DFATableToJson(const vector<DFATableRow> &rows);

void DFAToDotFile(const DFA &dfa, string path);
#endif // RE_JSON_SERIALIZER_HPP