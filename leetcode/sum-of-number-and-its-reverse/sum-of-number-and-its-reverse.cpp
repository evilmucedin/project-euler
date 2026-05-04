#include "../header.h"

class Solution {
public:
    unordered_set<int> good_;
    vector<int> rev_;

    Solution() {
        for (int i = 0; i < 123456; ++i) {
            good_.emplace(i + digReverse(i));
        }
        cerr << "{";
        for (auto x: good_) {
            cerr << x << ", ";
        }
        cerr << "}" << endl;
    }


    int digReverse(int n) {
        if (n == 0) {
            return 0;
        }
        rev_.clear();
        while (n) {
            rev_.emplace_back(n % 10);
            n /= 10;
        }
        int result = 0;
        for (auto it = rev_.begin(); it != rev_.end(); ++it) {
            result = 10*result + *it;
        }
        return result;
    }

    bool sumOfNumberAndReverse(int num) {
        return good_.count(num);
    }
};

int main() {
    Timer t("Subarray timer");
    Solution sol;
    cerr << sol.sumOfNumberAndReverse(63) << endl;

    return 0;
}
