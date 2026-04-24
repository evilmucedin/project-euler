To solve the problem of minimizing the Hamming distance between two arrays after performing allowed swaps, we use a Union-Find (Disjoint Set Union) data structure to group indices into connected components based on the allowed swap operations. Within each component, elements from the source array can be reordered freely. We then count how many elements in each component can match between the source and target arrays to determine the maximum number of matches, and subtract this from the total number of elements to get the minimum Hamming distance.

Here is the efficient C++ implementation using a Union-Find structure and frequency maps:

```cpp
#include <vector>
#include <numeric>
#include <unordered_map>
#include <algorithm>

using namespace std;

class Solution {
public:
    int minimumHammingDistance(vector<int>& source, vector<int>& target, vector<vector<int>>& allowedSwaps) {
        int n = source.size();
        vector<int> parent(n);
        iota(parent.begin(), parent.end(), 0);

        // Find with path compression
        function<int(int)> find = [&](int x) -> int {
            if (parent[x] != x)
                parent[x] = find(parent[x]);
            return parent[x];
        };

        // Union operation
        auto unite = [&](int x, int y) {
            int rootX = find(x);
            int rootY = find(y);
            if (rootX != rootY) {
                parent[rootX] = rootY;
            }
        };

        // Build connected components using allowed swaps
        for (const auto& swap : allowedSwaps) {
            unite(swap[0], swap[1]);
        }

        // Group indices by component root
        unordered_map<int, vector<int>> components;
        for (int i = 0; i < n; ++i) {
            int root = find(i);
            components[root].push_back(i);
        }

        int totalMatches = 0;

        // Count element frequencies and compute matches within each component
        for (auto& [root, indices] : components) {
            unordered_map<int, int> sourceCount, targetCount;
            for (int idx : indices) {
                sourceCount[source[idx]]++;
                targetCount[target[idx]]++;
            }
            for (const auto& [val, count] : sourceCount) {
                if (targetCount.count(val)) {
                    totalMatches += min(count, targetCount[val]);
                }
            }
        }

        return n - totalMatches;
    }
};
```

**Explanation:**
- The Union-Find structure groups indices into components where elements can be freely rearranged.
- Within each component, we use hash maps to count how many times each number appears in the source and target.
- We sum up the minimum of these counts across all numbers to determine how many positions can match after reordering.
- The minimum Hamming distance is then the total number of elements minus these matches.

**Time Complexity:** O(n ⋅ α(n) + k), where n is the array length, k is the number of allowed swaps, and α(n) is the nearly constant inverse Ackermann function from Union-Find with path compression.

**Space Complexity:** O(n), due to storage for Union-Find parent array, component indices, and frequency maps.

This solution is optimal and robust for the given constraints, efficiently handling up to 10⁵ elements and swaps.