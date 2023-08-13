#include "../header.h"

class Solution {
public:
const int MOD = 1e9 + 7;

long long modInverse(long long a, long long m) {
    long long m0 = m, t, q;
    long long x0 = 0, x1 = 1;

    if (m == 1)
        return 0;

    while (a > 1) {
        q = a / m;
        t = m;

        m = a % m, a = t;
        t = x0;

        x0 = x1 - q * x0;
        x1 = t;
    }

    if (x1 < 0)
        x1 += m0;

    return x1;
}

int maxPrimeScore(int num) {
    int count = 0;
    for (int i = 2; i * i <= num; ++i) {
        if (num % i == 0) {
            ++count;
            while (num % i == 0) {
                num /= i;
            }
        }
    }
    if (num > 1) {
        ++count;
    }
    return count;
}

int maximumScore(const vector<int>& nums, int k) {
    const int n = nums.size();

    vector<int> maxPrimeScores(n);
    for (int i = 0; i < n; ++i) {
        maxPrimeScores[i] = maxPrimeScore(nums[i]);
    }

    using PII = pair<long long, int>;
    vector<PII> sortedScores;
    for (int i = 0; i < n; ++i) {
        sortedScores.emplace_back(PII(nums[i], i));
    }
    sort(sortedScores.begin(), sortedScores.end());
    reverse(sortedScores.begin(), sortedScores.end());

    long long score = 1;

    for (int i = 0; i < n && k > 0; ++i) {
        int pos = sortedScores[i].second;
        int l = pos;
        while (l >= 1 && maxPrimeScores[l - 1] < maxPrimeScores[pos]) {
            --l;
        }
        int r = pos;
        while (r + 1 < n && maxPrimeScores[r + 1] <= maxPrimeScores[pos]) {
            ++r;
        }
        int ints = (r - pos + 1)*(pos - l + 1);
        int pow = min(ints, k);
        cerr << sortedScores[i] << " " << l << " " << r << " " << ints << " " <<  pow << endl;
        k -= ints;
        for (int j = 0; j < pow; ++j) {
            score = (score*sortedScores[i].first) % MOD;
        }
    }

    return score;
}

};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.maximumScore({8, 3, 9, 3, 8}, 2) << endl;
    cerr << sol.maximumScore({19, 12, 14, 6, 10, 18}, 3) << endl;
    // 256720975
    cerr << sol.maximumScore({3289,2832,14858,22011}, 6) << endl;

    return 0;
}
