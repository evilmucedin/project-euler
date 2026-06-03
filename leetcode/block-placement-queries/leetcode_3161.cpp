#include <iostream>
#include <vector>

using namespace std;

vector<bool> blockPlacementQueries(vector<vector<int>>& queries) {
    vector<bool> results;
    int maxObstacle = 0;
    for (const auto& query : queries) {
        if (query[0] == 1) {
            // Place an obstacle at x
            maxObstacle = max(maxObstacle, query[1]);
        } else {
            // Check if a block of size sz can be placed in the range [0, x]
            int x = query[1];
            int sz = query[2];
            results.push_back(x - maxObstacle >= sz);
        }
    }
    return results;
}

int main() {
    vector<vector<int>> queries = {{1, 2}, {2, 3, 3}, {2, 3, 1}, {2, 2, 2}};
    vector<bool> result = blockPlacementQueries(queries);
    for (bool res : result) {
        cout << (res ? "true" : "false") << ", ";
    }
    return 0;
}