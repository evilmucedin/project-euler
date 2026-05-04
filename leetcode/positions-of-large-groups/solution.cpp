
#include <vector>
#include <string>

using namespace std;

class Solution {
public:
    vector<vector<int>> largeGroupPositions(string s) {
	vector<vector<int>> result;
	const int n = s.size();
	int a = 0;
	for (int i = 1; i <= n; ++i) {
		if (i == n || s[i] != s[a]) {
            		if (i > a + 2) {
				vector<int> v;
				v.push_back(a);
				v.push_back(i - 1);
				result.push_back(v);
			}
			a = i;
		}
	}
		
	return result;	
    }
};

int main() {
	Solution s;
	return 0;
}
