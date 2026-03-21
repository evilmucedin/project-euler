#include <iostream>

using namespace std;

class Solution {
    public:
        void setZeroes(vector<vector<int>>& matrix) {
            const int n = matrix.size();
            vector<int> x(n);
            vector<int> y(n);
            for (int i = 0; i < n; ++i)
                for (int j = 0; j < n; ++j)
                    if (matrix[i][j] == 0) {
                        x[i] = 1;
                        y[j] = 1;
                    }
            for (int i = 0; i < n; ++i)
                for (int j = 0; j < n; ++j)
                    if (x[i] == 1 || y[j] == 1)
                        matrix[i][j] = 0;
        }
};

int main() {
    Solution s;
    // cout << s.setZeroes() << endl;
    return 0;
}
