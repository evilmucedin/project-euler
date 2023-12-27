#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"
#include "lib/unionFind.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
using namespace std;

// A struct to store a component and its number of connections
struct Component {
  string name;
  int connections;
  Component(string n, int c) : name(n), connections(c) {}
};

// A comparator function to sort components in descending order by connections
bool compare(Component a, Component b) {
  return a.connections > b.connections;
}

// A function to find the three wires to disconnect and the product of the group sizes
int findWires(vector<string>& diagram) {
  // A map to store the connections of each component
  unordered_map<string, vector<string>> graph;
  // A vector to store the components and their number of connections
  vector<Component> components;
  // A variable to store the total number of components
  int n = 0;
  vector<pair<string, string>> allEdges;
  set<string> allNodes;

  UnionFind<string> uf0;

// Loop over the diagram and build the graph and the components vector
  for (string line : diagram) {
    // Split the line by colon
    size_t pos = line.find(':');
    string name = line.substr(0, pos);
    uf0.add(name);
    allNodes.emplace(name);
    string rest = line.substr(pos + 1);
    // Split the rest by space
    vector<string> neighbors;
    pos = 0;
    while ((pos = rest.find(' ')) != string::npos) {
      string neighbor = rest.substr(0, pos);
      rest.erase(0, pos + 1);
      if (neighbor.empty()) {
        continue;
      }
      allEdges.emplace_back(make_pair(name, neighbor));
      neighbors.push_back(neighbor);
      allNodes.emplace(neighbor);
    }
    neighbors.push_back(rest);
    // Add the component and its connections to the graph and the vector
    graph[name] = neighbors;
    components.push_back(Component(name, neighbors.size()));
    n++;
  }

  for (const auto& n: allNodes) {
    uf0.add(n);
  }
  cerr << vector<string>(allNodes.begin(), allNodes.end()) << endl;

  int res = 0;
  for (int i = 0; i < allEdges.size(); ++i) {
    for (int j = i + 1; j < allEdges.size(); ++j) {
        for (int k = j + 1; k < allEdges.size(); ++k) {
            UnionFind<string> uf = uf0;
            for (int l = 0; l < allEdges.size(); ++l) {
                if (l == i) {
                    continue;
                }
                if (l == j) {
                    continue;
                }
                if (l == k) {
                    continue;
                }
                uf.unite(allEdges[l].first, allEdges[l].second);
            }

           const UnionFind<string>::Components comp = uf.components();
           if (comp.size() == 2) {
               LOG(INFO) << OUT(to_string(comp)) << endl;
               res = max<int>(res, comp[0].size * comp[1].size);
               // LOG(INFO) << OUT(comp.size()) << OUT(comp[0].size) << OUT(comp[1].size);
           }
        }
    }
  }
  return res;

  // Sort the components in descending order by connections
  sort(components.begin(), components.end(), compare);

  // A vector to store the wires to disconnect
  vector<string> wires;
  // A map to store the group of each component
  unordered_map<string, int> group;
  // A variable to store the sum of the first group
  int sum1 = 0;
  // A variable to store the sum of the second group
  int sum2 = 0;

  // Loop over the components and assign them to the group with smaller sum
  for (Component c : components) {
    // If the first group has smaller sum, assign the component to it
    if (sum1 <= sum2) {
      group[c.name] = 1;
      sum1++;
    }
    // Otherwise, assign the component to the second group
    else {
      group[c.name] = 2;
      sum2++;
    }
    // Loop over the neighbors of the component and check if they are in the same group
    for (string neighbor : graph[c.name]) {
      // If they are in the same group, add a wire to disconnect
      if (group[neighbor] == group[c.name]) {
        wires.push_back(c.name + "/" + neighbor);
      }
    }
  }

  // Print the wires to disconnect
  cout << "The three wires to disconnect are: " << endl;
  for (int i = 0; i < min<int>(wires.size(), 3); i++) {
    cout << wires[i] << endl;
  }

  // Return the product of the group sizes
  return sum1 * sum2;
}



void first() {
    auto input = readInputLines();
    cout << findWires(input) << endl;
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

