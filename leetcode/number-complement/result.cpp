#include <iostream>
#include <algorithm>

using namespace std;

class Solution {
public:
    int findComplement(int num) {
        int res = 0;
        int x = 1;
        while (num) {
            if (num % 2 == 0)
                res += x;
            
            if (x == 1<<31) {
	    	break;
	    }
	    x *= 2;
            num /= 2;
        }
        return res;    
    }

    int findComplement2(int num) {
        if (num == 0) {
            return 1; // Special case: complement of 0 is 1
        }

        int mask = 0;
        int temp_num = num;

        // Create a mask with all 1s up to the most significant bit of num
        while (temp_num > 0) {
            mask = (mask << 1) | 1; // Shift mask left and set LSB to 1
            temp_num >>= 1;         // Right shift temp_num
        }

        // XOR num with the mask to flip the relevant bits
        return num ^ mask;
    }

};

std::string intToBinaryString(int n) {
    if (n == 0) return "0";
    std::string binary_str = "";
    while (n > 0) {
        binary_str += (n % 2 == 0 ? '0' : '1');
        n /= 2;
    }
    std::reverse(binary_str.begin(), binary_str.end());
    return binary_str;
}

int main() {
	Solution s;
	cout << 1 << " " << s.findComplement(1) << endl;
	cout << 5 << " " << s.findComplement(5) << endl;
	cout << 2147483647 << " " << s.findComplement(2147483647) << " " << intToBinaryString(2147483647) << " " << s.findComplement2(2147483647) << endl;
	return 0;
}
