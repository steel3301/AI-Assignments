#include <bits/stdc++.h>
using namespace std;

/*
 * Constraint Satisfaction Problem Example:
 * MAP COLORING with 4 colors.
 *
 * - Variables: Regions (A, B, C, D, E, F, G)
 * - Domain: {Red, Green, Blue, Yellow}
 * - Constraints: Adjacent regions cannot share the same color
 *
 * Solved using Backtracking.
 */

vector<string> colors = {"Red", "Green", "Blue", "Yellow"};

// Graph adjacency list
map<char, vector<char>> adj = {
    {'A', {'B','C','D'}},
    {'B', {'A','C','E'}},
    {'C', {'A','B','D','E','F'}},
    {'D', {'A','C','F','G'}},
    {'E', {'B','C','F'}},
    {'F', {'C','D','E','G'}},
    {'G', {'D','F'}}
};

// Function to check if assignment is valid
bool isSafe(char node, string color, map<char, string>& assignment) {
    for (char neighbor : adj[node]) {
        if (assignment[neighbor] == color) {
            return false; // conflict
        }
    }
    return true;
}

// Backtracking CSP solver
bool solveCSP(vector<char>& nodes, int idx, map<char, string>& assignment) {
    if (idx == nodes.size()) return true; // all nodes assigned

    char node = nodes[idx];
    for (string color : colors) {
        if (isSafe(node, color, assignment)) {
            assignment[node] = color;

            if (solveCSP(nodes, idx + 1, assignment))
                return true;

            assignment[node] = ""; // backtrack
        }
    }
    return false;
}

int main() {
    vector<char> nodes = {'A','B','C','D','E','F','G'};
    map<char, string> assignment;

    // initialize with no color
    for (char node : nodes) assignment[node] = "";

    if (solveCSP(nodes, 0, assignment)) {
        cout << "Solution Found:\n";
        for (char node : nodes) {
            cout << "Region " << node << " -> " << assignment[node] << "\n";
        }
    } else {
        cout << "No solution exists.\n";
    }

    return 0;
}
