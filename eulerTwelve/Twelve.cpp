#include "lib/header.h"

int main() {
    vector<IntVector> kPerm = {
        {2, 3, 4, 5},
        {1, 3, 6, 8},
        {1, 4, 7, 11},
        {8, 9, 10, 11},
        {2, 6, 9, 12},
        {5, 7, 10, 12},
        // {1, 2, 5, 8, 11, 12},
    };

    IntVector indices(12);
    for (size_t i = 0; i < indices.size(); ++i) {
        indices[i] = i + 1;
    }

    unordered_set<IntVector> sets;

    int count = 0;
    do {
        auto sum = [&](const IntVector& indices, const IntVector& perm) -> int {
            int res = 0;
            for (auto i: perm) {
                res += indices[i - 1];
            }
            return res;
        };

        auto sub = [&](const IntVector& indices, const IntVector& perm) -> IntVector {
            IntVector res;
            for (auto i: perm) {
                res.emplace_back(indices[i - 1]);
            }
            return res;
        };

        size_t j = 0;
        while (j < kPerm.size() && sum(indices, kPerm[j]) == 26) {
            ++j;
        }
        if (j == kPerm.size()) {
            auto sub1 = sub(indices, {1, 8, 11});
            sets.insert(sub1);
            cout << OUT(indices) << OUT(sub1) << OUT(sub(indices, {2, 5, 12})) << endl;
            ++count;
        }
    } while (next_permutation(indices.begin(), indices.end()));
    cout << OUT(count) << endl;
    cout << OUT(sets.size()) << endl;

    return 0;
}
