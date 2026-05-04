#include "../header.h"

#include <lib/unionFindDense.h>

class Solution {
public:
    long long countPairs(int n, const vector<vector<int>>& edges) {
        UnionFindDense uf(n);
        for (const auto& e: edges) {
            uf.unite(e[0], e[1]);
        }

        const auto comp = uf.components();
        long long result = 0;
        for (const auto& c: comp) {
            result += c.size * (n - c.size);
        }
        return result / 2;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.countPairs(7, {{0, 2}, {0, 5}, {2, 4}, {1, 6}, {5, 4}}) << endl;

    return 0;
}
