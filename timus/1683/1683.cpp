#include <iostream>
#include <vector>

using namespace std;

using i64 = long long int;

int main() {
    i64 n;
    cin >> n;
    vector<i64> result;
    while (n != 1) {
        n -= n / 2;
        result.emplace_back(n);
    }
    cout << result.size() << endl;
    for (int x: result) {
        cout << x << " ";
    }
    cout << endl;
    return 0;
}
