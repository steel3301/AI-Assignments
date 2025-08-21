#include <bits/stdc++.h>
using namespace std;

// Structure for a cell in the grid
struct Cell {
    int parentX, parentY; // Parent cell coordinates
    double f, g, h;       // f = g + h
    Cell() {
        parentX = -1;
        parentY = -1;
        f = g = h = FLT_MAX;
    }
};

// Directions (Up, Down, Left, Right, Diagonals)
int dx[8] = {-1, 1, 0, 0, -1, -1, 1, 1};
int dy[8] = {0, 0, -1, 1, -1, 1, -1, 1};

// Check if cell is valid
bool isValid(int x, int y, int ROW, int COL) {
    return (x >= 0 && x < ROW && y >= 0 && y < COL);
}

// Check if cell is blocked
bool isUnblocked(vector<vector<int>>& grid, int x, int y) {
    return grid[x][y] == 1;
}

// Check if reached destination
bool isDestination(int x, int y, pair<int,int> dest) {
    return (x == dest.first && y == dest.second);
}

// Calculate heuristic (Euclidean distance)
double calculateH(int x, int y, pair<int,int> dest) {
    return sqrt((x - dest.first)*(x - dest.first) + (y - dest.second)*(y - dest.second));
}

// Trace path back
void tracePath(vector<vector<Cell>>& cellDetails, pair<int,int> dest) {
    cout << "Path found: ";
    int row = dest.first;
    int col = dest.second;

    stack<pair<int,int>> Path;

    while (!(cellDetails[row][col].parentX == row && cellDetails[row][col].parentY == col)) {
        Path.push({row, col});
        int temp_row = cellDetails[row][col].parentX;
        int temp_col = cellDetails[row][col].parentY;
        row = temp_row;
        col = temp_col;
    }

    Path.push({row, col});
    while (!Path.empty()) {
        pair<int,int> p = Path.top();
        Path.pop();
        cout << "(" << p.first << "," << p.second << ") ";
    }
    cout << endl;
}

// A* Search Algorithm
void aStarSearch(vector<vector<int>>& grid, pair<int,int> src, pair<int,int> dest) {
    int ROW = grid.size();
    int COL = grid[0].size();

    if (!isValid(src.first, src.second, ROW, COL) || !isValid(dest.first, dest.second, ROW, COL)) {
        cout << "Source or destination invalid!" << endl;
        return;
    }

    if (!isUnblocked(grid, src.first, src.second) || !isUnblocked(grid, dest.first, dest.second)) {
        cout << "Source or destination blocked!" << endl;
        return;
    }

    if (isDestination(src.first, src.second, dest)) {
        cout << "Already at the destination!" << endl;
        return;
    }

    vector<vector<bool>> closedList(ROW, vector<bool>(COL, false));
    vector<vector<Cell>> cellDetails(ROW, vector<Cell>(COL));

    int i = src.first, j = src.second;
    cellDetails[i][j].f = 0.0;
    cellDetails[i][j].g = 0.0;
    cellDetails[i][j].h = 0.0;
    cellDetails[i][j].parentX = i;
    cellDetails[i][j].parentY = j;

    set<pair<double, pair<int,int>>> openList;
    openList.insert({0.0, {i,j}});

    bool foundDest = false;

    while (!openList.empty()) {
        pair<double, pair<int,int>> p = *openList.begin();
        openList.erase(openList.begin());

        i = p.second.first;
        j = p.second.second;
        closedList[i][j] = true;

        // Check neighbors
        for (int d = 0; d < 8; d++) {
            int newX = i + dx[d];
            int newY = j + dy[d];

            if (isValid(newX, newY, ROW, COL)) {
                if (isDestination(newX, newY, dest)) {
                    cellDetails[newX][newY].parentX = i;
                    cellDetails[newX][newY].parentY = j;
                    cout << "Destination found!" << endl;
                    tracePath(cellDetails, dest);
                    foundDest = true;
                    return;
                }
                else if (!closedList[newX][newY] && isUnblocked(grid, newX, newY)) {
                    double gNew = cellDetails[i][j].g + 1.0;
                    double hNew = calculateH(newX, newY, dest);
                    double fNew = gNew + hNew;

                    if (cellDetails[newX][newY].f == FLT_MAX || cellDetails[newX][newY].f > fNew) {
                        openList.insert({fNew, {newX,newY}});
                        cellDetails[newX][newY].f = fNew;
                        cellDetails[newX][newY].g = gNew;
                        cellDetails[newX][newY].h = hNew;
                        cellDetails[newX][newY].parentX = i;
                        cellDetails[newX][newY].parentY = j;
                    }
                }
            }
        }
    }

    if (!foundDest)
        cout << "Failed to find destination!" << endl;
}

int main() {
    // 1 = open cell, 0 = blocked
    vector<vector<int>> grid =
    {
        { 1, 1, 1, 1, 1 },
        { 1, 0, 1, 0, 1 },
        { 1, 1, 1, 0, 1 },
        { 0, 0, 1, 1, 1 },
        { 1, 1, 1, 0, 1 }
    };

    pair<int,int> src = {0,0};
    pair<int,int> dest = {4,4};

    aStarSearch(grid, src, dest);

    return 0;
}
