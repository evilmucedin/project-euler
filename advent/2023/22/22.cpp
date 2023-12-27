#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

// Structure to represent a brick
struct Brick {
    int x1, y1, z1; // Coordinates of one end
    int x2, y2, z2; // Coordinates of the other end
};

// Function to check if a brick is supported by another brick
bool isSupported(const Brick& brick, const Brick& supportingBrick) {
    return (supportingBrick.x1 <= brick.x1 && supportingBrick.y1 <= brick.y1 && supportingBrick.z1 < brick.z1 &&
            supportingBrick.x2 >= brick.x2 && supportingBrick.y2 >= brick.y2 && supportingBrick.z2 < brick.z2);
}

// Function to build a graph of supported bricks
std::unordered_map<char, std::unordered_set<char>> buildGraph(const std::vector<Brick>& bricks) {
    std::unordered_map<char, std::unordered_set<char>> graph;

    for (char ch1 = 'A'; ch1 < 'A' + bricks.size(); ++ch1) {
        for (char ch2 = 'A'; ch2 < 'A' + bricks.size(); ++ch2) {
            if (ch1 != ch2 && isSupported(bricks[ch1 - 'A'], bricks[ch2 - 'A'])) {
                graph[ch2].insert(ch1); // ch2 supports ch1
            }
        }
    }

    return graph;
}

// Function to perform topological sorting on the graph
std::vector<char> topologicalSort(std::unordered_map<char, std::unordered_set<char>>& graph) {
    std::vector<char> result;
    std::queue<char> q;

    for (const auto& entry : graph) {
        if (entry.second.empty()) {
            q.push(entry.first);
        }
    }

    while (!q.empty()) {
        char current = q.front();
        q.pop();
        result.push_back(current);

        for (auto& entry : graph) {
            if (entry.second.erase(current)) {
                if (entry.second.empty()) {
                    q.push(entry.first);
                }
            }
        }
    }

    return result;
}

// Function to count the number of bricks that cannot be safely disintegrated
int countUnsafeBricks(const std::vector<Brick>& bricks) {
    std::unordered_map<char, std::unordered_set<char>> graph = buildGraph(bricks);
    std::vector<char> sortedBricks = topologicalSort(graph);

    int bricksOnFloor = 0;
    for (const Brick& brick : bricks) {
        if (brick.z1 == 1 || brick.z2 == 1) {
            bricksOnFloor++;
        }
    }

    return bricksOnFloor + (bricks.size() - sortedBricks.size());
}

void first() {
    const auto input = readInputLines();
    vector<Brick> bricks;
    for (const auto& s: input) {
        const auto ints = parseIntegers(s);
        Brick b;
        b.x1 = ints[0];
        b.y1 = ints[1];
        b.z1 = ints[2];
        b.x2 = ints[3];
        b.y2 = ints[4];
        b.z2 = ints[5];
        bricks.emplace_back(b);
    }
    int result = countUnsafeBricks(bricks);
    cout << result << endl;
}

void second() {
    const auto input = readInputLines();
    int result = 0;
    cout << result << endl;
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

