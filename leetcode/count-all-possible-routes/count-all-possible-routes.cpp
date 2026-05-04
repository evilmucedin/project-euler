#include "../header.h"

class Solution {
public:
    vector<vector<int>> dp_;
    static constexpr long long MOD = 1e9 + 7;

    int count(const vector<int>& locations, int start, int finish, int fuel) {
        if (fuel < 0) {
            return 0;
        }

        if (dp_[start][fuel] != -1) {
            return dp_[start][fuel];
        }

        int result = 0;
        if (start == finish) {
            ++result;
        }

        for (int i = 0; i < locations.size(); ++i) {
            int cost = abs(locations[i] - locations[start]);
            if (i != start && cost <= fuel) {
                result = (result + count(locations, i, finish, fuel - cost)) % MOD;
            }
        }

        dp_[start][fuel] = result;

        return result;
    }

    int countRoutes(const vector<int>& locations, int start, int finish, int fuel) {
        const int n = locations.size();
        dp_.clear();
        dp_.resize(n, vector<int>(fuel + 1, -1));
        return count(locations, start, finish, fuel);
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.countRoutes({4, 3, 1}, 1, 0, 6) << endl;

    return 0;
}
