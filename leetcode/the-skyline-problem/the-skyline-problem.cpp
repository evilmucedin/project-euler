#include "../header.h"

#include <set>

class Solution {
public:
    vector<vector<int>> getSkyline(const vector<vector<int>>& buildings) {
        struct Point {
            int x;
            int h;
            bool left;

            bool operator<(const Point& p) const {
                if (x != p.x) {
                    return x < p.x;
                } else {
                    return left > p.left;
                }
            };
        };
        using Points = vector<Point>;
        Points ps;
        for (const auto& b: buildings) {
            ps.emplace_back(Point{b[0], b[2], true});
            ps.emplace_back(Point{b[1], b[2], false});
        }
        sort(ps.begin(), ps.end());

        multiset<int> hs;
        vector<vector<int>> results;
        for (const auto& p : ps) {
            if (p.left) {
                hs.emplace(p.h);
            } else {
                hs.erase(hs.find(p.h));
            }
            if (!hs.empty()) {
                const auto maxH = *hs.rbegin();
                results.emplace_back(vector<int>{p.x, maxH});
            } else {
                results.emplace_back(vector<int>{p.x, 0});
            }
        }

        vector<vector<int>> results2;
        results2.emplace_back(results[0]);
        for (int i = 1; i < results.size(); ++i) {
            if (results[i][1] != results[i - 1][1]) {
                results2.emplace_back(results[i]);
            }
        }

        vector<vector<int>> results3;
        for (int i = 0; i + 1 < results2.size(); ++i) {
            if (results2[i][0] != results2[i + 1][0]) {
                results3.emplace_back(results2[i]);
            }
        }
        results3.emplace_back(results2.back());
        return results3;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.getSkyline({{2, 9, 10}, {3, 7, 15}, {5, 12, 12}, {15, 20, 10}, {19, 24, 8}}) << endl;
    cerr << sol.getSkyline({{0, 2, 3}, {2, 5, 3}}) << endl;
    cerr << sol.getSkyline({{1, 2, 1}, {1, 2, 2}, {1, 2, 3}}) << endl;

    return 0;
}
