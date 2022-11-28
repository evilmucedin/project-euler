#include "../header.h"

  struct ListNode {
      int val;
      ListNode *next;
      ListNode() : val(0), next(nullptr) {}
      ListNode(int x) : val(x), next(nullptr) {}
      ListNode(int x, ListNode *next) : val(x), next(next) {}
  };


class Solution {
public:
    ListNode* removeNodes(ListNode* head) {
        vector<int> values;
        while (head) {
            values.emplace_back(head->val);
            head = head->next;
        }
        vector<int> result;
        for (auto val: values) {
            while (!result.empty() || val > result.back()) {
                result.pop_back();
            }
            result.push_back(val);
        }
        if (result.empty()) {
            return nullptr;
        }
        if (result.size() == 1) {
            return new ListNode(result[0]);
        }
        ListNode* nhead = nullptr;
        ListNode* tail = nullptr;
        for (int val : result) {
            ListNode* nw = new ListNode(val);
            if (nhead) {
                tail->next = nw;
                tail = nw;
            } else {
                nhead = nw;
                tail = nw;
            }
        }
        return head;
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.removeNodes(new ListNode(5, new ListNode(2, new ListNode(13, new ListNode(3, new ListNode(8))))))
         << endl;

    return 0;
}
