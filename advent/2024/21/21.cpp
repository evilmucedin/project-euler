#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"


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
                }
            }
        }
    }
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
*.
