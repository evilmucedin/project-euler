```cpp
class Solution {
public:
    vector<string> addOperators(string num, int target) {
        vector<string> result;
        backtrack(num, 0, "", 0, 0, target, result);
        return result;
    }
    
private:
    void backtrack(const string& num, int index, string path,
                   long long currentVal, long long prevVal,
                   int target, vector<string>& result) {
        // Base case: if all digits have been used
        if (index == num.size()) {
            if (currentVal == target) {
                result.push_back(path);
            }
            return;
        }
        
        // Try all possible numbers starting from current index
        for (int i = index; i < num.size(); i++) {
            // Skip numbers with leading zeros (except single "0")
            if (i > index && num[index] == '0') {
                break;
            }
            
            // Extract current number substring
            string numStr = num.substr(index, i - index + 1);
            long long numVal = stoll(numStr);
            
            if (index == 0) {
                // First number: no operator before it
                backtrack(num, i + 1, numStr, numVal, numVal, target, result);
            } else {
                // Try '+' operator
                backtrack(num, i + 1, path + "+" + numStr,
                         currentVal + numVal, numVal, target, result);
                
                // Try '-' operator
                backtrack(num, i + 1, path + "-" + numStr,
                         currentVal - numVal, -numVal, target, result);
                
                // Try '*' operator - need to handle precedence
                // Undo previous operation and apply multiplication
                backtrack(num, i + 1, path + "*" + numStr,
                         currentVal - prevVal + prevVal * numVal,
                         prevVal * numVal, target, result);
            }
        }
    }
};
```

### Explanation

This solution uses **backtracking** to explore all possible ways of inserting `+`, `-`, `*` operators (or no operator) between digits to form valid expressions evaluating to the target.

#### Key Components:

- **`backtrack` Function Parameters:**
  - `index`: Current position in the input string
  - `path`: Expression built so far
  - `currentVal`: Current value of the expression
  - `prevVal`: Last operand (used for multiplication precedence)
  - `target`: Desired result
  - `result`: Output vector to store valid expressions

- **Leading Zeros Check:**
  - Prevents numbers like "05" by breaking the loop when `num[index] == '0'` and `i > index`.

- **Multiplication Precedence:**
  - To correctly apply `*`, we subtract the previous operand from `currentVal` and add `prevVal * numVal`.
  - `prevVal` is updated to `prevVal * numVal` so future operations respect precedence.

#### Time Complexity:
- **O(4^N)**, where N is the number of digits.
- At each digit position, there are up to 4 choices: `+`, `-`, `*`, or concatenate.

#### Space Complexity:
- **O(N)** for recursion stack and path storage.

#### Notes:
- `long long` is used for intermediate values to avoid overflow.
- The solution is efficient for inputs up to length 10, as per constraints.

This implementation is concise, handles edge cases, and respects operator precedence accurately.