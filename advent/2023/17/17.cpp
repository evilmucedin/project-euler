#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

#include <climits>

// A struct to represent a state of the crucible
struct State {
  int x; // the x-coordinate of the current block
  int y; // the y-coordinate of the current block
  int heat; // the total heat loss so far
  int dir; // the current direction of the crucible (0: up, 1: right, 2: down, 3: left)
  int turns; // the number of consecutive blocks moved in the same direction
};

// A function to check if a state is valid
bool isValid(const State& s, int n, int m) {
  return s.x >= 0 && s.x < n && s.y >= 0 && s.y < m && s.turns <= 3;
}

// A function to check if a state is the goal state
bool isGoal(const State& s, int n, int m) {
  return s.x == n - 1 && s.y == m - 1;
}

// A function to find the least heat loss using BFS
int findLeastHeatLoss(const vector<vector<int>>& map) {
  int n = map.size();                        // the number of rows in the map
  int m = map[0].size();                     // the number of columns in the map
  int minHeat = numeric_limits<int>::max();  // the minimum heat loss so far
  queue<State> q;                            // a queue to store the states to explore
  vector<vector<vector<bool>>> visited(
      n, vector<vector<bool>>(m, vector<bool>(4, false)));  // a 3D vector to mark the visited states
  // The initial state is the top-left block with zero heat loss, direction right, and one turn
  State start = {0, 0, map[0][0], 1, 1};
  q.push(start);                              // push the initial state to the queue
  visited[0][0][1] = true;                    // mark the initial state as visited
  while (!q.empty()) {                        // while the queue is not empty
      State curr = q.front();                 // get the current state from the front of the queue
      q.pop();                                // pop the current state from the queue
      if (isGoal(curr, n, m)) {               // if the current state is the goal state
          minHeat = min(minHeat, curr.heat);  // update the minimum heat loss
      } else {                                // otherwise
          // Try all possible moves from the current state
          for (int i = -1; i <= 1; i++) {            // for each possible change in direction
              int nextDir = (curr.dir + i + 4) % 4;  // the next direction after the change
              int nextX = curr.x;                    // the next x-coordinate after the move
              int nextY = curr.y;                    // the next y-coordinate after the move
              int nextTurns = curr.turns;            // the next number of turns after the move
              // Update the next coordinates and turns based on the next direction
              switch (nextDir) {
                  case 0:  // up
                      nextX--;
                      break;
                  case 1:  // right
                      nextY++;
                      break;
                  case 2:  // down
                      nextX++;
                      break;
                  case 3:  // left
                      nextY--;
                      break;
              }
              if (i == 0) {       // if the direction is unchanged
                  nextTurns++;    // increment the turns
              } else {            // otherwise
                  nextTurns = 1;  // reset the turns to one
              }
              if (nextX < 0 || nextX >= n) {
                  continue;
              }
              if (nextY < 0 || nextY >= m) {
                  continue;
              }
              // The next state after the move
              State next = {nextX, nextY, curr.heat + map[nextX][nextY] - map[curr.x][curr.y], nextDir, nextTurns};
              // If the next state is valid and not visited
              if (isValid(next, n, m) && !visited[nextX][nextY][nextDir]) {
                  q.push(next);                           // push the next state to the queue
                  visited[nextX][nextY][nextDir] = true;  // mark the next state as visited
              }
          }
      }
  }
  return minHeat;  // return the minimum heat loss
}

// ===================================

int minHeatLoss = INT_MAX;

bool isValid(int x, int y, std::vector<std::vector<bool>>& visited, const vector<vector<int>>& grid) {
  return x >= 0 && x < grid.size() && y >= 0 && y < grid[x].size() && !visited[x][y];
}

void findMinHeatLoss(int x, int y, int heatLoss, int direction, int consecutive,
                     std::vector<std::vector<bool>>& visited, const vector<vector<int>>& grid) {
  if (x == grid.size() - 1 && y == grid[x].size() - 1) {
      minHeatLoss = std::min(minHeatLoss, heatLoss);
      return;
  }

  visited[x][y] = true;

  // Move in four directions: right, down, left, up
  int dx[] = {0, 1, 0, -1};
  int dy[] = {1, 0, -1, 0};

  for (int i = 0; i < 4; ++i) {
      int newX = x + dx[i];
      int newY = y + dy[i];

      if (isValid(newX, newY, visited, grid)) {
          int newHeatLoss = heatLoss + grid[newX][newY];

          // Check consecutive movements in the same direction
          if (i == direction) {
              findMinHeatLoss(newX, newY, newHeatLoss, i, consecutive + 1, visited, grid);
          } else {
              findMinHeatLoss(newX, newY, newHeatLoss, i, 1, visited, grid);
          }
      }
  }

  visited[x][y] = false;
}

void first() {
    const auto input = readInputLines();
    vector<vector<int>> grid(input.size(), vector<int>(input[0].size()));
    for (int i = 0; i < input.size(); ++i) {
        for (int j = 0; j < input[i].size(); ++j) {
            grid[i][j] = input[i][j] - '0';
        }
    }
    /*
    std::vector<std::vector<bool>> visited(input.size(), std::vector<bool>(input[0].size(), false));
    findMinHeatLoss(0, 0, grid[0][0], 0, 1, visited, grid);
    cout << minHeatLoss << endl;
    */
    cout << findLeastHeatLoss(grid) << endl;
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

