#include "../header.h"

class Solution {
public:
    double champagneTower(int poured, int query_row, int query_glass) {
        vector<vector<double>> flow(query_row + 1, vector<double>(query_row + 1));
        flow[0][0] = poured;
        for (int i = 1; i <= query_row; ++i) {
            for (int j = 0; j <= query_row; ++j) {
                if (j >= 1) {
                    if (flow[i - 1][j - 1] > 1) {
                        flow[i][j] += (flow[i - 1][j - 1] - 1.0) / 2;
                    }
                }
                if (flow[i - 1][j] > 1) {
                    flow[i][j] += (flow[i - 1][j] - 1.0) / 2;
                }
            }
        }

        return min(1.0, flow[query_row][query_glass]);
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.champagneTower(1, 1, 1) << endl;
    cerr << sol.champagneTower(2, 1, 1) << endl;
    cerr << sol.champagneTower(100000009, 33, 17) << endl;

    return 0;
}
