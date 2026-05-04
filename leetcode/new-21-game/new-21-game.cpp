#include "../header.h"

class Solution {
public:
    double new21Game(int n, int k, int maxPts) {
        vector<double> prob(n + 2);
        prob[0] = 1.0;
        for (int i = 0; i <= n + 20; ++i) {
            // cerr << i << " " << prob << endl;
            vector<double> newProb(n + 2);
            for (int j = 0; j < k; ++j) {
                if (!prob[j]) {
                    continue;
                }
                for (int l = 1; l <= maxPts; ++l) {
                    int p = min(n + 1, j + l);
                    newProb[p] += prob[j] / maxPts;
                }
            }
            for (int j = k; j < prob.size(); ++j) {
                newProb[j] += prob[j];
            }
            prob = newProb;
        }
        double result = 0;
        for (int i = 0; i <= n; ++i) {
            result += prob[i];
        }
        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.new21Game(21, 17, 10) << endl;

    return 0;
}
