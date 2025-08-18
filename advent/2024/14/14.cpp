#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

#include <iostream>
#include <vector>
#include <cmath>

// Constants for the size of the space
const int WIDTH = 101;
const int HEIGHT = 103;
const int SECONDS = 100;

// Struct to represent a robot's position and velocity
struct Robot {
    int px, py;  // Position
    int vx, vy;  // Velocity
};

// Function to move the robots and wrap around the edges if necessary
void move_robots(std::vector<Robot>& robots) {
    for (auto& robot : robots) {
        robot.px = (robot.px + robot.vx * SECONDS) % WIDTH;
        robot.py = (robot.py + robot.vy * SECONDS) % HEIGHT;
        while (robot.px < 0) robot.px += WIDTH;
        while (robot.py < 0) robot.py += HEIGHT;
    }
}

// Function to count the number of robots in each quadrant
std::vector<int> count_robots_in_quadrants(const std::vector<Robot>& robots) {
    std::vector<int> quadrant_counts(4, 0);
    for (const auto& robot : robots) {
        if (robot.px == WIDTH / 2 || robot.py == HEIGHT / 2) continue;
        if (robot.px < WIDTH / 2 && robot.py < HEIGHT / 2) ++quadrant_counts[0];
        else if (robot.px >= WIDTH / 2 && robot.py < HEIGHT / 2) ++quadrant_counts[1];
        else if (robot.px < WIDTH / 2 && robot.py >= HEIGHT / 2) ++quadrant_counts[2];
        else if (robot.px >= WIDTH / 2 && robot.py >= HEIGHT / 2) ++quadrant_counts[3];
    }
    return quadrant_counts;
}

int main() {
    // List of robots with their initial positions and velocities
    std::vector<Robot> robots = {
        {0, 4, 3, -3}, {6, 3, -1, -3}, {10, 3, -1, 2}, {2, 0, 2, -1},
        {0, 0, 1, 3}, {3, 0, -2, -2}, {7, 6, -1, -3}, {3, 0, -1, -2},
        {9, 3, 2, 3}, {7, 3, -1, 2}, {2, 4, 2, -3}, {9, 5, -3, -3}
    };

    // Move the robots for 100 seconds
    move_robots(robots);

    // Count the number of robots in each quadrant
    std::vector<int> quadrant_counts = count_robots_in_quadrants(robots);
    cerr << quadrant_counts << endl;

    // Calculate the safety factor
    int safety_factor = quadrant_counts[0] * quadrant_counts[1] * quadrant_counts[2] * quadrant_counts[3];

    // Output the result
    std::cout << "Safety factor after 100 seconds: " << safety_factor << std::endl;

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
