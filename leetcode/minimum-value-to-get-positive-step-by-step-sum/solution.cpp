#include <vector>
#include <iostream>

using namespace std;

class Solution {
public:
    int minStartValue(const vector<int>& nums) {
        int result = 0;
        int sum = 0;
        for (int i: nums) {
            sum += i;
            int x = 1 - sum;
            if (x > result)
                result = x;
        }
        return result;
    }
};

int main() {
    Solution s;
    cout << s.minStartValue({-3, 2, -3, 4, 2}) << endl;
    return 0;
}
