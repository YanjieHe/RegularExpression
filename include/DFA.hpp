#ifndef DFA_HPP
#define DFA_HPP
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <set>

namespace regex
{
using std::vector;
using std::u32string;
using std::unordered_map;
using std::unordered_set;
using std::u32string;
using StateID = size_t;

enum class RangeType
{
	Epsilon,
	LineBegin,
	LineEnd,
	CharacterRange
};

struct UnicodeRange
{
	RangeType rangeType;
	char32_t lower;
	char32_t upper;

	UnicodeRange()
		: rangeType{RangeType::Epsilon}
		, lower{0}
		, upper{0}
	{
	}
	UnicodeRange(RangeType rangeType, char32_t lower, char32_t upper)
		: rangeType{rangeType}
		, lower{lower}
		, upper{upper}
	{
	}

	bool IsEpsilon() const
	{
		return rangeType == RangeType::Epsilon;
	}
	bool InBetween(char32_t c) const
	{
		if (rangeType != RangeType::Epsilon)
		{
			return lower <= c && c <= upper;
		}
		else
		{
			return false;
		}
	}
	u32string ToString() const;

	static UnicodeRange EPSILON;
};
struct UnicodeRangeHash
{
	size_t operator()(const regex::UnicodeRange& pattern) const noexcept
	{
		size_t h1 = static_cast<size_t>(pattern.rangeType);
		size_t h2 = static_cast<size_t>(pattern.lower);
		size_t h3 = static_cast<size_t>(pattern.upper);
		size_t hash = 1;
		hash = hash * 31 + h1;
		hash = hash * 31 + h2;
		hash = hash * 31 + h3;
		return hash;
	}
};
inline bool operator==(const UnicodeRange& x, const UnicodeRange& y)
{
	return x.rangeType == y.rangeType && x.lower == y.lower &&
		   x.upper == y.upper;
}
class UnicodePatterns
{
public:
	unordered_map<UnicodeRange, int, UnicodeRangeHash> patternToID;
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

	int GetIDByPattern(const UnicodeRange& pattern) const
	{
		return patternToID.at(pattern);
	}

	bool HasPattern(const UnicodeRange& pattern) const
	{
		return patternToID.count(pattern);
	}

	const UnicodeRange& GetPatternByID(int id) const
	{
		return IDToPattern.at(id);
	}
};

class Edge
{
public:
	StateID from;
	StateID to;
	UnicodeRange pattern;

	Edge() = default;
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

	Graph() = default;
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
		size_t hash = 1;
		for (auto& i : stateIDs)
		{
			hash = hash * 31 + i;
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

class DFAMatrix
{
public:
	vector<vector<int>> matrix;
	UnicodePatterns patterns;
	unordered_set<StateID> endStates;
	DFAMatrix() = default;
	explicit DFAMatrix(const DFA& dfaGraph);

	bool FullMatch(const u32string& str) const;
	int Search(const u32string& str) const;
	int Match(const u32string& str, size_t startPos, size_t endPos,
			  bool greedyMode) const;

private:
	bool MatchPattern(int& state, const UnicodeRange& pattern, char32_t c,
					  size_t& i, size_t j, size_t startPos,
					  size_t endPos) const;
	bool IsEndState(int state) const;
};

DFA DFATableRowsToDFAGraph(const vector<DFATableRow>& rows,
						   const UnicodePatterns& patterns,
						   const Graph& nfaGraph, int nfaEndState);

bool IsEndState(const std::set<StateID>& index, const Graph& nfaGraph,
				StateID nfaEndState);

StateID RecordState(
	unordered_map<std::set<StateID>, StateID, StateIDSetHash>& stateMap,
	const std::set<StateID>& state);

bool CanTransit(const Graph& G, StateID s1, StateID s2);

} // namespace regex

#endif // DFA_HPP