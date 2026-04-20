Here's an optimized solution to the "Dungeon Game" problem using dynamic programming. It calculates the minimum initial health the knight needs to survive the dungeon and reach the princess safely, ensuring that health never drops to 0 or below at any point.

```cpp
class Solution {
public:
    int calculateMinimumHP(vector<vector<int>>& dungeon) {
        int m = dungeon.size();
        int n = dungeon[0].size();

        // dp[j] stores the minimum health needed to survive from column j to the end
        vector<int> dp(n, 0);

        // Initialize the last cell (princess's location)
        dp[n - 1] = max(1, 1 - dungeon[m - 1][n - 1]);

        // Initialize the last row (bottom row of dungeon)
        for (int j = n - 2; j >= 0; --j) {
            dp[j] = max(1, dp[j + 1] - dungeon[m - 1][j]);
        }

        // Process remaining rows from bottom to top
        for (int i = m - 2; i >= 0; --i) {
            // Update last column of current row
            dp[n - 1] = max(1, dp[n - 1] - dungeon[i][n - 1]);

            // Update other columns
            for (int j = n - 2; j >= 0; --j) {
                int minHealthNeeded = min(dp[j], dp[j + 1]);
                dp[j] = max(1, minHealthNeeded - dungeon[i][j]);
            }
        }

        return dp[0];
    }
};
```

### Explanation

- We work backwards from the princess's cell at the bottom-right to the starting cell at the top-left.
- At each cell, we compute the minimum health needed before entering it so that the knight survives through to the end.
- The `dp` array is optimized to use only a single row, reducing space complexity.
- For each cell `(i, j)`, we consider the minimum required health when moving either down or right, and adjust for the current room's effect (damage or healing).
- We enforce that the knight's health never drops below 1 using `max(1, ...)`.

### Complexity

- **Time Complexity:** O(m × n), where m and n are the dimensions of the dungeon.
- **Space Complexity:** O(n), using a 1D DP array to optimize space.

### Example Walkthrough

For input: `dungeon = [[-2, -3, 3], [-5, -10, 1], [10, 30, -5]]`

- Start from bottom-right cell with value `-5` → needs at least `6` health to survive.
- Traverse backwards, updating health requirements based on path choices.
- Final result: minimum initial health needed is `7`.

This solution ensures the knight can reach the princess safely with the least possible starting health.