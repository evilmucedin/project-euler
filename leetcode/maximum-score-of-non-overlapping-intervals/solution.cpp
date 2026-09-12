#include <vector>
#include <algorithm>

using namespace std;

class Solution {
    struct Interval {
        int start, end, weight, id;
        // Sort primarily by start time; use ID as a tie-breaker
        bool operator<(const Interval& other) const {
            if (start != other.start) return start < other.start;
            return id < other.id;
        }
    };

    struct State {
        long long weight = 0;
        vector<int> indices;

        // Custom comparison to handle both maximum weight and lexicographical ties
        bool isBetterThan(const State& other) const {
            if (weight != other.weight) {
                return weight > other.weight;
            }
            // If weights are equal, the lexicographically smaller index list wins
            return indices < other.indices;
        }
    };

public:
    vector<int> maximumWeight(vector<vector<int>>& input) {
        int n = input.size();
        vector<Interval> intervals(n);
        for (int i = 0; i < n; ++i) {
            intervals[i] = {input[i][0], input[i][1], input[i][2], i};
        }
        sort(intervals.begin(), intervals.end());

        // dp[i][j] stores the best state choosing up to j intervals from suffix intervals[i...n-1]
        vector<vector<State>> dp(n + 1, vector<State>(5));

        // Binary search tracking array for quick jump queries
        vector<int> starts(n);
        for (int i = 0; i < n; ++i) {
            starts[i] = intervals[i].start;
        }

        // Fill the DP table backwards (Suffix DP)
        for (int i = n - 1; i >= 0; --i) {
            // Find the first valid non-overlapping interval that starts after intervals[i] ends
            int next_idx = upper_bound(starts.begin() + i, starts.end(), intervals[i].end) - starts.begin();

            for (int j = 1; j <= 4; ++j) {
                // Option A: Skip the current interval
                State best_state = dp[i + 1][j];

                // Option B: Take the current interval
                State take_state;
                take_state.weight = intervals[i].weight + dp[next_idx][j - 1].weight;
                take_state.indices = dp[next_idx][j - 1].indices;
                take_state.indices.push_back(intervals[i].id);
                sort(take_state.indices.begin(), take_state.indices.end()); // Keep sorted for lexicographical comparison

                // Pick the best state between skipping or taking
                if (take_state.isBetterThan(best_state)) {
                    best_state = take_state;
                }

                dp[i][j] = best_state;
            }
        }

        return dp[0][4].indices;
    }
};
