#include "../header.h"

#include <lib/string.h>

class Solution {
public:
    string simplifyPath(string path) {
        const auto parts = removeEmpty(split(path, '/'));
        StringVector s;
        for (const auto& p : parts) {
            if (p == "..") {
                if (!s.empty()) {
                    s.pop_back();
                }
            } else if (p == ".") {
            } else {
                s.push_back(p);
            }
        }
        return "/" + join("/", s);
    }
};

int main() {
    Solution sol;
    Timer t("Subarray timer");
    cerr << sol.simplifyPath("/home/") << endl;
    cerr << sol.simplifyPath("/../") << endl;
    cerr << sol.simplifyPath("/home//foo/") << endl;
    cerr << sol.simplifyPath("/a/./b/../../c/") << endl;

    return 0;
}
