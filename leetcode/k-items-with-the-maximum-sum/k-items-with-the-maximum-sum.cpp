#include "../header.h"

class Solution {
public:
    int kItemsWithMaximumSum(int numOnes, int numZeros, int numNegOnes, int k) {
        int result = 0;
        int a = min(k, numOnes);
        result += a;
        k -= a;
        if (k) {
            int b = min(k, numZeros);
            k -= b;
            if (k) {
                int c = min(k, numNegOnes);
                k -= c;
                result -= c;
            }
        }
        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.kItemsWithMaximumSum(3, 2, 0, 2) << endl;
    cerr << sol.kItemsWithMaximumSum(3, 2, 0, 4) << endl;

    return 0;
}
