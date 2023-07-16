#include "../header.h"

class Solution {
public:
    int maxValue(const vector<vector<int>>& events0, int k) {
        auto events = events0;

        const int n = events.size();
        sort(events.begin(), events.end());

        vector<int> times;
        for (const auto& e: events) {
            times.emplace_back(e[0]);
            times.emplace_back(e[1]);
        }
        sort(times.begin(), times.end());
        times.erase(unique(times.begin(), times.end()), times.end());
        const int m = times.size();

        unordered_map<int, int> time2idx;
        for (int i = 0; i < times.size(); ++i) {
            time2idx[times[i]] = i;
        }

        vector<vector<int>> idx(m + 1);
        for (int i = 0; i < n; ++i) {
            auto& e = events[i];
            e[0] = time2idx[e[0]];
            e[1] = time2idx[e[1]];
            idx[e[1]].emplace_back(i);
        }

        vector<vector<int>> dp(k + 1, vector<int>(m + 1));

        for (int i = 1; i <= k; ++i) {
            for (int j = 1; j <= m; ++j) {
                dp[i][j] = max(dp[i - 1][j], dp[i][j - 1]);
                for (auto ie : idx[j]) {
                    const auto& e = events[ie];
                    dp[i][j] = max(dp[i][j], dp[i - 1][e[0] - 1] + e[2]);
                }
            }
        }

        // cerr << events << endl;
        // cerr << dp << endl;

        return dp.back().back();
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.maxValue({{1, 2, 4}, {3, 4, 3}, {2, 3, 1}}, 2) << endl;
    cerr << sol.maxValue({{1, 2, 4}, {3, 4, 3}, {2, 3, 10}}, 2) << endl;
    cerr << sol.maxValue({{1, 1, 1}, {2, 2, 2}, {3, 3, 3}, {4, 4, 4}}, 3) << endl;

    return 0;
}
