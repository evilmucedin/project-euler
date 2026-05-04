#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

enum class Direction { UP, DOWN, LEFT, RIGHT };

struct Point {
    int x;
    int y;

    Point(int x, int y) : x(x), y(y) {}
};

class LavaContraption {
private:
    std::vector<std::string> grid;
    std::unordered_set<std::pair<int, int>> energizedTiles;
    std::unordered_set<std::pair<int, std::pair<int, int>>> cache;

    bool isValidPosition(int x, int y) const {
        return x >= 0 && y >= 0 && x < grid.size() && y < grid[0].size();
    }

    void moveBeam(Point& point, Direction& direction) {
        switch (direction) {
            case Direction::UP:
                point.x--;
                break;
            case Direction::DOWN:
                point.x++;
                break;
            case Direction::LEFT:
                point.y--;
                break;
            case Direction::RIGHT:
                point.y++;
                break;
        }
    }

    void reflectBeam(Point& point, Direction& direction) {
        char currentTile = grid[point.x][point.y];
        if (currentTile == '/') {
            if (direction == Direction::UP) {
                direction = Direction::RIGHT;
            } else if (direction == Direction::DOWN) {
                direction = Direction::LEFT;
            } else if (direction == Direction::LEFT) {
                direction = Direction::DOWN;
            } else if (direction == Direction::RIGHT) {
                direction = Direction::UP;
            }
        } else if (currentTile == '\\') {
            if (direction == Direction::UP) {
                direction = Direction::LEFT;
            } else if (direction == Direction::DOWN) {
                direction = Direction::RIGHT;
            } else if (direction == Direction::LEFT) {
                direction = Direction::UP;
            } else if (direction == Direction::RIGHT) {
                direction = Direction::DOWN;
            }
        } else if (currentTile == '-') {
            if (direction == Direction::LEFT) {
                direction = Direction::RIGHT;
            } else if (direction == Direction::RIGHT) {
                direction = Direction::LEFT;
            }
        } else if (currentTile == '|') {
            if (direction == Direction::UP) {
                direction = Direction::DOWN;
            } else if (direction == Direction::DOWN) {
                direction = Direction::UP;
            }
        }
    }

    void processTile(Point point, Direction direction) {
        while (isValidPosition(point.x, point.y)) {

            auto key = make_pair<int, pair<int, int>>((int)direction, make_pair(point.x, point.y));
            if (cache.count(key)) {
                return;
            }
            cache.insert(key);

            energizedTiles.insert({ point.x, point.y });

            char currentTile = grid[point.x][point.y];
            if (currentTile == '.') {
                moveBeam(point, direction);
            } else if (currentTile == '/' || currentTile == '\\') {
                reflectBeam(point, direction);
                moveBeam(point, direction);
            } else { // Splitter encountered, create two beams
                Direction originalDirection = direction;
                Point originalPoint = point;
                processTile(point, direction);

                // Move in the other direction of the splitter
                direction = originalDirection;
                point = originalPoint;
                reflectBeam(point, direction);
                moveBeam(point, direction);
                processTile(point, direction);
                return;
            }
        }
    }

public:
    LavaContraption(const std::vector<std::string>& inputGrid) : grid(inputGrid) {}

    void print() const {
        for (int i = 0; i < grid.size(); ++i) {
            for (int j = 0; j < grid[i].size(); ++j) {
                if (energizedTiles.count({i, j})) {
                    cout << "#";
                } else {
                    cout << ".";
                }
            }
            cout << endl;
        }
        cout << endl;
    }

    int countEnergizedTiles() {
        energizedTiles.clear();

        Point startingPoint(0, 0);
        Direction beamDirection = Direction::RIGHT;

        processTile(startingPoint, beamDirection);

        return energizedTiles.size();
    }
};


void first() {
    const auto input = readInputLines();
    LavaContraption lc(input);
    int result = lc.countEnergizedTiles();
    lc.print();
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

