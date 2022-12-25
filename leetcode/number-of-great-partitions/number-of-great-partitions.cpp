#include "../header.h"

const long long MOD = 1e9 + 7;

class Solution {
public:
    using Key = tuple<int, long long, long long>;
    map<Key, long long> dp;

    long long powMod(long long a, long long p, long long mod) {
        long long result = 1;
        while (p) {
            if (p & 1) {
                result = (result * a) % mod;
                --p;
            } else {
                a = (a * a) % mod;
                p /= 2;
            }
        }
        return result;
    }

    long long rec_(const vector<int>& nums, const vector<long long>& psums, int now, long long left1, long long left2, int n) {
        if (now >= n) {
            if (left1 <= 0 && left2 <= 0) {
                return 1;
            } else {
                return 0;
            }
        }
        if (left1 <= 0 && left2 <= 0) {
            return powMod(2, n - now, MOD);
        }
        if (left1 > 0) {
            if (psums[now] < left1) {
                return 0;
            }
        }
        if (left2 > 0) {
            if (psums[now] < left2) {
                return 0;
            }
        }
        return (rec(nums, psums, now + 1, left1 - nums[now], left2, n) +
                rec(nums, psums, now + 1, left1, left2 - nums[now], n)) %
               MOD;
    }

    long long rec(const vector<int>& nums, const vector<long long>& psums, int now, long long left1, long long left2, int n) {
        Key k(now, left1, left2);
        auto toK = dp.find(k);
        if (toK != dp.end()) {
            return toK->second;
        }
        auto res = rec_(nums, psums, now, left1, left2, n);
        dp.emplace(k, res);
        return res;
    }


    int countPartitions(const vector<int>& nums0, long long k) {
        auto nums = nums0;
        sort(nums.begin(), nums.end());
        reverse(nums.begin(), nums.end());
        vector<long long> psums;
        long long sum = 0;
        for (auto x: nums) {
            sum += x;
            psums.emplace_back(sum);
        }
        if (sum <= 2*k) {
            return 0;
        }
        reverse(psums.begin(), psums.end());
        const int n = nums.size();
        dp.clear();
        return rec(nums, psums, 0, k, k, n);
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.countPartitions({73, 16, 86, 25, 98, 92, 15, 11, 87, 88, 88, 94, 83, 74, 1, 48, 91, 9, 45}, 61) << endl;
    cerr << sol.countPartitions(
        {367, 160, 643, 105, 915, 595, 517, 327, 57,  638, 609, 181, 595, 341, 901, 365, 990, 55,  496, 232, 840,
         825, 19,  482, 910, 109, 926, 169, 1,   325, 252, 593, 11,  583, 239, 840, 928, 423, 620, 829, 706, 313,
         445, 135, 981, 285, 780, 5,   678, 117, 376, 494, 383, 681, 244, 989, 780, 493, 697, 203, 810, 833, 138,
         555, 411, 109, 832, 504, 325, 758, 404, 323, 709, 831, 311, 870, 360, 322, 866, 10,  959, 543, 54,  715,
         638, 804, 263, 663, 441, 702, 473, 309, 279, 143, 924, 298, 712, 80,  122, 863, 880, 289, 682, 517, 471,
         4,   985, 743, 501, 157, 381, 22,  914, 293, 332, 8,   604, 410, 519, 889, 162, 722, 676, 601, 910, 54,
         536, 220, 89,  501, 411, 754, 409, 527, 714, 395, 915, 338, 442, 822, 70,  839, 15,  451, 937, 47,  88,
         988, 608, 357, 385, 952, 766, 917, 543, 810, 217, 47,  717, 306, 201, 91,  93,  135, 418, 56,  519, 269,
         855, 755, 98,  735, 414, 742, 552, 885, 183, 694, 463, 292, 318, 239, 459, 763, 698, 447, 506, 657, 108,
         185, 666, 697, 849, 782, 461, 285, 950, 704, 951, 846, 86,  600, 429, 111, 520, 14,  576, 591, 602, 678,
         961, 2,   766, 710, 643, 385, 465, 66,  748, 528, 758, 1,   54,  969, 635, 588, 625, 44,  927, 921, 9,
         50,  632, 77,  735, 977, 521, 64,  282, 362, 996, 561, 125, 319, 556, 209, 855, 280, 613, 432, 93,  126,
         427, 780, 969, 933, 809, 637, 294, 225, 367, 351, 290, 406, 35,  460, 153, 625, 695, 77,  389, 279, 648,
         804, 65,  624, 405, 428, 49,  851, 515, 449, 340, 43,  708, 405, 201, 283, 779, 201, 306, 707, 372, 15,
         971, 566, 828, 819, 537, 747, 730, 764, 141, 248, 768, 195, 986, 834, 775, 719, 969, 23,  866, 724, 780,
         24,  744, 727, 53,  414, 100, 246, 506, 364, 315, 142, 516, 66,  947, 669, 272, 753, 621, 169, 73,  169,
         930, 345, 767, 555, 82,  806, 312, 552, 57,  432, 759, 942, 339, 385, 242, 297, 120, 785, 443, 843, 642,
         224, 873, 556, 278, 855, 273, 803, 597, 288, 478, 300, 894, 462, 354, 901, 366, 489, 664, 551, 758, 727,
         528, 276, 228, 890, 602, 768, 752, 577, 586, 92,  97,  956, 688, 438, 501, 372, 20,  352, 308, 923, 545},
        787) << endl;

    return 0;
}
