#include "../header.h"

#include "lib/graph/pushRelabel.h"

class Solution {
public:
    int maxStudents(const vector<vector<char>>& seats) {

    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.maxStudents({{'#','.','#','#','.','#'},{'.','#','#','#','#','.'},{'#','.','#','#','.','#'}}) << endl;

    return 0;
}
