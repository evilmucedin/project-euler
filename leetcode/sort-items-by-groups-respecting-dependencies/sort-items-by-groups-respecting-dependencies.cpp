#include "../header.h"

class Solution {
   public:
    vector<int> sortItems(int n, int m, vector<int>& group, vector<vector<int>>& beforeItems) {
        vector<vector<int>> groupGraph(m + n);
        vector<vector<int>> itemGraph(n);
        vector<int> groupInDegree(m + n, 0);
        vector<int> itemInDegree(n, 0);

        for (int i = 0; i < n; ++i) {
            if (group[i] != -1) {
                groupGraph[m + i].push_back(group[i]);
                groupInDegree[group[i]]++;
            }

            for (int before : beforeItems[i]) {
                itemGraph[before].push_back(i);
                itemInDegree[i]++;
                if (group[i] != -1 && group[i] == group[before]) {
                    groupGraph[before].push_back(i + m);
                    groupInDegree[i + m]++;
                }
            }
        }

        vector<int> groupOrder;
        queue<int> groupQueue;

        for (int i = 0; i < m + n; ++i) {
            if (groupInDegree[i] == 0) {
                groupQueue.push(i);
            }
        }

        while (!groupQueue.empty()) {
            int currentGroup = groupQueue.front();
            groupQueue.pop();
            groupOrder.push_back(currentGroup);

            for (int nextGroup : groupGraph[currentGroup]) {
                if (--groupInDegree[nextGroup] == 0) {
                    groupQueue.push(nextGroup);
                }
            }
        }

        if (groupOrder.size() != m + n) {
            return {};  // Not all groups/items could be sorted
        }

        vector<int> result;

        for (int currentGroup : groupOrder) {
            if (currentGroup < m) {
                queue<int> itemQueue;

                for (int i = 0; i < n; ++i) {
                    if (group[i] == currentGroup && itemInDegree[i] == 0) {
                        itemQueue.push(i);
                    }
                }

                while (!itemQueue.empty()) {
                    int currentItem = itemQueue.front();
                    itemQueue.pop();
                    result.push_back(currentItem);

                    for (int nextItem : itemGraph[currentItem]) {
                        if (--itemInDegree[nextItem] == 0) {
                            itemQueue.push(nextItem);
                        }
                    }
                }
            }
        }

        if (result.size() != n) {
            return {};  // Not all items could be sorted within groups
        }

        return result;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.<< endl;

    return 0;
}
