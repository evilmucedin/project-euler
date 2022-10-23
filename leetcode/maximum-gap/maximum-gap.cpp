#include "../header.h"

using IntVector = vector<int>;

void radixSort(IntVector& vct) {
    IntVector counts((1 << 8) + 1);
    IntVector temp(vct.size());
    for (auto shift : {0, 8, 16, 24}) {
        const u32 mask = ((1UL << 8) - 1) << shift;
        fill(counts.begin(), counts.end(), 0);
        for (auto v: vct) {
            auto bucket = ((v & mask) >> shift) + 1;
            ++counts[bucket];
        }
        for (int i = 1; i <= (1 << 8); ++i) {
            counts[i] += counts[i - 1];
        }
        for (auto v: vct) {
            auto bucket = (v & mask) >> shift;
            temp[ counts[bucket]++ ] = v;
        }
        vct.swap(temp);
    }
}

class Solution {
public:
    int maximumGap(vector<int> nums) {
        if (nums.size() < 2) {
            return 0;
        }

        radixSort(nums);
        int max = nums[1] - nums[0];
        for (int i = 2; i < nums.size(); ++i) {
            int d = nums[i] - nums[i - 1];
            if (d > max) {
                max = d;
            }
        }

        return max;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.maximumGap({3, 6, 9, 1}) << endl;

    return 0;
}
