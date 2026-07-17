#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

class Solution {
public:
    int numMatchingSubseq(string s, vector<string>& words) {
        // Create a map to store the indices of each character in the string `s`
        unordered_map<char, vector<int>> char_indices;
        for (int i = 0; i < s.size(); ++i) {
            char_indices[s[i]].push_back(i);
        }

        int count = 0;

        // Function to check if a word is a subsequence of `s`
        auto isSubsequence = [&](const string& word) -> bool {
            int prev_index = -1; // Tracks the previous character's index in `s`
            for (char c : word) {
                // If the character is not found in `s`, return false
                if (!char_indices.count(c)) {
                    return false;
                }
                // Use binary search to find the next valid index for the current character
                const auto& indices = char_indices[c];
                auto it = upper_bound(indices.begin(), indices.end(), prev_index);
                if (it == indices.end()) {
                    return false; // No valid index found
                }
                prev_index = *it;
            }
            return true;
        };

        // Check each word in `words`
        for (const string& word : words) {
            if (isSubsequence(word)) {
                ++count;
            }
        }

        return count;
    }
};

