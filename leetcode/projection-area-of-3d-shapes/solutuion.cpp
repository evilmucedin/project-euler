#include <vector>
#include <iostream>

using namespace std;

class Solution {
public:
    int projectionArea(const vector<vector<int>>& grid) {
	vector<int> a;
	vector<int> b;
	vector<int> c;
	for (int i = 0; i < grid.size(); ++i)
		for (int j = 0; j < grid[i].size(); ++j) {
			if (j >= a.size())
				a.push_back(grid[i][j]);
			a[j] = max(a[j], grid[i][j]);
			if (i >= b.size())
				b.push_back(grid[i][j]);
			b[i] = max(b[i], grid[i][j]);
		}

	for (int i = 0; i < grid.size(); ++i) {
		c.push_back(grid[i].size());
	}
    	
	int result = 0;
	for (int i : a)
		result += i;
	for (int i : b)
		result += i;
	for (int i : c)
		result += i;
	return result;
    }
};

int main() {
	Solution s;
	const vector<vector<int>> s1(2, vector<int>(1, 2), vector<int>(3, 4));
	cerr << s.projectionArea(s1) << endl;

	return 0;
}
