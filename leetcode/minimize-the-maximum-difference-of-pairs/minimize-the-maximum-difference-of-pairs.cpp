#include "../header.h"

#include <climits>

class Solution {
public:
int countPairs(vector<int>& nums, int mid) {
    int count = 0;
    int n = nums.size();
    int j = n - 1;
    for (int i = 0; i < n; i++) {
        while (j > i && nums[j] - nums[i] > mid) {
            j--;
        }
                    while (j + 1 < n && nums[j + 1] - nums[i] <= mid) {
                j++;
            }

        count += max(0, j - i);
    }
    return count;
}

    int minimizeMax4(vector<int>& nums, int p) {
        sort(nums.begin(), nums.end());
        int n = nums.size();
        int left = 0;
        int right = nums.back() - nums.front();
        int result = INT_MAX;
        /*
        if (p >= n / 2) {
            for (int i = 0; i < n; i++) {
                for (int j = i + 1; j < n; j++) {
                    result = min(result, nums[j] - nums[i]);
                }
            }
            return result;
        }
        */
        while (left <= right) {
            int mid = left + (right - left) / 2;
            int count = countPairs(nums, mid);
            if (count >= p) {
                result = min(result, mid);
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }
            while (result > 0) {
                int count = countPairs(nums, result - 1);
                if (count < p) {
                    break;
                } else {
                    --result;
                }
            }
        return result;
    }

    using PII = pair<int, int>;

    vector<PII> minCount(vector<PII>& res, unordered_set<int>& v, const vector<PII>& ps, int count, int index) {
        if (index >= ps.size()) {
            return {};
        }
        if (count <= 0) {
            return res;
        }

        auto sub2 = minCount(res, v, ps, count, index + 1);

        const auto& p = ps[index];
        if (v.count(p.first) == 0 && v.count(p.second) == 0) {
            res.emplace_back(p);
            v.emplace(p.first);
            v.emplace(p.second);
            auto sub = minCount(res, v, ps, count - 1, index + 1);
            v.erase(p.first);
            v.erase(p.second);
            res.pop_back();

            if (sub.empty() || sub.size() > sub2.size()) {
                return sub2;
            } else {
                return sub;
            }
        }
        return sub2;
    }

    int minimizeMax(vector<int>& nums, int p) {
        if (p == 0) {
            return 0;
        }

        const int n = nums.size();
        vector<PII> ps;
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                ps.emplace_back(make_pair(i, j));
            }
        }

        auto d = [&](const PII& a) {
            return abs(nums[a.first] - nums[a.second]);
        };

        sort(ps.begin(), ps.end(), [&](const PII& a, const PII& b) {return d(a) < d(b);});


        int count = 0;
        vector<PII> res1, res2;
        unordered_set<int> v;
        res2 = minCount(res1, v, ps, p, 0);

        /*
        int i = 0;
        while (count < p) {
            const auto& p = ps[i];
            if (v.count(p.first) == 0 && v.count(p.second) == 0) {
                ++count;
                res.emplace_back(p);
                v.emplace(p.first);
                v.emplace(p.second);
            }
            ++i;
        }
        */

        return d(res2.back());
    }


    int minimizeMax2(vector<int>& nums, int p) {
        const int n = nums.size();
        sort(nums.begin(), nums.end());
        int ans = INT_MAX;

        int np = 1;
        if (p > 1) {
            while (np*(np - 1)/2 < p) {
                ++np;
            }
        }

        for (int i = 0; i + 2*np - 1 < n; i++) {
            int diff = 0;
            for (int j = i + 1; j < i + 2*np; ++j) {
                diff = max(diff, nums[j] - nums[j - 1]);
            }
            ans = min(ans, diff);
        }
        return ans;
    }
};


int main() {
    Solution sol;
    Timer t("Subarray timer");
    vector<int> nums{4, 2, 1, 2};
    cerr << sol.minimizeMax(nums, 1) << endl;
    vector<int> nums2{3, 4, 1, 2, 1};
    cerr << sol.minimizeMax(nums2, 2) << endl;
    vector<int> nums3{3, 4, 2, 3, 2, 1, 2};
    cerr << sol.minimizeMax(nums3, 3) << endl;
    vector<int> nums4{1, 2};
    cerr << sol.minimizeMax(nums4, 1) << endl;

    return 0;
}
