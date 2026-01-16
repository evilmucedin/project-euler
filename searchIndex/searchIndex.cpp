#include <string>
#include <iostream>

using namespace std;

int main() {
    for (int file = 1; file < 10; ++file) {
        string filename = to_string(file) + ".txt";
        cerr << filename << endl;
    }
    return 0;
}
