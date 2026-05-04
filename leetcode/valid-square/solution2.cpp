#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

class Solution {
public:
    bool validSquare(vector<int>& p1, vector<int>& p2, vector<int>& p3, vector<int>& p4) {
        // Store squared distances to avoid precision issues with sqrt
        vector<long> dists = {
            distSq(p1, p2), distSq(p1, p3), distSq(p1, p4),
            distSq(p2, p3), distSq(p2, p4), distSq(p3, p4)
        };

        sort(dists.begin(), dists.end());

        // A square must have:
        // 1. Four equal sides (dists[0] to dists[3])
        // 2. Two equal diagonals (dists[4] to dists[5])
        // 3. Side length > 0
        return dists[0] > 0 &&
               dists[0] == dists[1] &&
               dists[1] == dists[2] &&
               dists[2] == dists[3] &&
               dists[4] == dists[5] &&
               dists[3] < dists[4]; // Diagonals must be longer than sides
    }

private:
    long distSq(vector<int>& a, vector<int>& b) {
        return (long)(a[0] - b[0]) * (a[0] - b[0]) +
               (long)(a[1] - b[1]) * (a[1] - b[1]);
    }
};

