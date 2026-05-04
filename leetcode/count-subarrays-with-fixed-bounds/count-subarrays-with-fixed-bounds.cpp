#include "../header.h"

class Solution {
public:
    long long countSubarrays_(const vector<int>& nums, int minK, int maxK, int begin, int end) {
        // cerr << "Bounds {" << begin << "," << end << "}" << endl;
        vector<int> minpos;
        vector<int> maxpos;
        for (int i = begin; i < end; ++i) {
            if (nums[i] == minK) {
                minpos.emplace_back(i);
            }
            if (nums[i] == maxK) {
                maxpos.emplace_back(i);
            }
        }
        if (minpos.empty()) {
            return 0;
        }
        if (maxpos.empty()) {
            return 0;
        }
        int toMin = 0;
        int toMax = 0;
        long long result = 0;
        for (int i = begin; i < end; ++i) {
            while (toMin < minpos.size() && minpos[toMin] < i) {
                ++toMin;
            }
            if (toMin == minpos.size()) {
                return result;
            }
            while (toMax < maxpos.size() && maxpos[toMax] < i) {
                ++toMax;
            }
            if (toMax == maxpos.size()) {
                return result;
            }
            result += (end - max(minpos[toMin], maxpos[toMax]));
        }
        return result;
    }

    long long countSubarrays(const vector<int>& nums, int minK, int maxK) {
        if (minK > maxK) {
            return 0;
        }

        long long result = 0;
        int begin = 0;
        bool in = false;
        for (int i = 0; i < nums.size(); ++i) {
            if (nums[i] < minK || nums[i] > maxK) {
                if (in) {
                    result += countSubarrays_(nums, minK, maxK, begin, i);
                    in = false;
                }
            } else {
                if (!in) {
                    in = true;
                    begin = i;
                }
            }
        }
        if (in) {
            result += countSubarrays_(nums, minK, maxK, begin, nums.size());
        }
        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.countSubarrays({1, 3, 5, 2, 7, 5}, 1, 5) << endl;
    cerr << sol.countSubarrays({1, 1, 1, 1}, 1, 1) << endl;
    // 118
    cerr << sol.countSubarrays({934372, 927845, 479424, 49441, 17167, 17167, 65553, 927845, 17167, 927845, 17167,
                                425106, 17167, 927845, 17167, 927845, 251338, 17167},
                               17167, 927845)
         << endl;

    return 0;
}
