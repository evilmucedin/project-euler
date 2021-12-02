#include "lib/header.h"

void first() {
    string cmd;
    int d;

    i64 x = 0;
    i64 y = 0;

    cin >> cmd >> d;
    while (!cmd.empty()) {
        if (cmd == "forward") {
            x += d;
        } else if (cmd == "down") {
            y += d;
        } else if (cmd == "up") {
            y -= d;
        }

        cmd.clear();
        cin >> cmd >> d;
    }

    cout << x << endl;
    cout << y << endl;
    cout << x*y << endl;
}

void second() {
    string cmd;
    int d;

    i64 x = 0;
    i64 y = 0;
    i64 aim = 0;

    cin >> cmd >> d;
    while (!cmd.empty()) {
        if (cmd == "forward") {
            x += d;
            y += d*aim;
        } else if (cmd == "down") {
            aim += d;
        } else if (cmd == "up") {
            aim -= d;
        }

        cmd.clear();
        cin >> cmd >> d;
    }

    cout << x << endl;
    cout << y << endl;
    cout << x*y << endl;
}

int main() {
    second();
    return 0;
}
