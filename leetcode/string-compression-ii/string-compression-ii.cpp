#include "../header.h"

class Solution {
   public:
    int getLengthOfOptimalCompression(string s, int k) {
        dp.clear();
        dp.resize(s.length(), vector<int>(k + 1, kMax));
        return compression(s, 0, k);
    }

   private:
    static constexpr int kMax = 101;
    vector<vector<int>> dp;

    int compression(const string& s, int i, int k) {
        if (k < 0) {
            return kMax;
        }
        if (i == s.length() || s.length() <= k + i) {
            return 0;
        }
        if (dp[i][k] != kMax) {
            return dp[i][k];
        }

        int maxFreq = 0;  // Max freq in s[i..j]
        vector<int> count(128);

        for (int j = i; j < s.length(); ++j) {
            ++count[s[j]];
            maxFreq = max(maxFreq, count[s[j]]);
            dp[i][k] = min(dp[i][k], getLength(maxFreq) + compression(s, j + 1, k - (j - i + 1 - maxFreq)));
        }

        return dp[i][k];
    }

    int getLength(int maxFreq) const {
        if (maxFreq == 1) {
            return 1;
        }
        if (maxFreq < 10) {
            return 2;
        }
        if (maxFreq < 100) {
            return 3;
        }
        return 4;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.getLengthOfOptimalCompression("aaabcccd", 2) << endl
         << sol.getLengthOfOptimalCompression("aabbaa", 2) << endl
         << sol.getLengthOfOptimalCompression("aaaaaaaaaaa", 0) << endl;

    return 0;
}
