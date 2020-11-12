#ifndef DFA_HPP
#define DFA_HPP
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <set>

namespace regex
{
using std::u32string;
static const int EPSILON = -1;
using StateID = size_t;
enum class RangeType
{
	Epsilon,
	LineStart,
	LineEnd,
	WordStart,
	WordEnd,
	CharacterRange
};
struct UnicodeRange
{
	int32_t lower;
	int32_t upper;

	UnicodeRange()
		: lower{EPSILON}
		, upper{EPSILON}
	{
	}
	UnicodeRange(int32_t lower, int32_t upper)
		: lower{lower}
		, upper{upper}
	{
	}

	bool IsEpsilon() const
	{
		return lower == EPSILON && upper == EPSILON;
	}
	bool InBetween(char32_t c) const
	{
		return lower <= static_cast<int32_t>(c) &&
			   static_cast<int32_t>(c) <= upper;
	}
	u32string ToString() const;
};
} // namespace regex

template <>
struct std::hash<regex::UnicodeRange>
{
	size_t operator()(const regex::UnicodeRange& pattern) const noexcept
	{
		size_t h1 = static_cast<size_t>(pattern.lower);
		size_t h2 = static_cast<size_t>(pattern.upper);
		return h1 ^ (h2 << 1);
	}
};

namespace regex
{
using std::vector;
using std::u32string;
using std::unordered_map;
using std::unordered_set;
using std::optional;

class UnicodePatterns
{
public:
	unordered_map<UnicodeRange, int> patternToID;
	unordered_map<int, UnicodeRange> IDToPattern;

	void Add(UnicodeRange pattern, int id)
	{
		patternToID[pattern] = id;
		IDToPattern[id] = pattern;
	}

	size_t Size() const
	{
		return patternToID.size();
	}

	optional<int> GetIDByPattern(UnicodeRange pattern) const
	{
		if (patternToID.count(pattern))
		{
			return patternToID.at(pattern);
		}
		else
		{
			return optional<int>();
		}
	}

	optional<UnicodeRange> GetPatternByID(int id) const
	{
		if (IDToPattern.count(id))
		{
			return IDToPattern.at(id);
		}
		else
		{
			return optional<UnicodeRange>();
		}
	}
};

class Edge
{
public:
	StateID from;
	StateID to;
	UnicodeRange pattern;

	Edge()
		: from{0}
		, to{0}
		, pattern{UnicodeRange(EPSILON, EPSILON)}
	{
	}
	Edge(StateID from, StateID to, UnicodeRange pattern)
		: from{from}
		, to{to}
		, pattern{pattern}
	{
	}
	bool IsEpsilon() const
	{
		return pattern.IsEpsilon();
	}
};

class Graph
{
public:
	vector<vector<Edge>> adj;

	Graph()
	{
	}
	StateID AddNode()
	{
		StateID n = adj.size();
		adj.emplace_back();
		return n;
	}
	void AddEdge(const Edge& edge)
	{
		size_t n = NodeCount();
		while (n <= edge.from || n <= edge.to)
		{
			adj.emplace_back();
			n = NodeCount();
		}
		adj.at(edge.from).push_back(edge);
	}
	size_t NodeCount() const
	{
		return adj.size();
	}
	vector<Edge> GetEdges() const
	{
		vector<Edge> edges;
		for (const auto& edgeVec : adj)
		{
			for (const auto& edge : edgeVec)
			{
				edges.push_back(edge);
			}
		}
		return edges;
	}
	const vector<Edge>& Adj(int index) const
	{
		return adj.at(index);
	}
};

struct StateIDSetHash
{
	size_t operator()(const std::set<StateID>& stateIDs) const
	{
		size_t hash = stateIDs.size();
		for (auto& i : stateIDs)
		{
			hash = hash ^ (i + 0x9e3779b9 + (hash << 6) + (hash >> 2));
		}
		return hash;
	}
};

class DFA
{
public:
	Graph G;
	unordered_set<StateID> endStates;
	UnicodePatterns patterns;
};

class DFATableRow
{
public:
	std::set<StateID> index;
	vector<std::set<StateID>> nextStates;
	DFATableRow() = default;
	DFATableRow(std::set<StateID> index, vector<std::set<StateID>> nextStates)
		: index{index}
		, nextStates{nextStates}
	{
	}
};

inline bool operator==(const UnicodeRange& x, const UnicodeRange& y)
{
	return x.lower == y.lower && x.upper && y.upper;
}

class DFAMatrix
{
public:
	vector<vector<int>> matrix;
	UnicodePatterns patterns;
	unordered_set<StateID> endStates;

	bool Match(const u32string& str) const;
	int Find(const u32string& str) const;
	int MatchFromBeginning(const u32string& str, size_t startIndex,
						   bool greedyMode) const;
};

DFA DFATableToDFAGraph(const vector<DFATableRow>& rows,
					   const UnicodePatterns& patterns, const Graph& nfaGraph,
					   int nfaEndState);

bool IsEndState(const std::set<StateID>& index, const Graph& nfaGraph,
				StateID nfaEndState);

StateID RecordState(
	unordered_map<std::set<StateID>, StateID, StateIDSetHash>& stateMap,
	const std::set<StateID>& state);

DFAMatrix CreateDFAMatrix(const DFA& dfaGraph);

bool CanTransit(const Graph& G, StateID s1, StateID s2);

} // namespace regex

#endif // DFA_HPP