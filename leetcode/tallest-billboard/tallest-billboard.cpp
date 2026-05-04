#include <numeric>

#include "../header.h"

class Solution {
public:
    vector<vector<vector<int>>> dp_;

    int rec(const vector<int>& rods, int index, int sum1, int sum2) {
        if (dp_[index][sum1][sum2] != -1) {
            return dp_[index][sum1][sum2];
        }

        int result = 0;
        if (sum1 == sum2) {
            result = sum1;
        }
        if (index == rods.size()) {
            return result;
        }
        result = max({result, rec(rods, index + 1, sum1 + rods[index], sum2),
                      rec(rods, index + 1, sum1, sum2 + rods[index]), rec(rods, index + 1, sum1, sum2)});
        dp_[index][sum1][sum2] = result;
        return result;
    }

    int tallestBillboard(const vector<int>& rods) {
        int sum = std::accumulate(rods.begin(), rods.end(), 0) + 1;
        dp_.resize(rods.size() + 1, vector<vector<int>>(sum, vector<int>(sum, -1)));

        return rec(rods, 0, 0, 0);
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.tallestBillboard({1, 2, 3, 4, 5, 6}) << endl;

    return 0;
}
