#include "advent/lib/aoc.h"
#include "gflags/gflags.h"
#include "lib/init.h"
#include "lib/string.h"

DEFINE_int32(test, 1, "test number");

void first() {
    const auto input = readInputLines();

    size_t sum = 0;
    for (const auto& s : input) {
        vector<char> stack;

#define CHECK(p, score)                       \
    if (stack.empty() || stack.back() != p) { \
        sum += score;                         \
        break;                                \
    } else {                                  \
        stack.pop_back();                     \
    }

        for (auto ch : s) {
            if (ch == '(' || ch == '{' || ch == '[' || ch == '<') {
                stack.emplace_back(ch);
            } else if (ch == ')') {
                CHECK('(', 3);
            } else if (ch == ']') {
                CHECK('[', 57);
            } else if (ch == '}') {
                CHECK('{', 1197);
            } else if (ch == '>') {
                CHECK('<', 25137);
            }
        }
    }

    cout << sum << endl;
}

void second() {
    const auto input = readInputLines();

    vector<size_t> scores;
    for (const auto& s : input) {
        vector<char> stack;

#undef CHECK
#define CHECK(p)                              \
    if (stack.empty() || stack.back() != p) { \
        bad = true;                           \
        break;                                \
    } else {                                  \
        stack.pop_back();                     \
    }

        bool bad = false;
        for (auto ch : s) {
            if (ch == '(' || ch == '{' || ch == '[' || ch == '<') {
                stack.emplace_back(ch);
            } else if (ch == ')') {
                CHECK('(');
            } else if (ch == ']') {
                CHECK('[');
            } else if (ch == '}') {
                CHECK('{');
            } else if (ch == '>') {
                CHECK('<');
            }
        }

        if (!bad) {
            reverse(stack);
            size_t delta = 0;
            for (char ch : stack) {
                size_t d = 0;
                if (ch == '(') {
                    d = 1;
                } else if (ch == '[') {
                    d = 2;
                } else if (ch == '{') {
                    d = 3;
                } else if (ch == '<') {
                    d = 4;
                }
                delta = 5 * delta + d;
            }
            scores.emplace_back(delta);
        }
    }

    sort(scores);
    cout << scores[scores.size() / 2] << endl;
}

int main(int argc, char* argv[]) {
    standardInit(argc, argv);

    if (FLAGS_test == 1) {
        first();
    } else if (FLAGS_test == 2) {
        second();
    }
    return 0;
}
