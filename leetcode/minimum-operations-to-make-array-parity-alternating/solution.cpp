class Solution {
    public:
        vector<int> makeParityAlternating(vector<int>& nums) {
            int n = nums.size();
            if (n <= 1) return nums;

            auto isEven = [](int x) { return x % 2 == 0; };

            // Count mismatches for two possible patterns:
            // pattern 0: even index -> even, odd index -> odd
            // pattern 1: even index -> odd,  odd index -> even
            int mismatchesPattern0 = 0;
            int mismatchesPattern1 = 0;

            for (int i = 0; i < n; ++i) {
                bool evenParity = isEven(nums[i]);
                if (i % 2 == 0) {
                    // For pattern 0, expect even; for pattern 1, expect odd
                    if (!evenParity) ++mismatchesPattern0;
                    if (evenParity) ++mismatchesPattern1;
                } else {
                    // For pattern 0, expect odd; for pattern 1, expect even
                    if (evenParity) ++mismatchesPattern0;
                    if (!evenParity) ++mismatchesPattern1;
                }
            }

            bool usePattern0 = mismatchesPattern0 <= mismatchesPattern1;

            vector<int> res = nums;
            for (int i = 0; i < n; ++i) {
                bool needEven;
                if (usePattern0) {
                    needEven = (i % 2 == 0);      // pattern 0: even index -> even
                } else {
                    needEven = (i % 2 == 1);      // pattern 1: odd index  -> even
                }

                bool curEven = isEven(res[i]);
                if (needEven != curEven) {
                    // Flip parity with a single +1 or -1; works for negatives too.
                    if (res[i] >= 0)
                        ++res[i];
                    else
                        --res[i];
                }
            }

            return res;
        }
    };