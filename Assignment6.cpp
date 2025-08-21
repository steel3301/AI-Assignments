#include <bits/stdc++.h>
using namespace std;

const int N = 8;  // Board size (8x8 for 8 queens)

int board[N][N];

// Check if a queen can be placed at board[row][col]
bool isSafe(int row, int col) {
    // Check column
    for (int i = 0; i < row; i++)
        if (board[i][col]) return false;

    // Check upper-left diagonal
    for (int i = row - 1, j = col - 1; i >= 0 && j >= 0; i--, j--)
        if (board[i][j]) return false;

    // Check upper-right diagonal
    for (int i = row - 1, j = col + 1; i >= 0 && j < N; i--, j++)
        if (board[i][j]) return false;

    return true;
}

// Recursive function to solve 8-Queens
bool solveNQueens(int row) {
    if (row == N) return true; // All queens placed

    for (int col = 0; col < N; col++) {
        if (isSafe(row, col)) {
            board[row][col] = 1; // Place queen

            if (solveNQueens(row + 1)) return true; // Recurse

            board[row][col] = 0; // Backtrack
        }
    }
    return false; // No valid position found
}

// Print the chessboard
void printBoard() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cout << (board[i][j] ? "Q " : ". ");
        }
        cout << endl;
    }
}

int main() {
    memset(board, 0, sizeof(board));

    if (solveNQueens(0)) {
        cout << "Solution to 8-Queens Problem:\n";
        printBoard();
    } else {
        cout << "No solution exists!\n";
    }

    return 0;
}
