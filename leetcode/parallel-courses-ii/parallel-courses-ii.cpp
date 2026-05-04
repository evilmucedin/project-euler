#include "../header.h"
#include "glog/logging.h"

class Solution_ {
   public:
    static int minNumberOfSemesters_(unordered_map<int, vector<int>>& prevs, vector<int>& placed, vector<int>& order,
                                     int placedToday, int placedTotal, int day, int n, int k) {
        if (placedTotal == n) {
            // cerr << order << " " << placed << " " << day << endl;
            /*
            cerr << "{";
            for (int i = 0; i < order.size(); ++i) {
                cerr << order[i] << "|" << placed[order[i]] << ", ";
            }
            cerr << "} " << day << endl;
            */
            return day;
        }
        int result = 123456789;
        // int remains = n - placedTotal;
        // int minRemains = (remains % k) ? remains / k + 1 + day : remains / k + day;
        for (int i = 1; i <= n; ++i) {
            if (placed[i]) {
                continue;
            }
            bool good = true;
            for (auto u : prevs[i]) {
                if (!placed[u] || placed[u] == day) {
                    good = false;
                    break;
                }
            }
            if (good) {
                bool nextDay = placedToday + 1 > k;
                placed[i] = (nextDay) ? day + 1 : day;
                order.emplace_back(i);
                result = min(result, minNumberOfSemesters_(prevs, placed, order, (nextDay) ? 1 : placedToday + 1,
                                                           placedTotal + 1, (nextDay) ? day + 1 : day, n, k));
                placed[i] = 0;
                order.pop_back();
            }

            // LOG_EVERY_MS(INFO, 1000) << result << " " << n << " " << k;
            /*
            if (result == minRemains) {
                return result;
            }
            */

            good = true;
            for (auto u : prevs[i]) {
                if (!placed[u]) {
                    good = false;
                    break;
                }
            }
            if (good) {
                placed[i] = day + 1;
                order.emplace_back(i);
                bool nextDay = true;
                result = min(result, minNumberOfSemesters_(prevs, placed, order, (nextDay) ? 1 : placedToday + 1,
                                                           placedTotal + 1, (nextDay) ? day + 1 : day, n, k));
                placed[i] = 0;
                order.pop_back();
            }

            // LOG_EVERY_MS(INFO, 1000) << result << " " << n << " " << k;
            /*
            if (result == minRemains) {
                return result;
            }
            */
        }
        return result;
    }

    int minNumberOfSemesters(int n, const vector<vector<int>>& relations, int k) {
        unordered_map<int, vector<int>> prevs;
        for (const auto& v : relations) {
            prevs[v[1]].emplace_back(v[0]);
        }
        vector<int> placed(n + 1, false);
        vector<int> order;
        return minNumberOfSemesters_(prevs, placed, order, 0, 0, 1, n, k);
    }
};

class Solution {
public:
    static int dp(int n, int k, int mask, const vector<int>& deps, vector<int>& dpMem) {
        if (mask + 1 == 1 << n) {
            return 0;
        }

        if (dpMem[mask] != -1) {
            return dpMem[mask];
        }

        int possible = 0;
        for (int i = 0; i < n; ++i) {
            if ((mask & deps[i]) == deps[i]) {
                if ((mask & (1 << i)) == 0) {
                    possible |= 1 << i;
                }
            }
        }

        int best = 123456789;
        for (int daymask = possible; daymask > 0; daymask = ((daymask - 1) & possible)) {
            if (__popcount(daymask) <= k) {
                best = min(best, 1 + dp(n, k, mask | daymask, deps, dpMem));
            }
        }
        dpMem[mask] = best;
        return best;
    }

    int minNumberOfSemesters(int n, const vector<vector<int>>& relations, int k) {
        vector<int> deps(n);
        for (const auto& v: relations) {
            deps[v[1] - 1] |= 1 << (v[0] - 1);
        }
        vector<int> dpMem(1 << n, -1);
        return dp(n, k, 0, deps, dpMem);
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    // 6
    cerr << sol.minNumberOfSemesters(12, {{1, 2}, {1, 3}, {7, 5}, {7, 6}, {4, 8}, {8, 9}, {9, 10}, {10, 11}, {11, 12}},
                                     2)
         << endl;
    cerr << sol.minNumberOfSemesters(4, {{2, 1}, {3, 1}, {1, 4}}, 2) << endl;
    cerr << sol.minNumberOfSemesters(5, {{2, 1}, {3, 1}, {4, 1}, {1, 5}}, 2) << endl;
    cerr << sol.minNumberOfSemesters(11, {}, 2) << endl;
    cerr << sol.minNumberOfSemesters(15, {}, 2) << endl;
    cerr << sol.minNumberOfSemesters(13, {{12, 8}, {2, 4}, {3, 7},  {6, 8},  {11, 8}, {9, 4},  {9, 7}, {12, 4}, {11, 4},
                                          {6, 4},  {1, 4}, {10, 7}, {10, 4}, {1, 7},  {1, 8},  {2, 7}, {8, 4},  {10, 8},
                                          {12, 7}, {5, 4}, {3, 4},  {11, 7}, {7, 4},  {13, 4}, {9, 8}, {13, 8}},
                                     9)
         << endl;

    return 0;
}
