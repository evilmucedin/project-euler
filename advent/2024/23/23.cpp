#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <sstream>

using namespace std;

// Function to split the input string by a delimiter and return a vector of tokens
vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;
    stringstream ss(str);
    string token;
    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Function to find all sets of three interconnected computers
set<set<string>> findTriangles(const unordered_map<string, unordered_set<string>>& graph) {
    set<set<string>> triangles;
    for (const auto& [node, neighbors] : graph) {
        for (const string& neighbor : neighbors) {
            if (graph.find(neighbor) != graph.end()) {
                for (const string& secondNeighbor : graph.at(neighbor)) {
                    if (graph.at(node).find(secondNeighbor) != graph.at(node).end()) {
                        set<string> triangle = {node, neighbor, secondNeighbor};
                        triangles.insert(triangle);
                    }
                }
            }
        }
    }
    return triangles;
}

// Function to check if all nodes in the set are fully connected
bool isFullyConnected(const set<string>& nodes, const unordered_map<string, unordered_set<string>>& graph) {
    for (const auto& node : nodes) {
        for (const auto& other : nodes) {
            if (node != other && graph.at(node).find(other) == graph.at(node).end()) {
                return false;
            }
        }
    }
    return true;
}

// Function to find the largest clique (largest set of fully connected nodes)
set<string> findLargestClique(const unordered_map<string, unordered_set<string>>& graph) {
    set<string> largestClique;
    for (const auto& [node, neighbors] : graph) {
        set<string> clique = {node};
        for (const auto& neighbor : neighbors) {
            if (all_of(clique.begin(), clique.end(), [&](const string& c) { return graph.at(c).find(neighbor) != graph.at(c).end(); })) {
                clique.insert(neighbor);
            }
        }
        if (clique.size() > largestClique.size()) {
            largestClique = clique;
        }
    }
    return largestClique;
}



int main() {
    vector<string> connections = readInputLines();

    unordered_map<string, unordered_set<string>> graph;

    // Build the graph
    for (const string& connection : connections) {
        vector<string> nodes = split(connection, '-');
        graph[nodes[0]].insert(nodes[1]);
        graph[nodes[1]].insert(nodes[0]);
    }

    // Find all triangles
    set<set<string>> triangles = findTriangles(graph);

    // Count triangles containing at least one computer with a name starting with 't'
    int count = 0;
    for (const auto& triangle : triangles) {
        for (const string& node : triangle) {
            if (node[0] == 't') {
                count++;
                break;
            }
        }
    }

    cout << "Number of triangles with at least one computer starting with 't': " << count << endl;

// Find the largest clique and construct the password
    set<string> largestClique = findLargestClique(graph);
    vector<string> sortedClique(largestClique.begin(), largestClique.end());
    sort(sortedClique.begin(), sortedClique.end());

    string password;
    for (size_t i = 0; i < sortedClique.size(); ++i) {
        password += sortedClique[i];
        if (i < sortedClique.size() - 1) {
            password += ",";
        }
    }

    cout << "Password to get into the LAN party: " << password << endl;


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
