#ifndef DFA_HPP
#define DFA_HPP
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace regex
{
using std::vector;
using std::u32string;
using std::unordered_map;
using std::unordered_set;

static const int EPSILON = -1;

class Edge
{
public:
	size_t from;
	size_t to;
	int pattern;

	Edge()
		: from{0}
		, to{0}
		, pattern{EPSILON}
	{
	}
	Edge(size_t from, size_t to, int pattern)
		: from{from}
		, to{to}
		, pattern{pattern}
	{
	}
	bool IsEpsilon() const
	{
		return pattern == EPSILON;
	}
};

class Graph
{
public:
	vector<vector<Edge>> adj;

	Graph()
	{
	}
	size_t AddNode()
	{
		size_t n = adj.size();
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

class Interval
{
public:
	int32_t lower;
	int32_t upper;
	Interval() = default;
	Interval(int32_t lower, int32_t upper)
		: lower{lower}
		, upper{upper}
	{
	}
	u32string ToString() const;
};

class DFAGraph
{
public:
	Graph dfa;
	unordered_set<int32_t> endStates;
	vector<Interval> patternIDToIntervals;
};

class DFATableRow
{
public:
	vector<int32_t> index;
	vector<vector<int32_t>> nextStates;
	DFATableRow() = default;
	DFATableRow(vector<int32_t> index, vector<vector<int32_t>> nextStates)
		: index{index}
		, nextStates{nextStates}
	{
	}
};

inline bool operator==(const Interval& x, const Interval& y)
{
	return x.lower == y.lower && x.upper && y.upper;
}

struct IntervalHash
{
	size_t operator()(const Interval& interval) const noexcept
	{
		size_t h1 = static_cast<size_t>(interval.lower);
		size_t h2 = static_cast<size_t>(interval.upper);
		return h1 ^ (h2 << 1);
	}
};

struct Int32VectorHash
{
	size_t operator()(const vector<int32_t>& ints) const
	{
		size_t hash = ints.size();
		for (auto& i : ints)
		{
			hash = hash ^ (i + 0x9e3779b9 + (hash << 6) + (hash >> 2));
		}
		return hash;
	}
};

class DFAMatrix
{
public:
	vector<vector<int>> matrix;
	vector<Interval> patterns;
	unordered_set<int> endStates;

	bool Match(const u32string& str) const;
	int Find(const u32string& str) const;
	int MatchFromBeginning(const u32string& str, size_t startIndex,
						   bool greedyMode) const;
};

DFAGraph DFATableToDFAGraph(
	const vector<DFATableRow>& rows,
	const unordered_map<int32_t, Interval>& patternIDToInterval,
	int nfaEndState);

bool IsEndState(const vector<int32_t>& index, int nfaEndState);

int32_t RecordState(
	unordered_map<vector<int32_t>, int32_t, Int32VectorHash>& stateMap,
	const vector<int32_t>& state);

DFAMatrix CreateDFAMatrix(const DFAGraph& dfaGraph);

} // namespace regex
#endif // DFA_HPP