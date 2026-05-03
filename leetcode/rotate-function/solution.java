To solve this problem, we need to calculate the rotation function \( F(k) \) for each possible rotation of the array and find the maximum value. The rotation function is defined as:

\[ F(k) = 0 \cdot arrk[0] + 1 \cdot arrk[1] + \ldots + (n - 1) \cdot arrk[n - 1] \]

where \( arrk \) is the array obtained by rotating \( nums \) by \( k \) positions clock-wise.

Here's a Java implementation that addresses the given requirements:

```java
class Solution {
    public int maxRotateFunction(int[] nums) {
        if (nums == null || nums.length == 0) return 0;

        int n = nums.length;
        int sum = 0;
        int f = 0;

        // Calculate the initial sum of elements and the initial value of F(0)
        for (int i = 0; i < n; i++) {
            sum += nums[i];
            f += i * nums[i];
        }

        int maxF = f;

        // Iterate over each rotation
        for (int k = 1; k < n; k++) {
            // Calculate the next value of F(k)
            f = f + sum - n * nums[n - k];
            maxF = Math.max(maxF, f);
        }

        return maxF;
    }
}
```

### Explanation:

1. **Initialization**:
   - `sum` is initialized to the sum of all elements in the array.
   - `f` is initialized to the value of \( F(0) \), which is calculated as the dot product of the indices and the array elements.

2. **Initial Calculation**:
   - We iterate over the array once to calculate the initial sum and the initial value of \( F(0) \).

3. **Rotation Loop**:
   - For each rotation from 1 to \( n-1 \):
     - Update `f` by adding the total sum minus the product of \( n \) and the element that is moved out of the array in this rotation.
     - Update `maxF` with the maximum value encountered so far.

4. **Return Result**:
   - After all rotations, `maxF` contains the maximum value of \( F(0), F(1), \ldots, F(n-1) \).

This approach efficiently computes the required maximum rotation function value in linear time, making it suitable for large input sizes as specified by the constraints.