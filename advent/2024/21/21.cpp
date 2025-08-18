#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

<<<<<<< HEAD
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>

using namespace std;

// Keypad layout
const vector<string> numericKeypad = {
    "789",
    "456",
    "123",
    " 0A"
};

const vector<string> directionalKeypad = {
    " ^A",
    "<v>"
};

// Directions for movement
const vector<pair<int, int>> directions = {
    {0, 1}, {1, 0}, {0, -1}, {-1, 0}  // Down, Right, Up, Left
};

// Function to get the target button position on the numeric keypad
pair<int, int> findButtonPosition(const vector<string>& keypad, char button) {
    for (int row = 0; row < keypad.size(); ++row) {
        for (int col = 0; col < keypad[row].size(); ++col) {
            if (keypad[row][col] == button) {
                return {row, col};
            }
        }
    }
    return {-1, -1};  // Button not found
}

// Function to find the shortest path to press a button on the numeric keypad
int findShortestPathNumeric(const string& code) {
    auto [targetRow, targetCol] = findButtonPosition(numericKeypad, 'A');
    queue<tuple<int, int, string>> q;
    q.push({3, 2, ""});
    unordered_map<string, int> visited;
    visited["3,2"] = 0;

    while (!q.empty()) {
        auto [row, col, path] = q.front();
        q.pop();

        if (row == targetRow && col == targetCol) {
            if (path.back() != 'A') {
                path.push_back('A');
            }
            return path.length();
        }

        for (auto [dr, dc] : directions) {
            int newRow = row + dr;
            int newCol = col + dc;

            if (newRow >= 0 && newRow < numericKeypad.size() &&
                newCol >= 0 && newCol < numericKeypad[newRow].size() &&
                numericKeypad[newRow][newCol] != ' ') {
                string newPos = to_string(newRow) + "," + to_string(newCol);

                if (visited.find(newPos) == visited.end() || visited[newPos] > path.length() + 1) {
                    visited[newPos] = path.length() + 1;
                    q.push({newRow, newCol, path + "A"});
=======

#include <cassert>
#include <iostream>
#include <vector>
#include <random>
#include <set>
#include <algorithm>
#include <string>
#include <map>
#include <sstream>
#include <regex>
#include <deque>
#include <numeric>
#include <fstream>
#include <queue>

int64_t cheapestRobot(const std::string& presses, int nrobots);

uint64_t hash(int curr, int curc, int destr, int destc, int nrobots)
{
    uint64_t result = curr;
    result *= 4;
    result += curc;
    result *= 4;
    result += destr;
    result *= 4;
    result += destc;
    result *= 30;
    result += nrobots;
    return result;
}

std::map<uint64_t, int64_t> memo;

int64_t cheapestDirPad(int curr, int curc, int destr, int destc, int nrobots)
{
    uint64_t h = hash(curr, curc, destr, destc, nrobots);
    auto it = memo.find(h);
    if (it != memo.end())
        return it->second;

    int64_t answer = std::numeric_limits<int64_t>::max();
    struct Visit
    {
        int r, c;
        std::string presses;
    };
    std::deque<Visit> q;
    q.push_back({ curr, curc, "" });
    while (!q.empty())
    {
        auto v = q.front();
        q.pop_front();
        if (v.r == destr && v.c == destc)
        {
            int64_t rec = cheapestRobot(v.presses + "A", nrobots-1);
            answer = std::min(answer, rec);
            continue;
        }
        if (v.r == 0 && v.c == 0)
            continue;
        else
        {
            if (v.r < destr)
            {
                q.push_back({ v.r + 1,v.c,v.presses + "v" });
            }
            else if (v.r > destr)
            {
                q.push_back({ v.r - 1, v.c, v.presses + "^" });
            }
            if (v.c < destc)
            {
                q.push_back({ v.r, v.c + 1, v.presses + ">" });
            }
            else if (v.c > destc)
            {
                q.push_back({ v.r, v.c - 1, v.presses + "<" });
            }
        }
    }
    memo[h] = answer;
    return answer;
}

int64_t cheapestRobot(const std::string& presses, int nrobots)
{
    if (nrobots == 1)
        return presses.length();

    int64_t result = 0;
    std::string padConfig = "X^A<v>";

    int curr = 0;
    int curc = 2;

    for (int i = 0; i < presses.length(); i++)
    {
        for (int nextr = 0; nextr < 2; nextr++)
        {
            for (int nextc = 0; nextc < 3; nextc++)
            {
                if (padConfig[nextr * 3 + nextc] == presses[i])
                {
                    result += cheapestDirPad(curr, curc, nextr, nextc, nrobots);
                    curr = nextr;
                    curc = nextc;
>>>>>>> e834f0da99848b39036854b69e64195abc3be68f
                }
            }
        }
    }
<<<<<<< HEAD

    return -1;  // Path not found
}

// Function to calculate the complexity of the code
long long int calculateComplexity(const string& code) {
    int numericPart = stoi(code.substr(0, code.length() - 1));
    int shortestPathLength = findShortestPathNumeric(code);
    return numericPart * shortestPathLength;
}

int main() {
    vector<string> codes = readInputLines();
    long long int totalComplexity = 0;

    for (const auto& code : codes) {
        auto cpx = calculateComplexity(code);
        totalComplexity += cpx;
        cerr << code << " " << cpx << endl;
    }

    cout << "Sum of the complexities of the five codes: " << totalComplexity << endl;

    return 0;
}

=======
    return result;
}

int64_t cheapest(int curr, int curc, int destr, int destc)
{
    int64_t answer = std::numeric_limits<int64_t>::max();
    struct Visit
    {
        int r, c;
        std::string presses;
    };
    std::deque<Visit> q;
    q.push_back({ curr, curc, "" });
    while (!q.empty())
    {
        auto v = q.front();
        q.pop_front();
        if (v.r == destr && v.c == destc)
        {
            int64_t rec = cheapestRobot(v.presses + "A", 26);
            answer = std::min(answer, rec);
            continue;
        }
        if (v.r == 3 && v.c == 0)
            continue;
        else
        {
            if (v.r < destr)
            {
                q.push_back({ v.r + 1,v.c,v.presses + "v" });
            }
            else if (v.r > destr)
            {
                q.push_back({ v.r - 1, v.c, v.presses + "^" });
            }
            if (v.c < destc)
            {
                q.push_back({ v.r, v.c + 1, v.presses + ">" });
            }
            else if (v.c > destc)
            {
                q.push_back({ v.r, v.c - 1, v.presses + "<" });
            }
        }
    }
    return answer;
}

int main()
{
    int64_t sum = 0;
    while (true)
    {
        std::string s;
        std::getline(std::cin, s);
        if (!std::cin)
            break;

        int64_t result = 0;
        std::string padConfig = "789456123X0A";

        int curr = 3;
        int curc = 2;

        for (int i = 0; i < s.length(); i++)
        {
            for (int nextr = 0; nextr < 4; nextr++)
            {
                for (int nextc = 0; nextc < 3; nextc++)
                {
                    if (padConfig[nextr * 3 + nextc] == s[i])
                    {
                        result += cheapest(curr, curc, nextr, nextc);
                        curr = nextr;
                        curc = nextc;
                    }
                }
            }
        }
        std::stringstream ss(s);
        int code;
        ss >> code;
        sum += result * code;
    }
    std::cout << sum << std::endl;
}
>>>>>>> e834f0da99848b39036854b69e64195abc3be68f

DEFINE_int32(test, 1, "test number");

void first() {
    const auto input = readInputLines();
    int result = 0;
    cout << result << endl;
}

void second() {
    const auto input = readInputLines();
    int result = 0;
    cout << result << endl;
}

/*
int main(int argc, char* argv[]) {
    standardInit(argc, argv);

    if (FLAGS_test == 1) {
        first();
    } else if (FLAGS_test == 2) {
        second();
    }
    return 0;
}
<<<<<<< HEAD
*/
=======
*.
>>>>>>> e834f0da99848b39036854b69e64195abc3be68f
