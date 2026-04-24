# Shortest Unsorted Continuous Subarray

## Problem Summary

Given an integer array, find the length of the shortest continuous subarray that, when sorted, makes the entire array sorted in non-decreasing order.

## Approach

We use an efficient method that compares the original array with a sorted version:

1. **Create a sorted copy** of the input array.
2. **Scan from both ends**:
   - From the left, find the first index where the original array differs from the sorted version.
   - From the right, find the first index where they differ.
3. **Calculate the subarray length**: If mismatches are found, return `right - left + 1`. Otherwise, return 0 (array is already sorted).

This approach has **O(n log n)** time complexity (due to sorting) and **O(n)** space complexity (for the sorted copy).

## C++ Solution

```cpp
#include <vector>
#include <algorithm>
using namespace std;

class Solution {
public:
    int findUnsortedSubarray(vector<int>& nums) {
        int n = nums.size();
        if (n <= 1) return 0;
        
        vector<int> sorted = nums;
        sort(sorted.begin(), sorted.end());
        
        int left = 0;
        while (left < n && nums[left] == sorted[left]) {
            left++;
        }
        
        if (left == n) return 0;
        
        int right = n - 1;
        while (right >= 0 && nums[right] == sorted[right]) {
            right--;
        }
        
        return right - left + 1;
    }
};
```

## Example Walkthrough

**Input:** `nums = [2,6,4,8,10,9,15]`  
**Sorted:** `[2,4,6,8,9,10,15]`

- Left mismatch at index 1 (`6 != 4`)
- Right mismatch at index 5 (`9 != 10`)
- Result: `5 - 1 + 1 = 5`

## Complexity

- **Time:** O(n log n) due to sorting.
- **Space:** O(n) for the sorted array.

## Alternative (O(n) Time)

For large inputs where sorting is costly, consider an O(n) approach by tracking minimum/maximum violations and expanding boundaries accordingly.

## Test Cases

```cpp
// Example 1: [2,6,4,8,10,9,15] => 5
// Example 2: [1,2,3,4] => 0 (already sorted)
// Example 3: [1] => 0 (single element)
```

This solution is clean, efficient, and handles all edge cases including already-sorted arrays and single-element inputs.