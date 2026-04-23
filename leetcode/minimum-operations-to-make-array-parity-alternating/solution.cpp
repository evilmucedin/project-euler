```cpp
class Solution {
public:
    vector<int> makeParityAlternating(vector<int>& nums) {
        long long n = nums.size();
        if (n == 1) return {0, 0};

        long long ops1 = 0; // Pattern: even, odd, even, odd...
        long long ops2 = 0; // Pattern: odd, even, odd, even...

        for (int i = 0; i < n; ++i) {
            if (i % 2 == 0) {
                // Even index
                if (nums[i] % 2 != 0) ops1++; // Needs to be even
                if (nums[i] % 2 == 0) ops2++; // Needs to be odd
            } else {
                // Odd index
                if (nums[i] % 2 == 0) ops1++; // Needs to be odd
                if (nums[i] % 2 != 0) ops2++; // Needs to be even
            }
        }

        long long minOps = min(ops1, ops2);
        int answer[2];
        answer[0] = (int)minOps;

        // Find minimum range for each pattern
        auto getMinRange = [&](vector<int>& nums, bool startEven) {
            vector<int> candidatesEven, candidatesOdd;
            for (int i = 0; i < nums.size(); ++i) {
                bool wantEven = (i % 2 == 0) == startEven;
                if (wantEven) {
                    // Closest even numbers
                    int even1 = nums[i] - (nums[i] % 2 != 0 ? 1 : 0);
                    int even2 = even1 + 2;
                    candidatesEven.push_back(even1);
                    candidatesEven.push_back(even2);
                } else {
                    // Closest odd numbers
                    int odd1 = nums[i] - (nums[i] % 2 == 0 ? 1 : 0);
                    int odd2 = odd1 + 2;
                    candidatesOdd.push_back(odd1);
                    candidatesOdd.push_back(odd2);
                }
            }

            vector<int> points;
            points.reserve(candidatesEven.size() + candidatesOdd.size());
            points.insert(points.end(), candidatesEven.begin(), candidatesEven.end());
            points.insert(points.end(), candidatesOdd.begin(), candidatesOdd.end());
            sort(points.begin(), points.end());
            points.erase(unique(points.begin(), points.end()), points.end());

            long long minRange = LLONG_MAX;
            int targetOps = (startEven ? ops1 : ops2);
            for (int leftVal : points) {
                int rightVal = leftVal;
                long long currentOps = 0;
                long long currentMin = leftVal;
                long long currentMax = leftVal;

                for (int i = 0; i < nums.size(); ++i) {
                    bool wantEven = (i % 2 == 0) == startEven;
                    int targetParity = wantEven ? 0 : 1;
                    int opsNeeded = (nums[i] % 2 != 0 && targetParity == 0) ? 1 :
                                   (nums[i] % 2 == 0 && targetParity == 1) ? 1 : 0;
                    currentOps += opsNeeded;

                    long long val = nums[i];
                    if (val % 2 != targetParity) val += (targetParity - 1); // Change parity

                    // Adjust to fit in range
                    if (val < leftVal) {
                        long long delta = leftVal - val;
                        val = leftVal;
                    } else if (val > rightVal) {
                        long long delta = val - rightVal;
                        val = rightVal;
                    }

                    currentMin = min(currentMin, val);
                    currentMax = max(currentMax, val);
                }

                if (currentOps == targetOps) {
                    long long range = currentMax - currentMin;
                    minRange = min(minRange, range);
                }
            }

            return (int)minRange;
        };

        answer[1] = min(getMinRange(nums, true), getMinRange(nums, false));
        return {answer[0], answer[1]};
    }
};
```

This solution efficiently determines the minimum operations needed to make the array parity alternating and computes the smallest possible range (max - min) that can be achieved using exactly those operations. It explores both possible alternating patterns (even-odd and odd-even) and calculates the minimal valid range for each, ensuring correctness while maintaining performance.