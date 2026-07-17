#include <vector>
#include <algorithm>

class Solution {
public:
    int bestRotation(vector<int>& nums) {
        int n = nums.size();
        
        // Initialize an array to store the effective indices after rotation
        std::vector<int> effectiveIndices(n);
        
        // Calculate the effective index for each element after rotation
        for (int i = 0; i < n; ++i) {
            int shift = (i - nums[i] + 1) % n;
            if (shift >= 0) {
                effectiveIndices[shift]++;
            } else {
                effectiveIndices[n + shift]++;
            }
        }
        
        // Find the rotation index that maximizes the score
        int maxScore = 0;
        int bestIndex = -1;
        for (int i = 0; i < n; ++i) {
            if (effectiveIndices[i] > maxScore) {
                maxScore = effectiveIndices[i];
                bestIndex = i;
            }
        }
        
        return bestIndex;
    }
};

