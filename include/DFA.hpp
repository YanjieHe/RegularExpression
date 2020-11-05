#ifndef DFA_HPP
#define DFA_HPP
#include <vector>
#include <string>
#include <unordered_map>

namespace regex
{
using std::vector;
using std::u32string;
using std::unordered_map;

class DFAEdge
{
public:
	int from;
	int to;
	int pattern;

	DFAEdge();
	DFAEdge(int from, int to, int pattern);
	inline bool IsEpsilon() const
	{
		return pattern == EPSILON;
	}
	const int EPSILON = -1;
};

class DFA
{
public:
	vector<vector<DFAEdge>> adj;

	DFA() = default;

	void AddEdge(const DFAEdge& edge);
	int NodeCount() const;
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
	DFA dfa;
    vector<int32_t> endStates;
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
	size_t operator()(const vector<int32_t>& V) const
	{
		size_t hash = V.size();
		for (auto& i : V)
		{
			hash ^= i + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		}
		return hash;
	}
};
DFAGraph DFATableToDFAGraph(
	const vector<DFATableRow>& rows,
	const unordered_map<int32_t, Interval>& patternIDToInterval);
bool IsEndState(const vector<vector<int32_t>>& nextStates);
int32_t RecordState(unordered_map<vector<int32_t>, int32_t, Int32VectorHash>& stateMap, const vector<int32_t>& state);
} // namespace regex
#endif // DFA_HPP