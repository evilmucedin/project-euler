#include <vector>
#include <string>
#include <iostream>

using namespace std;

class Solution {
public:
    int uniqueMorseRepresentations(vector<string>& words) {
        static const vector<string> MORSE{".-","-...","-.-.","-..",".","..-.","--.","....","..",".---","-.-",".-..","--","-.","---",".--.","--.-",".-.","...","-","..-","...-",".--","-..-","-.--","--.."};
        cout << MORSE.size() << endl;

        int result = 0;
        return result;
    }
};

int main() {
    Solution s;
    vector<string> v1{"gin","zen","gig","msg"};
    cout << s.uniqueMorseRepresentations(v1);
    return 0;
}
