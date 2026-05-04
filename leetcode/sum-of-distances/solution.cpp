To efficiently solve the problem of computing the sum of absolute differences between each index `i` and all other indices `j` where `nums[i] == nums[j]` and `i ≠ j`, we can use a grouping and prefix sum technique. This reduces the time complexity from O(n²) to O(n), making it suitable for large input sizes up to 10⁵.

### Approach

1. **Group Indices by Value**: Use a hash map to store all indices corresponding to each unique value in `nums`.
2. **Compute Prefix Sums for Each Group**: For each group of indices sharing the same value, compute prefix sums to allow fast range sum queries.
3. **Calculate Distance Efficiently**: For each index in a group:
   - The sum of distances to earlier indices = `i * current_index - prefix_sum_before_i`
   - The sum of distances to later indices = `suffix_sum - (group_size - 1 - i) * current_index`
4. **Combine Results**: Add both parts to compute the final value for `arr[i]`.

### C++ Implementation

```cpp
class Solution {
public:
    vector<long long> distance(vector<int>& nums) {
        int n = nums.size();
        vector<long long> result(n, 0);
        unordered_map<int, vector<int>> valueToIndices;

        // Group indices by value
        for (int i = 0; i < n; ++i) {
            valueToIndices[nums[i]].push_back(i);
        }

        // Process each group
        for (auto& [value, indices] : valueToIndices) {
            int k = indices.size();
            if (k <= 1) continue;

            // Compute prefix sums of indices
            vector<long long> prefix(k + 1, 0);
            for (int i = 0; i < k; ++i) {
                prefix[i + 1] = prefix[i] + indices[i];
            }

            // Compute distance for each index in this group
            for (int i = 0; i < k; ++i) {
                long long idx = indices[i];
                long long leftSum = (long long)i * idx - prefix[i];
                long long rightSum = (prefix[k] - prefix[i + 1]) - (long long)(k - 1 - i) * idx;
                result[idx] = leftSum + rightSum;
            }
        }

        return result;
    }
};
```

### Complexity Analysis

- **Time Complexity**: O(n) — each element is visited a constant number of times (once during grouping, and once per group during distance computation).
- **Space Complexity**: O(n) — for storing grouped indices and prefix sums.

### Example

For `nums = [1,3,1,1,2]`:
- Index 0 (value 1): other indices with same value = 2, 3 → |0-2| + |0-3| = 2 + 3 = 5
- Index 2 (value 1): others = 0, 3 → |2-0| + |2-3| = 2 + 1 = 3
- Index 3 (value 1): others = 0, 2 → |3-0| + |3-2| = 3 + 1 = 4
- Index 1 (value 3): no other 3s → 0
- Index 4 (value 2): no other 2s → 0

**Output**: `[5, 0, 3, 4, 0]`

This solution is optimal, efficient, and scalable for large input constraints.