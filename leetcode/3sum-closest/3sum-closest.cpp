#include "../header.h"

#pragma GCC target ("avx,avx2")
#pragma GCC optimize ("O3")

class Solution {
public:
    int threeSumClosest(vector<int> nums, int target) {
        sort(nums.begin(), nums.end());
        int res = nums[0] + nums[1] + nums[2];
        int opt = abs(res - target);
        int index = 0;
        for (int i = 0; i < nums.size() - 2; ++i) {
            int left = i + 1;
            int right = nums.size() - 1;
            while (left < right) {
                int sum = nums[i] + nums[left] + nums[right];
                if (sum == target) {
                    return sum;
                }
                if (abs(target - sum) < opt) {
                    opt = abs(target - sum);
                    res = sum;
                }
                if (sum > target) {
                    --right;
                } else {
                    ++left;
                }
            }
            /*
            for (int j = i + 1; j < nums.size(); ++j) {
                int ij = nums[i] + nums[j];
                // int index = lower_bound(nums.begin() + j + 1, nums.end(), target - ij) - nums.begin();
                index = max(index, j + 1);
                while (index < nums.size() && nums[index] + ij < target - ij) {
                    ++index;
                }
                int lb = max(j + 1, max(index - 2, 0));
                int rb = min<int>(nums.size(), index + 2);
                for (int k = lb; k < rb; ++k) {
                    if (abs(ij + nums[k] - target) < opt) {
                        res = ij + nums[k];
                        opt = abs(res - target);
                    }
                }
            }
            */
        }
        return res;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.threeSumClosest(
                {-828, 7,    -484,  -510, 579,  -416, 856,  -21,  584,  -751, -250, -414, -979, 756,  424,  593,  964,
                 656,  -718, -152,  -381, 95,   -267, 846,  996,  -923, 588,  -914, 885,  610,  767,  -799, 272,  -9,
                 -376, 616,  816,   -521, 390,  460,  -563, 170,  36,   303,  979,  509,  -323, -239, -274, 490,  -594,
                 297,  172,  546,   692,  633,  444,  326,  844,  -432, -366, -606, 890,  -11,  -43,  -310, 962,  350,
                 737,  783,  976,   713,  834,  163,  -643, 720,  -268, -652, 44,   624,  -86,  -489, -495, -256, 807,
                 406,  950,  -120,  -293, -82,  -217, 281,  772,  113,  -931, 360,  -66,  -976, 411,  354,  -295, 888,
                 19,   -721, 249,   512,  284,  998,  -901, -517, 742,  -874, 731,  -332, -816, 2,    -400, -526, -478,
                 -629, -532, 787,   -962, -610, -164, -240, 953,  -949, -435, -938, 48,   -153, -716, -925, 373,  -601,
                 -547, -312, -398,  -92,  829,  -771, 828,  269,  62,   -840, -379, 137,  -201, -736, -992, 799,  -636,
                 244,  -176, 798,   670,  147,  -626, 482,  -451, 462,  464,  510,  -53,  985,  134,  850,  642,  -972,
                 691,  861,  602,   -134, 302,  899,  966,  -813, 855,  355,  -244, 944,  615,  338,  103,  -428, 22,
                 -124, 400,  -533,  590,  -90,  653,  -251, 652,  619,  -485, -520, 778,  -562, -742, 294,  -84,  -883,
                 -773, 301,  -523,  262,  873,  -83,  78,   570,  -900, 164,  186,  -575, -661, 894,  875,  -73,  30,
                 815,  -991, -877,  -140, 428,  -791, -956, 903,  43,   -156, -189, 68,   -389, -809, 761,  -486, -306,
                 987,  599,  502,   963,  612,  207,  112,  -257, -182, -151, 527,  319,  223,  751,  -165, 848,  709,
                 -105, -777, 794,   -136, -315, -743, 852,  -362, -790, 471,  912,  627,  542,  -469, 311,  -657, 49,
                 762,  -836, 104,   -115, -524, -676, -885, -635, 673,  -145, 149,  415,  675,  607,  -698, 939,  -516,
                 -447, 117,  38,    802,  -168, -631, -767, -278, 695,  -63,  871,  -977, -825, 53,   896,  -817, -679,
                 -644, -77,  679,   -55,  960,  -871, 803,  -163, -821, 489,  210,  46,   -147, 566,  825,  -745, 352,
                 -150, 645,  357,   820,  777,  886,  461,  405,  -197, -554, 965,  -401, 477,  -589, -399, 63,   -56,
                 -668, -924, -838,  226,  -649, -302, 893,  -225, -356, -975, 906,  -74,  -282, 142,  231,  -572, -123,
                 554,  -861, -695,  -45,  -966, 842,  898,  806,  189,  -737, 659,  110,  339,  -639, 188,  422,  760,
                 275,  109,  -740,  225,  -36,  730,  -25,  285,  -754, 483,  492,  -605, -634, -557, 809,  -588, -772,
                 -203, -109, 202,   -677, -434, 921,  -763, -127, 105,  641,  507,  934,  159,  -893, 475,  916,  788,
                 -818, -423, 928,   -623, -939, 395,  -564, -891, -342, -476, -425, -530, -67,  173,  -301, -944, -5,
                 -707, 417,  -647,  563,  -875, -774, 526,  -669, -126, 121,  220,  -550, 889,  307,  317,  957,  278,
                 -144, -917, 221,   535,  618,  -6,   -835, 517,  -833, -958, -518, 412,  -377, 283,  505,  -431, 544,
                 732,  851,  135,   6,    -723, -514, 171,  -17,  335,  508,  666,  -830, 396,  -541, 127,  61,   -397,
                 -410, -353, -26,   47,   73,   -892, -786, -291, 267,  887,  690,  740,  -796, 821,  -12,  -202, -890,
                 -19,  271,  -590,  276,  552,  -179, -682, -247, -850, -842, -898, 969,  504,  -162, 148,  -672, 589,
                 -130, 649,  -324,  -641, 530,  -815, -787, 129,  -467, -628, 391,  704,  318,  575,  812,  -419, -254,
                 681,  304,  -444,  -442, 884,  506,  -439, 746,  -844, -265, -553, 50,   -38,  214,  199,  408,  -680,
                 -229, 814,  -685,  -512, -355, -245, 128,  -395, -184, -735, -326, -446, -422, -29,  -177, 682,  711,
                 716,  93,   -637,  -983, -27,  -31,  -580, -542, 942,  -100, 15,   429,  130,  106,  -985, 16,   -135,
                 368,  707,  -365,  -259, 402,  203,  215,  -198, 715,  -453, 941,  792,  -807, 981,  -213, -646, 586,
                 -935, 385,  -386,  182,  454,  835,  478,  -548, -981, -271, 958,  611,  -316, 89,   -285, 227,  -632,
                 486,  -349, -664,  -178, 67,   -895, 770,  764,  -769, -266, -747, 892,  -937, 118,  348,  545,  458,
                 232,  -993, -233,  -436, -778, -288, -382, -894, -596, 379,  -234, 299,  557,  539,  -405, -658, 351,
                 -770, -608, 550,   -58,  733,  -591, -713, 193,  -768, 845,  863,  -845, -299, 974,  667,  -618, 496,
                 644,  628,  -843,  -870, -8,   235,  -277, 744,  160,  87,   -188, 959,  573,  70,   780,  874,  541,
                 -39,  -998, -1000, -462, -339, 775,  701,  -522, 384,  -34,  -319, 721,  101,  -829, -607, 660,  -230,
                 -569, 643,  -827,  40,   156,  -121, 416,  151,  -858, -622, 515,  -113, -602, 752,  -595, -750, -409,
                 -800, 613,  -139,  827,  819,  -920, 377,  158,  -175, -670, -388, -148, -137, -445, 686,  57,   826,
                 165,  480,  94,    465,  79,   298,  891,  -640, -504, 865,  -23,  -693, 501,  403,  -915, 260,  -368,
                 -822, 436,  -826,  -186, -482, 25,   132,  551,  -357, -690, 41,   467,  -351, -41,  636,  -921, 585,
                 174,  779,  -60,   -47,  -14,  -802, 749,  463,  -988, 441,  185,  727,  346,  -613, -338, -80,  626,
                 432,  811,  -867,  -502, 181,  178,  -918, -638, -973, 187,  -559, -665, 240,  362,  677,  -101, 4,
                 334,  386,  -313,  908,  -300, 369,  -549, 640,  120,  -873, -298, -990, 313,  -933, -574, 972,  192,
                 27,   900,  708,   698,  -62,  929,  371,  388,  356,  370,  946,  -808, 937,  -190, -488, -888, 183,
                 213,  -904, 256,   -708, -129, -483, 909,  65,   31,   907,  525,  -59,  755,  9,    994,  823,  -952,
                 -761, 469,  -854,  -785, 549,  956,  39,   754,  45,   -848, 452,  518,  -656, -411, 324,  56,   533,
                 -688, 138,  -327,  712,  -675, 839,  -600, 86,   289,  -782, 847,  -648, -7,   -872, 190,  -65,  72,
                 -406, 631,  -263,  -863, -452, 905,  -336, 238,  266,  664,  -464, -321, 306,  392,  519,  365,  91,
                 -910, -692, 341,   637,  -849, 650,  -241, -492, 409,  534,  378,  774,  389,  -49,  -617, -907, -582,
                 268,  -970, 993,   -412, -455, 830,  393,  -420, 734,  866,  136,  -964, -886, -551, 34,   239,  420,
                 -819, 495,  -651,  868,  -946, 625,  -158, -666, 98,   538,  662,  236,  -54,  759,  -536, 945,  -264,
                 -108, -951, -118,  -578, -417, -986, 678,  414,  943,  -347, 96,   -329, 561,  933,  -862, 8},
                2791)
         << endl;

    return 0;
}
