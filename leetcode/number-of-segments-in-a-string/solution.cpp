class Solution {
public:
    int countSegments(string s) {
        bool in = false;
        int count = 0;
        for (const char c : s) {
            if ( ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '\'') || (c == '-')) && (C != ',') ) {
                if (!in) {
                    in = true;
                    ++count;
                }
            } else {
                if (in) {
                    in = false;
                }
            }
        }
        return count;    
    }
};
