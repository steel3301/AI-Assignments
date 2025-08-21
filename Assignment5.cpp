#include <bits/stdc++.h>
using namespace std;

const char HUMAN = 'O';
const char AI = 'X';
const char EMPTY = '_';

struct Move {
    int row, col;
};

// Check if moves are left
bool isMovesLeft(vector<vector<char>> &board) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[i][j] == EMPTY)
                return true;
    return false;
}

// Evaluate board state
int evaluate(vector<vector<char>> &b) {
    // Check rows
    for (int row = 0; row < 3; row++) {
        if (b[row][0] == b[row][1] && b[row][1] == b[row][2]) {
            if (b[row][0] == AI) return +10;
            else if (b[row][0] == HUMAN) return -10;
        }
    }
    // Check columns
    for (int col = 0; col < 3; col++) {
        if (b[0][col] == b[1][col] && b[1][col] == b[2][col]) {
            if (b[0][col] == AI) return +10;
            else if (b[0][col] == HUMAN) return -10;
        }
    }
    // Check diagonals
    if (b[0][0] == b[1][1] && b[1][1] == b[2][2]) {
        if (b[0][0] == AI) return +10;
        else if (b[0][0] == HUMAN) return -10;
    }
    if (b[0][2] == b[1][1] && b[1][1] == b[2][0]) {
        if (b[0][2] == AI) return +10;
        else if (b[0][2] == HUMAN) return -10;
    }
    return 0;
}

// Minimax with alpha-beta pruning
int minimax(vector<vector<char>> &board, int depth, bool isMax, int alpha, int beta) {
    int score = evaluate(board);

    if (score == 10) return score - depth;     // AI win
    if (score == -10) return score + depth;    // Human win
    if (!isMovesLeft(board)) return 0;         // Draw

    if (isMax) { // AI's turn
        int best = -1000;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == EMPTY) {
                    board[i][j] = AI;
                    best = max(best, minimax(board, depth + 1, !isMax, alpha, beta));
                    board[i][j] = EMPTY;
                    alpha = max(alpha, best);
                    if (beta <= alpha) return best; // Beta cut-off
                }
            }
        }
        return best;
    } else { // Human's turn
        int best = 1000;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == EMPTY) {
                    board[i][j] = HUMAN;
                    best = min(best, minimax(board, depth + 1, !isMax, alpha, beta));
                    board[i][j] = EMPTY;
                    beta = min(beta, best);
                    if (beta <= alpha) return best; // Alpha cut-off
                }
            }
        }
        return best;
    }
}

// Find the best move for AI
Move findBestMove(vector<vector<char>> &board) {
    int bestVal = -1000;
    Move bestMove = {-1, -1};

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == EMPTY) {
                board[i][j] = AI;
                int moveVal = minimax(board, 0, false, -1000, 1000);
                board[i][j] = EMPTY;

                if (moveVal > bestVal) {
                    bestMove.row = i;
                    bestMove.col = j;
                    bestVal = moveVal;
                }
            }
        }
    }
    return bestMove;
}

// Print board
void printBoard(vector<vector<char>> &board) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++)
            cout << board[i][j] << " ";
        cout << endl;
    }
}

int main() {
    vector<vector<char>> board(3, vector<char>(3, EMPTY));
    int x, y;

    cout << "Tic-Tac-Toe using Minimax (AI = X, You = O)\n";

    while (true) {
        printBoard(board);

        if (!isMovesLeft(board) || evaluate(board) != 0) break;

        cout << "Enter your move (row col): ";
        cin >> x >> y;
        if (x >= 0 && x < 3 && y >= 0 && y < 3 && board[x][y] == EMPTY)
            board[x][y] = HUMAN;
        else {
            cout << "Invalid move! Try again.\n";
            continue;
        }

        if (!isMovesLeft(board) || evaluate(board) != 0) break;

        Move bestMove = findBestMove(board);
        board[bestMove.row][bestMove.col] = AI;
    }

    printBoard(board);
    int score = evaluate(board);
    if (score == 10) cout << "AI Wins!\n";
    else if (score == -10) cout << "You Win!\n";
    else cout << "It's a Draw!\n";

    return 0;
}
