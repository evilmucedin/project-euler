#include "../header.h"

class Solution {
public:
    void split(int now, int index, int sum, int& mx, vector<vector<int>>& suffixes) {
        if (now < 0) {
            return;
        }

        if (index == suffixes.size() || now == 0) {
            mx = max(mx, sum);
            return;
        }

        for (int i = 0; i < suffixes[index].size(); ++i) {
            if (i > now) {
                return;
            }
            split(now - i, index + 1, sum + suffixes[index][i], mx, suffixes);
        }
    }

    int maxValueOfCoins_(vector<vector<int>>& piles, int k) {
        const int n = piles.size();
        vector<vector<int>> suffixes(n);
        for (int j = 0; j < n; ++j) {
            auto& s = suffixes[j];
            const auto& p = piles[j];
            s.resize(p.size() + 1);
            for (int i = 1; i <= p.size(); ++i) {
                s[i] = s[i - 1] + p[i - 1];
            }
        }

        int mx = 0;
        split(k, 0, 0, mx, suffixes);
        return mx;
    }

    int maxValueOfCoins(const vector<vector<int>>& piles, int k) {
        vector<int> maxSum(k + 1);
        const int n = piles.size();
        for (int i = 0; i < n; ++i) {
            vector<int> newMaxSum(k + 1);
            const auto& p = piles[i];
            int sum = 0;
            for (int j = 0; j <= p.size(); ++j) {
                for (int l = 0; l <= k; ++l) {
                    if (l + j <= k) {
                        newMaxSum[l + j] = max(newMaxSum[l + j], maxSum[l] + sum);
                    }
                }
                if (j != p.size()) {
                    sum += p[j];
                }
            }
            maxSum.swap(newMaxSum);
        }
        return maxSum[k];
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.maxValueOfCoins({{1, 100, 3}, {7, 8, 9}}, 2) << endl;
    cerr << sol.maxValueOfCoins({{100}, {100}, {100}, {100}, {100}, {100}, {1, 1, 1, 1, 1, 1, 700}}, 7) << endl;

    return 0;
}
