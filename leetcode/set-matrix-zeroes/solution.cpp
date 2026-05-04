#include <iostream>
#include <vector>

using namespace std;

class Solution {
    public:
        void setZeroes(vector<vector<int>>& matrix) {
            const int n = matrix.size();
            const int m = matrix[0].size();
            vector<int> x(n);
            vector<int> y(m);
            for (int i = 0; i < n; ++i)
                for (int j = 0; j < m; ++j)
                    if (matrix[i][j] == 0) {
                        x[i] = 1;
                        y[j] = 1;
                    }
            for (int i = 0; i < n; ++i)
                for (int j = 0; j < m; ++j)
                    if (x[i] == 1 || y[j] == 1)
                        matrix[i][j] = 0;
        }
};

int main() {
    Solution s;
    vector<vector<int>> m = {{0,1,2,0},{3,4,5,2},{1,3,1,5}};
    // cout << s.setZeroes() << endl;
    return 0;
}
