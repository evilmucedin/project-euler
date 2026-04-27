 To solve this problem efficiently, we need to find the smallest base \( k \) such that the number \( n \) represented in base \( k \) consists only of 1's. The key idea is to iterate over possible values of \( k \) and check if they satisfy the condition. However, instead of a naive iteration (which would be too slow), we can use mathematical properties to bound our search.

Here's a step-by-step approach:

1. **Convert the input string to an integer**: This is straightforward using `stoll` from the standard library.
2. **Iterate over possible values of \( k \)**: The maximum value for \( k \) can be determined by noting that if we have a number with all 1's in base \( k \), then the sum of powers of \( k \) must equal \( n \). Therefore, \( k \) should not exceed the logarithm (base 2) of \( n \), rounded up.
3. **Check for each \( k \)**: For each potential \( k \), calculate if it can be a valid base by comparing the sum of the geometric series with \( n \). If they match, then \( k \) is a candidate.
4. **Return the smallest valid \( k \)**: Since we iterate from the largest possible value down to 2 (the minimum base), the first valid \( k \) found will be the smallest.

Here's the implementation in C++:

```cpp
#include <string>
#include <cmath>
using namespace std;

class Solution {
public:
    string smallestGoodBase(string n) {
        long long num = stoll(n);
        // Find the maximum possible base by considering log2(num) + 1
        for (int m = log2(num) + 1; m >= 2; --m) {
            long long k = pow(num, 1.0 / (m - 1));
            long long sum = 1, curr = 1;
            for (int i = 0; i < m; ++i) {
                if (curr > num / k) break; // Overflow check
                curr *= k;
                sum += curr;
            }
            if (sum == num) return to_string(k);
        }
        return to_string(num - 1); // Fallback, though theoretically unnecessary
    }
};
```

### Explanation:
- **Conversion and Initial Setup**: Convert the input string `n` to a long long integer. Compute the logarithm base 2 of `num` and add 1 to find an upper bound for \( k \). This is because if \( n \) can be represented as all 1's in some base \( k \), then \( k \) must not exceed this value.
- **Finding Base**: For each potential base \( k \), calculate the sum of the geometric series up to \( m \) terms (where \( m \) is determined by our upper bound). If the sum matches `num`, then \( k \) is a valid base.
- **Return Result**: The first valid base found during iteration will be the smallest, so we return it as a string. If no base is found, which should not happen given the constraints but is theoretically possible to handle by returning an alternative value.

This approach ensures that we efficiently find the smallest good base without unnecessary computations.