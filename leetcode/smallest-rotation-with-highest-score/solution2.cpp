#include <vector>
#include <numeric>
#include <algorithm>

class Solution {
public:
    int bestRotation(std::vector<int>& nums) {
        const int n = nums.size();
        // change[k] tracks the net score change when moving from rotation (k-1) to k
        std::vector<int> change(n, 0);

        for (int i = 0; i < n; ++i) {
            // Calculate where this element will lose and gain a point
            int loss_k = (i - nums[i] + 1 + n) % n;
            int gain_k = (i + 1) % n;

            change[loss_k]--;
            change[gain_k]++;
        }

        int max_score = 0;
        int current_score = 0;
        int best_k = 0;

        // Iterate through all possible rotations to find the maximum relative score
        for (int k = 1; k < n; ++k) {
            current_score += change[k];
            if (current_score > max_score) {
                max_score = current_score;
                best_k = k;
            }
        }

        return best_k;
    }
};

