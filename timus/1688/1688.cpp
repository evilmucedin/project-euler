#include <iostream>

using namespace std;

using i64 = long long int;

int main() {
    i64 balance;
    i64 n;
    cin >> balance >> n;
    balance *= 3;

    i64 i = 0;
    while ((balance >= 0) && (i < n)) {
        i64 bill;
        cin >> bill;
        ++i;
        balance -= bill;
    }

    if (balance < 0) {
        cout << "Free after " << i << " times." << endl;
    } else {
        cout << "Team.GOV!" << endl;
    }

    return 0;
}
