To solve this problem efficiently, we need to minimize the number of guesses by systematically narrowing down the candidate pool based on feedback from the Master API. Each guess returns the number of exact character-position matches with the secret word. We use this information to eliminate words that are inconsistent with the observed match count.

### Approach

1. **Compute Similarity**: Define a helper function that calculates how many characters match in both value and position between two 6-letter words.
2. **Iterative Guessing**: In each iteration:
   - Pick a candidate word (e.g., the first one).
   - Submit it to `Master.guess()` and record the number of matches.
   - If the match count is 6, the secret word has been found.
   - Otherwise, filter the candidate list to only include words with the same similarity score to the guessed word as the feedback indicates.
3. **Repeat**: Continue filtering and guessing until the secret word is found.

This approach leverages the match count feedback to eliminate inconsistent candidates and converges efficiently without brute-force exploration.

### Solution Code

```cpp
/**
 * // This is the Master's API interface.
 * // You should not implement it, or speculate about its implementation
 * class Master {
 *   public:
 *     int guess(string word);
 * };
 */
class Solution {
public:
    void findSecretWord(vector<string>& words, Master& master) {
        auto similarity = [](const string& w1, const string& w2) {
            int count = 0;
            for (int i = 0; i < 6; ++i) {
                if (w1[i] == w2[i]) ++count;
            }
            return count;
        };

        while (!words.empty()) {
            string guess = words[0];
            int matchCount = master.guess(guess);

            if (matchCount == 6) break;

            vector<string> nextCandidates;
            for (const string& word : words) {
                if (similarity(word, guess) == matchCount) {
                    nextCandidates.push_back(word);
                }
            }

            words = std::move(nextCandidates);
        }
    }
};
```

### Explanation

- **Filtering Logic**: After each guess, we only retain candidate words that match the returned similarity count with the guessed word. This ensures we stay within the solution space compatible with the observed data.
- **Termination**: The loop ends when we receive 6 matches (the secret word is found) or when no candidates remain.
- **Efficiency**: This filtering strategy reduces the candidate set logarithmically, ensuring the secret word is found within the allowed guesses.

### Complexity Analysis

- **Time Complexity**: In the worst case, O(n × m), where n is the number of words and m is the length of each word (constant 6).
- **Space Complexity**: O(n) for storing filtered candidates during iteration.

This solution strikes a balance between simplicity and efficiency, making it well-suited for the constraints and performance expectations of the problem.