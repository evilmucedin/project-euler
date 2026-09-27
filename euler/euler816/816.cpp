#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>

// Structure to represent a point in 2D space
struct Point {
    long long x, y;
};

// Custom comparator to sort points primarily by X-coordinate, and secondarily by Y
bool compareX(const Point& a, const Point& b) {
    if (a.x != b.x) return a.x < b.x;
    return a.y < b.y;
}

int main() {
    // Problem parameters
    const int N = 2000000;

    // Pseudo-random generation constants (Blum Blum Shub variant)
    long long s = 290797;
    const long long M = 50515093;

    // Generate the points
    std::vector<Point> points(N);
    for (int i = 0; i < N; ++i) {
        s = (s * s) % M;
        long long x = s;
        s = (s * s) % M;
        long long y = s;
        points[i] = {x, y};
    }

    // Step 1: Sort all points by their X-coordinate
    std::sort(points.begin(), points.end(), compareX);

    // Step 2: Sweep-line approach to find the minimum distance
    // Initialize min_d_sq with the squared distance between the first two points
    double min_d = std::hypot(points[0].x - points[1].x, points[0].y - points[1].y);

    int left = 0; // Left bound index for our active search window

    for (int i = 0; i < N; ++i) {
        // Narrow down the sliding window: eliminate points whose X distance to points[i] is >= min_d
        while (points[i].x - points[left].x >= min_d) {
            left++;
        }

        // Check points within the horizontal window
        for (int j = left; j < i; ++j) {
            // Check vertical distance boundary constraint
            if (std::abs(points[i].y - points[j].y) < min_d) {
                double d = std::hypot(points[i].x - points[j].x, points[i].y - points[j].y);
                if (d < min_d) {
                    min_d = d;
                }
            }
        }
    }

    // Print the final result formatted to 9 decimal places
    std::cout << std::fixed << std::setprecision(9) << min_d << std::endl;

    return 0;
}

