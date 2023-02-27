#include "../header.h"

class Solution {
public:
    int maxEvents(const vector<vector<int>>& events) {
        auto q = events;
        sort(q.begin(), q.end(), [](const vector<int>& a, const vector<int>& b) {
            if (a[1] != b[1]) {
                return a[1] < b[1];
            }
            if (a[0] != b[0]) {
                return a[0] < b[0];
            }
            return false;
        });

        cerr << q << endl;

        int result = 0;
        int d = 0;
        for (int i = 0; i < q.size(); ++i) {
            auto e = q[i];
            if (d < e[0]) {
                d = e[0];
            }
            if (d >= e[0] && d <= e[1]) {
                ++result;
                ++d;
            }
        }
        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.maxEvents({{1, 2}, {1, 2}, {3, 3}, {1, 5}, {1, 5}}) << endl;

    return 0;
}
