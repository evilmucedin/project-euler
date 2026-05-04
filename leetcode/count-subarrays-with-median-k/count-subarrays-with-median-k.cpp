#include "../header.h"

class Solution {
public:
    int countSubarrays(vector<int> nums, int k) {
        for (auto& i: nums) {
            --i;
        }
        --k;
        const int n = nums.size();
        vector<int> pos(n);
        for (int i = 0; i < n; ++i) {
            pos[nums[i]] = i;
        }

        int pk = pos[k];
        int balance = 0;
        unordered_map<int, int> balCounts;
        for (int i = pk; i >= 0; --i) {
            if (nums[i] > k) {
                ++balance;
            } else if (nums[i] < k) {
                --balance;
            }
            ++balCounts[balance];
        }

        int result = 0;
        balance = 0;
        for (int i = pk; i < n; ++i) {
            if (nums[i] > k) {
                ++balance;
            } else if (nums[i] < k) {
                --balance;
            }
            result += balCounts[-balance] + balCounts[-balance + 1];
        }

        return result;

        /*
        vector<int> countLess(n);
        for (int i = 0; i < n; ++i) {
            countLess[i] = nums[i] < k;
            if (i) {
                countLess[i] += countLess[i - 1];
            }
        }
        */
        /*
        for (int i = 0; i < n; ++i) {
            countLess[i] = k - countLess[i] + 1;
        }
        */
        /*
        vector<int> countMore(n);
        for (int i = 0; i < n; ++i) {
            countMore[i] = nums[i] > k;
            if (i != 0) {
                countMore[i] += countMore[i - 1];
            }
        }
        // cerr << countLess << endl;
        // cerr << countMore << endl;
        vector<int> diff(n);
        for (int i = 0; i < n; ++i) {
            diff[i] = countLess[i] - countMore[i];
        }

        long long int result = 0;
        int kpos = pos[k];
        unordered_map<int, int> first0;
        unordered_map<int, int> first1;
        for (int i = 0; i < kpos; ++i) {
            if (i & 1) {
                ++first1[diff[i]];
            } else {
                ++first0[diff[i]];
            }
        }
        unordered_map<int, int> second0;
        unordered_map<int, int> second1;
        for (int i = kpos; i < n; ++i) {
            if (i & 1) {
                ++second1[diff[i]];
            } else {
                ++second0[diff[i]];
            }
        }
        cerr << diff << endl;
        // cerr << first0 << " : " << first1 << endl;
        // cerr << second0 << " : " << second1 << endl;

        for (auto& p: second0) {
            result += p.second*first0[p.first - 1];
            result += p.second*first1[p.first];
        }
        for (auto& p: second1) {
            result += p.second*first0[p.first];
            result += p.second*first1[p.first - 1];
        }

        return result;
        */
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.countSubarrays({3, 2, 1, 4, 5}, 4) << endl;
    cerr << sol.countSubarrays({2, 3, 1}, 3) << endl;
    cerr << sol.countSubarrays({2, 5, 1, 4, 3, 6}, 1) << endl;
    cerr << sol.countSubarrays({4, 1, 3, 2}, 1) << endl;

    return 0;
}
