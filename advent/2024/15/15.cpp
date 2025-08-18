#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

#include <iostream>
#include <vector>
#include <string>
#include <cmath>

// Dimensions of the warehouse
const int WIDTH = 10;
const int HEIGHT = 10;

// Robot structure to hold its position
struct Robot {
    int x, y;
};

// Function to move the robot and handle the movement of boxes
void move_robot(std::vector<std::string>& warehouse, Robot& robot, char move) {
    int dx = 0, dy = 0;
    if (move == '^') dy = -1;
    else if (move == 'v') dy = 1;
    else if (move == '<') dx = -1;
    else if (move == '>') dx = 1;

    int new_x = robot.x + dx;
    int new_y = robot.y + dy;
    int box_new_x = new_x + dx;
    int box_new_y = new_y + dy;

    // Check if the robot's new position is within bounds
    if (new_x >= 0 && new_x < WIDTH && new_y >= 0 && new_y < HEIGHT) {
        // Check if there's a box to push
        if (warehouse[new_y][new_x] == 'O') {
            // Check if the box can be pushed within bounds and not into a wall
            if (box_new_x >= 0 && box_new_x < WIDTH && box_new_y >= 0 && box_new_y < HEIGHT && warehouse[box_new_y][box_new_x] == '.') {
                warehouse[box_new_y][box_new_x] = 'O';
                warehouse[new_y][new_x] = '@';
                warehouse[robot.y][robot.x] = '.';
                robot.x = new_x;
                robot.y = new_y;
            }
        } else if (warehouse[new_y][new_x] == '.') { // Move robot if no box
            warehouse[new_y][new_x] = '@';
            warehouse[robot.y][robot.x] = '.';
            robot.x = new_x;
            robot.y = new_y;
        }
    }
}

// Function to calculate the sum of GPS coordinates of all boxes
int calculate_gps_sum(const std::vector<std::string>& warehouse) {
    int sum = 0;
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            if (warehouse[y][x] == 'O') {
                sum += 100 * y + x;
            }
        }
    }
    return sum;
}

int main() {
    // Initial warehouse state
    std::vector<std::string> warehouse = {
        "##########",
        "#..O..O.O#",
        "#......O.#",
        "#.OO..O.O#",
        "#..O@..O.#",
        "#O#..O...#",
        "#O..O..O.#",
        "#.OO.O.OO#",
        "#....O...#",
        "##########"
    };

    // Robot's initial position
    Robot robot = {4, 4};

    // Sequence of moves (example truncated for simplicity)
    std::string moves = "<vv>^<v^>v>^vv^v>v<>v^v<v<^vv<<<^><<><>>v<vvv<>^v^>^<<<><<v<<<v^vv^v>^vvv<<^>^v^^><<>>><>^<<><^vv^^<>vvv<>><^^v>^>vv<>v<<<<v<^v>^<^^>>>^<v<v><>vv>v^v^<>><>>>><^^>vv>v<^^^>>v^v^<^^>v^^>v^<^v>v<>>v^v^<v>v^^<^^vv<<v<^>>^^^^>>>v^<>vvv^><v<<<>^^^vv^<vvv>^>v<^^^^v<>^>vvvv><>>v^<<^^^^^<><^><>>><>^^<<^^v>>><^<v>^<vv>>v>>>^v><>^v><<<<v>>v<v<v>vvv>^<><<>^><^>><>^v<><^vvv<^^<><v<<<<<><^v<<<><<<^^<v<^^^><^>>^<v^><<<^>>^v<v^v<v^>^>>^v>vv>^<<^v<>><<><<v<<v><>v<^vv<<<>^^v^>^^>>><<^v>>v^v><^^>>^<>vv^<><^^>^^^<><vvvvv^v<v<<>^v<v>v<<^><<><<><<<^^<<<^<<>><<><^^^>^^<>^>v<>^^>vv<^v^v<vv>^<><v<^v>^^^>>>^^vvv^>vvv<>>>^<^>>>>>^<<^v>^vvv<>^<><<v>v^^>>><<^^<>>^v^<v^vv<>v^<<>^<^v^v><^<<<><<^<v><v<>vv>>v><v^<vv<>v^<<^";

    // Move the robot according to the sequence of moves
    for (char move : moves) {
        move_robot(warehouse, robot, move);
    }

    // Calculate the sum of GPS coordinates of all boxes
    int gps_sum = calculate_gps_sum(warehouse);

    // Output the result
    std::cout << "Sum of all boxes' GPS coordinates: " << gps_sum << std::endl;

    return 0;
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

    */
