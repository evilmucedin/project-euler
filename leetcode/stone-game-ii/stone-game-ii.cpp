#include "../header.h"

class Solution {
public:
    using Key = tuple<int, int>;
    map<Key, int> dp_;

    int stoneGame_(const vector<int>& piles, const vector<int>& suffixSum, int pos, int m) {
        if (pos >= piles.size()) {
            return 0;
        }

        if (pos + 2*m >= piles.size()) {
            return suffixSum[pos];
        }

        int mx = 0;
        for (int x = 1; x <= 2*m; ++x) {
            if (pos + x - 1 >= piles.size()) {
                break;
            }
            int newM = max(m, x);
            /*
            int mn = 1e9;
            for (int y = 0; y <= 2*newM; ++y) {
                mn = min(mn, sum1 + stoneGame(piles, pos + x + y, newM));
            }
            mx = max(mx, mn);
            */
            mx = max(mx, suffixSum[pos] - stoneGame(piles, suffixSum, pos + x, newM));
        }

        return mx;
    }

    int stoneGame(const vector<int>& piles, const vector<int>& suffixSum, int pos, int m) {
        Key key(pos, m);
        auto toKey = dp_.find(key);
        if (toKey == dp_.end()) {
            int res = stoneGame_(piles, suffixSum, pos, m);
            dp_.emplace(key, res);
            return res;
        }
        return toKey->second;
    }

    int stoneGameII(const vector<int>& piles) {
        dp_.clear();
        vector<int> suffixSum(piles.size() + 1);
        suffixSum[piles.size() - 1] = piles.back();
        for (int i = piles.size() - 2; i >= 0; --i) {
            suffixSum[i] = suffixSum[i + 1] + piles[i];
        }
        return stoneGame(piles, suffixSum, 0, 1);
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.stoneGameII({2, 7, 9, 4, 4}) << endl;
    cerr << sol.stoneGameII({100}) << endl;
    cerr << sol.stoneGameII({1, 2, 3, 4, 5, 100}) << endl;

    return 0;
}
