
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>

#include "lib/header.h"
#include "lib/string.h"

#include "advent/lib/aoc.h"

using namespace std;

int solve(const std::vector<std::string>& manifold) {
    // Start with a single beam at the 'S' position.
    // Find the 'S' column. The row is 0.
    int start_col = -1;
    for (int i = 0; i < manifold[0].length(); ++i) {
        if (manifold[0][i] == 'S') {
            start_col = i;
            break;
        }
    }

    if (start_col == -1) return 0;

    // Use a set to store the active beam columns (represented by intervals between beams)
    // A better approach might be using a boolean array for the current row's active cells
    // or a set of column indices where beams are present.
    // The example uses intervals of *active* downward beams.

    // Let's use a set of columns where the beams are present.
    std::set<int> active_beams;
    active_beams.insert(start_col);
    int split_count = 0;

    for (int row = 1; row < manifold.size(); ++row) {
        std::set<int> next_beams;
        for (int col : active_beams) {
            // Check for splitters in the current beam path (at the current row).
            if (col >= 0 && col < manifold[row].length() && manifold[row][col] == '^') {
                // Beam hits a splitter: stops and creates two new beams left and right.
                // The new beams continue from the immediate left/right in the *next* row.
                // Or rather, as shown in the example, they continue downwards from the current position's left and right *columns*.

                // The example diagram shows the splitters at the *row where the beam stops*.
                // The new beams appear in the *same* row, one column left and one column right.
                // The problem statement says "continues from the immediate left and from the immediate right of the splitter".
                // The visual implies new beams *start* at the same level as the splitter and go down.

                // Let's re-read the example.
                // The beam goes from S (row 0, col C) to the first ^ at (row 2, col C).
                // At row 2, the beam stops. New beams start from (row 2, col C-1) and (row 2, col C+1) downwards.
                // So the set of active columns needs to be updated *within* the current row's logic.

                // A better approach might be to process row by row and manage a dynamic set of "active columns for the *next* row".

                // Let's try that.
            }
        }
    }
    return split_count; // The simulation logic needs refinement
}

int main() {
    auto lines = readInputLines();

    int sum = solve(lines);

    cout << sum << endl;

    return 0;
}
