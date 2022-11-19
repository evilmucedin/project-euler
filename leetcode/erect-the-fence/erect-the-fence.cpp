#include "../header.h"

#include <lib/convexHull.h>

class Solution {
public:
    vector<vector<int>> outerTrees(const vector<vector<int>>& trees) {
        vector<Point2<int>> points;
        for (const auto& t: trees) {
            points.emplace_back(Point2<int>(t[0], t[1]));
        }

        const auto ch = convexHull<int>(points);
        vector<vector<int>> result;
        for (const auto& p: ch) {
            result.emplace_back(vector<int>{p.x, p.y});
        }
        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.outerTrees({{1, 1}, {2, 2}, {2, 0}, {2, 4}, {3, 3}, {4, 2}})
    << endl;

    return 0;
}
