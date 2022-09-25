#include "../header.h"

#include <queue>

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */

struct ListNode {
    int val;
    ListNode* next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode* next) : val(x), next(next) {}
};

class Cmp {
public:
 bool operator()(const ListNode* a, const ListNode* b) { return a->val > b->val; }
};

using Heap = priority_queue<ListNode*, vector<ListNode*>, Cmp>;

class Solution {
public:
    ListNode* mergeKLists(vector<ListNode*>& lists) {
        Heap h;
        for (auto item: lists) {
            if (item) {
                h.emplace(item);
            }
        }
        ListNode* head = nullptr;
        ListNode* prev = nullptr;
        while (!h.empty()) {
            auto next = h.top();
            h.pop();
            if (!head) {
                head = next;
            }
            if (prev) {
                prev->next = next;
            }
            prev = next;
            next = next->next;
            if (next) {
                h.emplace(next);
            }
        }

        return head;
    }
};

int main() {
    Timer t("Subarray timer");

    Solution sol;
    ListNode n1{5, nullptr};
    ListNode n2{1, nullptr};
    vector<ListNode*> v = {&n1, &n2};

    auto res = sol.mergeKLists(v);
    cerr << res->val << " " << res->next << endl;
    res = res->next;
    cerr << res->val << " " << res->next << endl;

    return 0;
}
