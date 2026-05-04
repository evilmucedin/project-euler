#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"

#include "gflags/gflags.h"

DEFINE_int32(test, 1, "test number");

struct Vertex {
    string name;
    bool isBig;
    bool isStart;
    bool isFinish;
    size_t index;
    vector<int> adj;
    int visited{};
};

using Vertexes = vector<Vertex>;

bool isBigS(const string& name) {
    for (char ch: name) {
        if (!isupper(ch)) {
            return false;
        }
    }
    return true;
}

size_t find(const string& name, Vertexes& vs) {
    size_t i = 0;
    while (i < vs.size() && vs[i].name != name) {
        ++i;
    }
    if (i < vs.size()) {
        return i;
    }
    Vertex nw;
    nw.name = name;
    nw.isBig = isBigS(name);
    nw.isStart = name == "start";
    nw.isFinish = name == "end";
    nw.index = vs.size();
    nw.visited = 0;
    vs.emplace_back(nw);
    return nw.index;
}

Vertexes parseInput() {
    const auto input = readInputLines();

    Vertexes vs;
    for (const auto& s: input) {
        const auto parts = split(s, '-');
        size_t index1 = find(parts[0], vs);
        size_t index2 = find(parts[1], vs);
        vs[index1].adj.emplace_back(index2);
        vs[index2].adj.emplace_back(index1);
    }

    return vs;
}

size_t r1(Vertexes& v, size_t index) {
    if (v[index].visited) {
        if (!v[index].isBig) {
            return 0;
        }
    }
    ++v[index].visited;
    size_t res = 0;
    if (v[index].isFinish) {
        ++res;
    } else {
        for (auto i: v[index].adj) {
            res += r1(v, i);
        }
    }
    --v[index].visited;
    return res;
}

bool hasVisitedSmallTwice(const Vertexes& vs) {
    for (const auto& v: vs) {
        if (!v.isBig && v.visited >= 2) {
            return true;
        }
    }
    return false;
}

size_t r2(Vertexes& v, size_t index) {
    if (v[index].visited) {
        if (v[index].isStart) {
            return 0;
        }
        if (v[index].isFinish) {
            return 0;
        }
        if (!v[index].isBig && v[index].visited > 1) {
            return 0;
        }
        if (!v[index].isBig && v[index].visited == 1 && hasVisitedSmallTwice(v)) {
            return 0;
        }
    }
    ++v[index].visited;
    size_t res = 0;
    if (v[index].isFinish) {
        ++res;
    } else {
        for (auto i: v[index].adj) {
            res += r2(v, i);
        }
    }
    --v[index].visited;
    return res;
}

void first() {
    auto vs = parseInput();

    size_t count = r1(vs, find("start", vs));

    cout << count << endl;
}

void second() {
    auto vs = parseInput();

    size_t count = r2(vs, find("start", vs));

    cout << count << endl;
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

