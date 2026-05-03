#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

class DSU {
public:
    vector<int> parent;
    int components;
    DSU(int n) : parent(n), components(n) {
        iota(parent.begin(), parent.end(), 0);
    }
    int find(int i) {
        if (parent[i] == i) return i;
        return parent[i] = find(parent[i]);
    }
    bool unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);
        if (root_i != root_j) {
            parent[root_i] = root_j;
            components--;
            return true;
        }
        return false;
    }
};

class Solution {
public:
    int maxStability(int n, vector<vector<int>>& edges, int k) {
        auto check = [&](int mid) {
            DSU dsu(n);
            int upgrades_used = 0;

            // 1. Process mandatory edges
            for (const auto& e : edges) {
                if (e[3] == 1) {
                    if (e[2] < mid || !dsu.unite(e[0], e[1])) return false;
                }
            }

            // 2. Process optional edges that don't need upgrades
            vector<vector<int>> needs_upgrade;
            for (const auto& e : edges) {
                if (e[3] == 0) {
                    if (e[2] >= mid) {
                        dsu.unite(e[0], e[1]);
                    } else if (e[2] * 2 >= mid) {
                        needs_upgrade.push_back(e);
                    }
                }
            }

            // 3. Process optional edges that need upgrades to reach 'mid'
            for (const auto& e : needs_upgrade) {
                if (upgrades_used < k && dsu.unite(e[0], e[1])) {
                    upgrades_used++;
                }
            }

            return dsu.components == 1;
        };

        // Pre-check: Mandatory edges must not form a cycle
        DSU pre_dsu(n);
        for (const auto& e : edges) {
            if (e[3] == 1 && !pre_dsu.unite(e[0], e[1])) return -1;
        }

        int low = 0, high = 2e9, ans = -1;
        while (low <= high) {
            int mid = low + (high - low) / 2;
            if (check(mid)) {
                ans = mid;
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }
        return ans;
    }
};

