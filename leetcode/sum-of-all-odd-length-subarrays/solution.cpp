#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
    int sumOddLengthSubarrays(const vector<int>& arr) {
        int result = 0;
        const int n = arr.size();
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j <= n; j += 2) {
                int sum = 0;
                for (int k = i; k < j; ++k) {
                    sum += arr[k];
                }
                // cerr << sum << endl;
                result += sum;
            }
        }
        return result;
    }
};

int main() {
    Solution s;
    cerr << s.sumOddLengthSubarrays({1,4,2,5,3}) << endl;
    return 0;
}
