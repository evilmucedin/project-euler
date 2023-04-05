#include "../header.h"

#include <glog/logging.h>

class Solution {
public:
    int minimizeArrayValue(const vector<int>& nums) {
        const int n = nums.size();

        long long sum = 0;
        for (long long i : nums) {
            sum += i;
        }

        long long l = max<long long>(sum/n, max(0, *min_element(nums.begin(), nums.end())));
        long long r = max<long long>(sum/n, max(0, *max_element(nums.begin(), nums.end())));

        vector<long long> ps(n);
        ps[0] = nums[0];
        for (int i = 1; i < n; ++i) {
            ps[i] = ps[i - 1] + nums[i];
        }

        auto good = [&](long long x) {
            for (long long i = 0; i < n; ++i) {
                if (ps[i] > x*(i + 1)) {
                    // LOG(INFO) << OUT(i) << OUT(ps[i]) << OUT(x);
                    return false;
                }
            }
            return true;
        };

        while (l + 2 < r) {
            long long m = l + ((r - l) / 2);
            if (good(m)) {
                r = m;
            } else {
                l = m;
            }
        }

        while (r > 0 && good(r - 1)) {
            --r;
        }

        while (!good(r)) {
            ++r;
        }

        return r;

        /*

        long long sum = 0;
        for (long long i : nums) {
            sum += i;
        }
        sum -= nums[0];

        return max<long long>((sum + n - 2) / (n - 1), nums[0]);

        bool cont = true;
        while (cont) {
            cont = false;
        for (int pos = 1; pos < n; ++pos) {
            if (nums[pos - 1] < nums[pos]) {
                int sum = nums[pos - 1] + nums[pos];
                nums[pos] = sum / 2;
                nums[pos - 1] = sum - nums[pos];
                cont = true;
            }
        }

        }

        vector<pair<int, int>> v(n);
        for (int i = 0; i < n; ++i) {
            v[i] = make_pair(nums[i], i);
        }
        sort(v.begin(), v.end());

        for (auto it = v.rbegin(); it != v.rend(); ++it) {
            int pos = it->second;
            if (pos > 0) {
                if (nums[pos - 1] < nums[pos]) {
                    int sum = nums[pos - 1] + nums[pos];
                    nums[pos] = sum / 2;
                    nums[pos - 1] = sum - nums[pos];
                }
            }
        }

        return *max_element(nums.begin(), nums.end());
        */
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.minimizeArrayValue({3, 7, 1, 6}) << endl;

    return 0;
}
