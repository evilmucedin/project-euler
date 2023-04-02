#include "../header.h"

class Solution {
   public:
    vector<int> minReverseOperations(int n, int p, const vector<int>& banned, int k) {
        // unordered_set<int> sBanned(banned.begin(), banned.end());
        vector<bool> sBanned(n, true);
        for (int i : banned) {
            sBanned[i] = false;
        }

        vector<int> res(n, -1);
        res[p] = 0;

        if (k == 2) {
            for (int d = 1; d < n; ++d) {
                if (p + d < n && sBanned[p + d]) {
                    res[p + d] = d;
                } else {
                    break;
                }
            }
            for (int d = 1; d < n; ++d) {
                if (p - d < 0) {
                    break;
                }
                if (p - d >= 0 && sBanned[p - d]) {
                    res[p - d] = d;
                } else {
                    break;
                }
            }
            return res;
        }

        priority_queue<pair<int, int>> q;
        q.emplace(make_pair(0, p));

        while (!q.empty()) {
            const auto top = q.top();
            q.pop();
            int curpos = top.second;
            int len = -top.first;

            for (int i = max(0, curpos - k + 1); i <= min(n - k, curpos); ++i) {
                int newpos = i + k - 1 - (curpos - i);
                if (sBanned[newpos]) {
                    if (res[newpos] == -1 || len + 1 < res[newpos]) {
                        res[newpos] = len + 1;
                        q.emplace(make_pair(-len - 1, newpos));
                    }
                }
            }
        }

        return res;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.minReverseOperations(100, 50, {}, 2) << endl;
    cerr << sol.minReverseOperations(100, 50, {}, 3) << endl;
    cerr << sol.minReverseOperations(100, 50, {}, 4) << endl;
    cerr << sol.minReverseOperations(100, 50, {}, 90) << endl;

    return 0;
}
