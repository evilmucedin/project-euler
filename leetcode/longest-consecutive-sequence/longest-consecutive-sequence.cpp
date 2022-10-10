#include "../header.h"

class Solution {
public:
    int longestConsecutive(const vector<int>& nums) {
        unordered_map<int, int> parent;

        auto find = [&](int first) {
            auto toParent = parent.find(first);
            while (toParent != parent.end() && toParent->second != first) {
                first = toParent->second;
                toParent = parent.find(first);
            }
            return first;
        };

        auto unite = [&](int first, int second) {
            auto toSecond = parent.find(second);
            if (toSecond != parent.end()) {
                parent[find(first)] = toSecond->second;
            }
        };

        for (int i : nums) {
            if (parent.find(i) == parent.end()) {
                parent.emplace(i, i);
                unite(i, i + 1);
                unite(i, i - 1);
            }
        }

        unordered_set<int> processed;
        unordered_map<int, int> parentSizes;
        for (int i : nums) {
            if (processed.count(i)) {
                continue;
            }
            processed.emplace(i);

            int j = i;
            auto toParent = parent.find(j);
            while (toParent != parent.end() && toParent->second != j) {
                j = toParent->second;
                toParent = parent.find(j);
            }
            ++parentSizes[j];

            int k = i;
            toParent = parent.find(k);
            while (toParent != parent.end() && toParent->second != k) {
                parent[k] = j;
                k = toParent->second;
                toParent = parent.find(k);
            }
        }

        int res = 0;
        for (const auto& s: parentSizes) {
            if (s.second > res) {
                res = s.second;
           }
        }

        return res;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.longestConsecutive({0, 3, 7, 2, 5, 8, 4, 6, 0, 1}) << endl;

    return 0;
}
