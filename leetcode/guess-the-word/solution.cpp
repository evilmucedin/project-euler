 To solve this problem efficiently without using a brute-force approach, we can leverage the information provided by the `Master.guess` function and use a probabilistic strategy to guess the secret word. Here's a step-by-step plan:

1. **Initial Guess**: Start with an initial guess. A good starting point is to choose a word that has minimal overlap with other words in the list, as this can help reduce the number of possible candidates quickly.
2. **Evaluate Guesses**: After each guess, use the feedback from `Master.guess` to narrow down the possibilities. The feedback will tell us how many characters match exactly (including position).
3. **Refine Candidates**: Based on the matches obtained, refine the list of potential words by eliminating those that cannot be the secret word based on the current guesses.
4. **Iterate**: Continue this process until you either find the secret word or exhaust your allowed number of guesses.

Here's a C++ implementation of this strategy:

```cpp
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdlib>
#include <ctime>
using namespace std;

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
        srand(time(0)); // Seed for random number generation
        
        // Initial guess - a random word from the list
        string guess = words[rand() % words.size()];
        int allowedGuesses = 10;
        
        while (allowedGuesses > 0) {
            int matches = master.guess(guess);
            
            if (matches == -1 || matches == 6) { // If the guess is not in list or matched all characters
                break;
            }
            
            vector<string> candidates;
            for (const string& word : words) {
                if (match(guess, word) == matches) {
                    candidates.push_back(word);
                }
            }
            
            // Update the list of possible guesses to be only those that match with the current guess
            words = candidates;
            
            // Choose a new guess from the remaining candidates
            if (!words.empty()) {
                guess = words[rand() % words.size()];
            } else {
                break;
            }
            
            allowedGuesses--;
        }
        
        if (allowedGuesses == 0 && master.guess(guess) != 6) {
            // If we run out of guesses without finding the secret word
            cout << "Either you took too many guesses, or you did not find the secret word." << endl;
        } else {
            cout << "You guessed the secret word correctly." << endl;
        }
    }
    
private:
    int match(const string& a, const string& b) {
        int count = 0;
        for (int i = 0; i < 6; ++i) {
            if (a[i] == b[i]) {
                count++;
            }
        }
        return count;
    }
};
```

### Explanation:
1. **Initialization**: We start by seeding the random number generator and choosing an initial guess randomly from the list of words.
2. **Guessing Loop**: In each iteration, we call `Master.guess` with our current guess. If it returns -1 or 6 (meaning all characters match), we stop the loop.
3. **Filtering Candidates**: Based on the number of matches, we filter out words that do not have the same character count at each position as the guessed word. This is done using a helper function `match`.
4. **Updating List**: We update our list of possible guesses to only include those that match with the current guess. If there are no more candidates, we break the loop.
5. **Output**: Finally, if we exhaust all allowed guesses without finding the secret word, or if we find it correctly within the allowed number of tries, we output the appropriate message.

This approach leverages probabilistic reasoning to efficiently narrow down the possible words and make efficient use of the limited number of guess attempts.