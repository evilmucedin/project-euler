```cpp
#include <vector>

using namespace std;

/*
 * Below is the interface for Iterator, which is already defined for you.
 * **DO NOT** modify the interface for Iterator.
 *
 *  class Iterator {
 *		struct Data;
 * 		Data* data;
 *  public:
 *		Iterator(const vector<int>& nums);
 * 		Iterator(const Iterator& iter);
 *
 * 		// Returns the next element in the iteration.
 *		int next();
 *
 *		// Returns true if the iteration has more elements.
 *		bool hasNext() const;
 *	};
 */

class PeekingIterator : public Iterator {
private:
    // Buffer to store the next element without consuming it
    int _cachedValue;
    // Flag to indicate if there is a valid element in the buffer
    bool _hasNextFlag;

public:
    PeekingIterator(const vector<int>& nums) : Iterator(nums) {
        // Initialize the base iterator first
        // Check if the underlying iterator has elements
        _hasNextFlag = Iterator::hasNext();
        
        // If it does, fetch the first element and cache it
        if (_hasNextFlag) {
            _cachedValue = Iterator::next();
        }
    }
    
    // Returns the next element in the iteration without advancing the iterator.
    int peek() {
        // Since all calls to peek are valid (guaranteed by hasNext), 
        // we can safely return the cached value.
        return _cachedValue;
    }
    
    // hasNext() and next() should behave the same as in the Iterator interface.
    // Override them if needed.
    int next() {
        // Return the current cached value
        int result = _cachedValue;
        
        // Advance the underlying iterator and cache the next element if available
        _hasNextFlag = Iterator::hasNext();
        if (_hasNextFlag) {
            _cachedValue = Iterator::next();
        }
        
        return result;
    }
    
    bool hasNext() const {
        // Return the status flag indicating if there is more data
        return _hasNextFlag;
    }
};
```

### Explanation of the Solution

1.  **State Management (`O(1)` Space):**
    *   We add two private member variables: `int _cachedValue` to store the "look-ahead" element, and `bool _hasNextFlag` to track if this cached value is valid.
    *   This avoids copying the entire underlying data structure (`nums`), adhering to the constraint.

2.  **Initialization (`Constructor`):**
    *   We first initialize the base `Iterator` class.
    *   We immediately check if the iterator has elements using `Iterator::hasNext()`.
    *   If yes, we consume the first element using `Iterator::next()` and store it in `_cachedValue`. This pre-loading allows `peek()` to work immediately without side effects.

3.  **`peek()` Operation (`O(1)` Time):**
    *   Simply returns `_cachedValue`. It does not modify any state or advance the iterator.

4.  **`next()` Operation (`O(1)` Time):**
    *   Returns the current `_cachedValue`.
    *   Immediately after, it checks if the underlying iterator has more elements.
    *   If yes, it fetches the next element using `Iterator::next()` and updates `_cachedValue`. This ensures the "look-ahead" buffer is always prepared for the next `peek()` or `next()` call.

5.  **`hasNext()` Operation (`O(1)` Time):**
    *   Returns `_hasNextFlag`, which accurately reflects whether there are elements remaining in the underlying iterator.

### Complexity Analysis
*   **Time Complexity:** All operations (`next`, `peek`, `hasNext`) are $O(1)$. We do not iterate through the array inside these methods; we only interact with the base iterator's single-step methods.
*   **Space Complexity:** $O(1)$ extra space is used to store the single integer cache and the boolean flag. We do not store a copy of the input vector.

### Follow-up (Generic/Template)
To extend this design to work with all types (not just `int`), you would use C++ Templates. You would change the class definition to:
```cpp
template <typename T>
class PeekingIterator : public Iterator<T> { ... }
```
And adjust member variables to `T _cachedValue`. The logic remains identical.