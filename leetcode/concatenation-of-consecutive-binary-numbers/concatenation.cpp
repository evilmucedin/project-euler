#include "../header.h"

static constexpr long long int MOD = 1000000007;

class Solution {
   public:
    static constexpr size_t N = 100001;
    vector<int> cache_;

    Solution() {
        cache_.resize(N);
        for (int i = 1; i < N; ++i) {
            cache_[i] = add(cache_[i - 1], i);
        }
    }

    static int add(long long int result, int n) {
        int nn = n;
        int len = 0;
        while (nn) {
            nn >>= 1;
            ++len;
        }
        result = (result << len) % MOD;
        result = (result + n) % MOD;
        return result;
    }

    static int add_(int result, int n) {
        if (!n) {
            return result;
        }

        result = add(result, n >> 1);
        result = (2 * result + (n & 1)) % MOD;
        return result;
    }

    int concatenatedBinary(int n) {
        return cache_[n];
    }
};

int main() {
    Timer t("Subarray timer");
    Solution sol;
    cerr << sol.concatenatedBinary(1) << endl;
    cerr << sol.concatenatedBinary(3) << endl;
    cerr << sol.concatenatedBinary(12) << endl;

    return 0;
}
