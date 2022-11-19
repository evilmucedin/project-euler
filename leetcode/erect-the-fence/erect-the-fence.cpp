#include "../header.h"

#include <lib/convexHull.h>

class Solution {
public:
    vector<vector<int>> outerTrees(const vector<vector<int>>& trees) {
        if (trees.size() <= 3) {
            return trees;
        }

        vector<Point2<int>> points;
        for (const auto& t: trees) {
            points.emplace_back(Point2<int>(t[0], t[1]));
        }

        const auto ch = convexHull<int>(points);
        vector<Line2<int>> chLines;
        for (int i = 0; i < ch.size(); ++i) {
            chLines.emplace_back(Line2<int>::fromPoints(ch[(i - 1 + ch.size()) % ch.size()], ch[i]));
        }
        vector<vector<int>> result;
        for (const auto& p: trees) {
            int j = 0;
            while (j < chLines.size() && !chLines[j].on(Point2<int>(p[0], p[1]))) {
                ++j;
            }
            if (j < chLines.size()) {
                result.emplace_back(p);
            }
        }
        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.outerTrees({{1, 1}, {2, 2}, {2, 0}, {2, 4}, {3, 3}, {4, 2}}) << endl;
    cerr << sol.outerTrees({{5, 3}, {7, 5}}) << endl;
    cerr << sol.outerTrees({{1,2},{2,2},{4,2},{5,2},{6,2},{7,2}}) << endl;
    
    return 0;
}
