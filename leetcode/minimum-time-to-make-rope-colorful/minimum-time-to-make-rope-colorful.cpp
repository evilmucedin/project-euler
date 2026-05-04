#include "../header.h"

class Solution {
public:
    int minCost(const string& colors, const vector<int>& neededTime) {
        int result = 0;
        static constexpr int INF = 1000000000;
        int beginSum = INF;
        int minSum = INF;
        char color = 0;
        for (size_t i = 0; i < colors.size(); ++i) {
            if (colors[i] == color) {
                beginSum += neededTime[i];
                minSum = max(minSum, neededTime[i]);
            } else {
                // cerr << color << " " << beginSum << " " << minSum << endl;
                result += beginSum - minSum;
                color = colors[i];
                beginSum = neededTime[i];
                minSum = neededTime[i];
            }
        }
        result += beginSum - minSum;
        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.minCost("abaac", {1, 2, 3, 4, 5}) << endl;
    cerr << sol.minCost("abc", {1, 2, 3}) << endl;
    cerr << sol.minCost("aabaa", {1, 2, 3, 4, 1}) << endl;

    return 0;
}
