#include <string>

using namespace std;

/**
 * // This is the interface that allows for creating nested lists.
 * // You should not implement it, or speculate about its implementation
 * class NestedInteger {
 *   public:
 *     // Constructor initializes an empty nested list.
 *     NestedInteger();
 *
 *     // Constructor initializes a single integer.
 *     NestedInteger(int value);
 *
 *     // Return true if this NestedInteger holds a single integer, rather than a nested list.
 *     bool isInteger() const;
 *
 *     // Return the single integer that this NestedInteger holds, if it holds a single integer
 *     // The result is undefined if this NestedInteger holds a nested list
 *     int getInteger() const;
 *
 *     // Set this NestedInteger to hold a single integer.
 *     void setInteger(int value);
 *
 *     // Set this NestedInteger to hold a nested list and adds a nested integer to it.
 *     void add(const NestedInteger &ni);
 *
 *     // Return the nested list that this NestedInteger holds, if it holds a nested list
 *     // The result is undefined if this NestedInteger holds a single integer
 *     const vector<NestedInteger> &getList() const;
 * };
 */
class Solution {
public:
    NestedInteger deserialize(string s) {
        int i = 0;
        return parse(s, i);
    }

private:
    NestedInteger parse(const string& s, int& i) {
        if (s[i] == '[') {
            // It's a list
            NestedInteger ni;
            i++; // skip '['
            
            while (i < s.size() && s[i] != ']') {
                ni.add(parse(s, i));
                if (i < s.size() && s[i] == ',') {
                    i++; // skip ','
                }
            }
            if (i < s.size()) i++; // skip ']'
            return ni;
        } else {
            // It's an integer
            int start = i;
            if (s[i] == '-') {
                i++;
            }
            while (i < s.size() && isdigit(s[i])) {
                i++;
            }
            int num = stoi(s.substr(start, i - start));
            return NestedInteger(num);
        }
    }
};
