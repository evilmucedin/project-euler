#include "../header.h"
#include "glog/logging.h"

class Solution {
   public:
    static int minNumberOfSemesters_(unordered_map<int, vector<int>>& prevs, vector<int>& placed, vector<int>& order,
                                     int placedToday, int placedTotal, int day, int n, int k) {
        if (placedTotal == n) {
            cerr << order << " " << placed << " " << day << endl;
            return day;
        }
        int result = 123456789;
        int remains = n - placedTotal;
        int minRemains = (remains % k) ? remains / k + 1 + day : remains / k + day;
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
            if (result == minRemains) {
                return result;
            }

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
            if (result == minRemains) {
                return result;
            }
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
