#include "../header.h"

struct ListNode {
    int val;
    ListNode* next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode* next) : val(x), next(next) {}
};

class Solution {
   public:
    size_t len(ListNode* head) {
        size_t result = 0;
        while (head) {
            ++result;
            head = head->next;
        }
        return result;
    }

    ListNode* removeNthFromEnd(ListNode* head, int n) {
        n = len(head) - n;

        if (n == 0) {
            return head->next;
        }

        ListNode* prev = nullptr;
        ListNode* now = head;
        for (int i = 0; i < n; ++i) {
            prev = now;
            if (now) {
                now = now->next;
            }
        }
        if (prev) {
            if (now) {
                prev->next = now->next;
            } else {
                prev->next = nullptr;
            }
        }
        return head;
    }
};

ListNode* createList(const vector<int>& v) {
    ListNode* head = nullptr;
    ListNode* prev = nullptr;
    for (auto i : v) {
        ListNode* next = new ListNode(i, prev);
        next->next = nullptr;
        if (!head) {
            head = next;
        } else {
            prev->next = next;
        }
        prev = next;
    }
    return head;
}

void print(ListNode* n) {
    while (n) {
        cerr << n->val << " ";
        n = n->next;
    }
    cerr << endl;
}

int main() {
    Solution sol;
    Timer t("Subarray timer");
    print(sol.removeNthFromEnd(createList({1, 2, 3, 4, 5}), 2));

    return 0;
}
