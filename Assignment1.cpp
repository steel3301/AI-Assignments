#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <algorithm>
#include <array>
#include <chrono>
#include <iomanip>

using namespace std;

/*
 * 8-Puzzle solver with BFS (optimal) and DFS via Iterative Deepening (IDA-style without heuristic).
 * - State: string of length 9, e.g., "123405678" where '0' is the blank.
 * - Moves: U/D/L/R relative to the blank.
 * - Input:
 *    CLI: eight_puzzle [bfs|dfs] START GOAL
 *          START/GOAL are 9 digits (0..8), e.g., 123405678
 *    or via stdin: two 3x3 grids (whitespace-separated), 0 = blank.
 * - Output: solution path, moves, stats.
 */

// ---------- Utilities ----------
static inline string strip_non_digits(const string& s) {
    string t;
    for (char c : s) if (c >= '0' && c <= '9') t.push_back(c);
    return t;
}

static inline bool read_grid_state(string& out) {
    // Reads 9 digits from stdin across up to 3 lines. Ignores non-digits.
    // Returns true if exactly 9 digits were read.
    out.clear();
    string line, acc;
    int needed = 9;
    while ((int)strip_non_digits(acc).size() < needed && std::getline(cin, line)) {
        acc += line;
        if ((int)strip_non_digits(acc).size() >= needed) break;
    }
    acc = strip_non_digits(acc);
    if ((int)acc.size() < needed) return false;
    out = acc.substr(0, 9);
    return true;
}

static inline bool is_valid_perm(const string& s) {
    if (s.size() != 9) return false;
    array<bool, 10> seen{}; // 0..9 (we'll ignore index 9)
    int count = 0;
    for (char c : s) {
        if (c < '0' || c > '9') return false;
        int d = c - '0';
        if (d > 8) return false;
        if (seen[d]) return false;
        seen[d] = true;
        count++;
    }
    return count == 9;
}

static inline int inversions(const string& s) {
    // Count inversions excluding '0'
    vector<int> v;
    v.reserve(9);
    for (char c : s) if (c != '0') v.push_back(c - '0');
    int inv = 0;
    for (int i = 0; i < (int)v.size(); ++i)
        for (int j = i + 1; j < (int)v.size(); ++j)
            if (v[i] > v[j]) ++inv;
    return inv;
}

static inline bool is_solvable(const string& start, const string& goal) {
    // For 3x3 puzzle, solvable iff start and goal have same inversion parity
    return (inversions(start) % 2) == (inversions(goal) % 2);
}

struct Neighbor {
    string state;
    char move; // 'U','D','L','R'
};

static inline vector<Neighbor> neighbors(const string& s) {
    static const array<array<int, 4>, 9> adj = {{
        {{-1, +1, -1, +3}}, // 0: U,L invalid; R:1; D:3
        {{-1, +2,  0, +4}}, // 1
        {{-1, -1,  1,  5}}, // 2: no R (index3), but we encode via columns logic differently; we’ll compute moves below
    }};
    // We'll compute moves via row/col instead; above scratch left for clarity but not used.

    vector<Neighbor> out;
    int z = s.find('0');
    int r = z / 3, c = z % 3;

    auto swap_and_push = [&](int nr, int nc, char mv) {
        int idx = nr * 3 + nc;
        string t = s;
        swap(t[z], t[idx]);
        out.push_back({t, mv});
    };

    if (r > 0) swap_and_push(r - 1, c, 'U');
    if (r < 2) swap_and_push(r + 1, c, 'D');
    if (c > 0) swap_and_push(r, c - 1, 'L');
    if (c < 2) swap_and_push(r, c + 1, 'R');

    return out;
}

static inline vector<string> reconstruct_path(
    const unordered_map<string, string>& parent,
    const string& start, const string& goal)
{
    vector<string> path;
    if (!parent.count(goal) && start != goal) return path;
    string cur = goal;
    while (true) {
        path.push_back(cur);
        if (cur == start) break;
        cur = parent.at(cur);
    }
    reverse(path.begin(), path.end());
    return path;
}

static inline string path_moves(
    const unordered_map<string, pair<string,char>>& parent_move,
    const string& start, const string& goal)
{
    if (start == goal) return "";
    if (!parent_move.count(goal)) return "";
    string cur = goal;
    string moves;
    while (cur != start) {
        auto it = parent_move.find(cur);
        if (it == parent_move.end()) break;
        moves.push_back(it->second.second);
        cur = it->second.first;
    }
    reverse(moves.begin(), moves.end());
    return moves;
}

// ---------- BFS (Optimal in steps) ----------
struct SearchResult {
    bool found = false;
    vector<string> path;
    string moves;
    size_t nodes_expanded = 0;
    double millis = 0.0;
};

SearchResult bfs_solve(const string& start, const string& goal) {
    auto t0 = chrono::high_resolution_clock::now();

    if (start == goal) {
        auto t1 = chrono::high_resolution_clock::now();
        return {true, {start}, "", 0,
                chrono::duration<double, milli>(t1 - t0).count()};
    }

    queue<string> q;
    unordered_map<string, string> parent; // child -> parent
    unordered_map<string, pair<string,char>> parent_move; // child -> {parent, move}
    unordered_set<string> visited;
    visited.reserve(100000);
    parent.reserve(100000);
    parent_move.reserve(100000);

    q.push(start);
    visited.insert(start);

    size_t expanded = 0;

    while (!q.empty()) {
        string cur = q.front(); q.pop();
        ++expanded;

        for (const auto& nb : neighbors(cur)) {
            if (visited.insert(nb.state).second) {
                parent[nb.state] = cur;
                parent_move[nb.state] = {cur, nb.move};
                if (nb.state == goal) {
                    auto t1 = chrono::high_resolution_clock::now();
                    SearchResult res;
                    res.found = true;
                    res.path = reconstruct_path(parent, start, goal);
                    res.moves = path_moves(parent_move, start, goal);
                    res.nodes_expanded = expanded;
                    res.millis = chrono::duration<double, milli>(t1 - t0).count();
                    return res;
                }
                q.push(nb.state);
            }
        }
    }

    auto t1 = chrono::high_resolution_clock::now();
    return {false, {}, "", expanded, chrono::duration<double, milli>(t1 - t0).count()};
}

// ---------- DFS (Iterative Deepening) ----------
struct DfsCtx {
    string goal;
    size_t nodes_expanded = 0;
    unordered_set<string> pathset; // to avoid cycles along current path
    unordered_map<string, pair<string,char>> parent_move;
};

static bool dfs_limited(DfsCtx& ctx, const string& cur, int depth, int limit) {
    if (cur == ctx.goal) return true;
    if (depth == limit) return false;

    ++ctx.nodes_expanded;

    for (const auto& nb : neighbors(cur)) {
        if (ctx.pathset.insert(nb.state).second) {
            ctx.parent_move[nb.state] = {cur, nb.move};
            if (dfs_limited(ctx, nb.state, depth + 1, limit)) return true;
            ctx.pathset.erase(nb.state);
        }
    }
    return false;
}

SearchResult iddfs_solve(const string& start, const string& goal, int max_depth = 40) {
    auto t0 = chrono::high_resolution_clock::now();

    if (start == goal) {
        auto t1 = chrono::high_resolution_clock::now();
        return {true, {start}, "", 0,
                chrono::duration<double, milli>(t1 - t0).count()};
    }

    for (int limit = 0; limit <= max_depth; ++limit) {
        DfsCtx ctx;
        ctx.goal = goal;
        ctx.pathset.clear();
        ctx.parent_move.clear();
        ctx.pathset.insert(start);

        if (dfs_limited(ctx, start, 0, limit)) {
            // reconstruct via parent_move
            // build parent map implicitly by walking from goal to start
            vector<string> rev;
            string cur = goal;
            rev.push_back(cur);
            while (cur != start) {
                auto it = ctx.parent_move.find(cur);
                if (it == ctx.parent_move.end()) { rev.clear(); break; }
                cur = it->second.first;
                rev.push_back(cur);
            }
            reverse(rev.begin(), rev.end());

            // build move string
            string mv;
            for (size_t i = 1; i < rev.size(); ++i) {
                char step = ctx.parent_move[rev[i]].second;
                mv.push_back(step);
            }

            auto t1 = chrono::high_resolution_clock::now();
            return {true, rev, mv, ctx.nodes_expanded,
                    chrono::duration<double, milli>(t1 - t0).count()};
        }
    }

    auto t1 = chrono::high_resolution_clock::now();
    return {false, {}, "", 0, chrono::duration<double, milli>(t1 - t0).count()};
}

// ---------- Pretty Printing ----------
static void print_state(const string& s) {
    for (int i = 0; i < 9; ++i) {
        cout << (s[i] == '0' ? ' ' : s[i]) << ((i % 3 == 2) ? '\n' : ' ');
    }
}

static void print_path(const vector<string>& path) {
    for (size_t i = 0; i < path.size(); ++i) {
        cout << "Step " << i << ":\n";
        print_state(path[i]);
        if (i + 1 < path.size()) cout << "----\n";
    }
}

static void print_summary(const SearchResult& r) {
    if (!r.found) {
        cout << "No solution found.\n";
        cout << "Nodes expanded: " << r.nodes_expanded << "\n";
        cout << "Time (ms): " << fixed << setprecision(3) << r.millis << "\n";
        return;
    }
    cout << "Solution found!\n";
    cout << "Depth (moves): " << (r.path.empty() ? 0 : (int)r.path.size() - 1) << "\n";
    cout << "Moves: " << (r.moves.empty() ? "(none)" : r.moves) << "\n";
    cout << "Nodes expanded: " << r.nodes_expanded << "\n";
    cout << "Time (ms): " << fixed << setprecision(3) << r.millis << "\n\n";
    print_path(r.path);
}

// ---------- Main ----------
int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string mode = "bfs";
    string start, goal;

    // Parse CLI
    if (argc >= 2) {
        string m = argv[1];
        for (auto& ch : m) ch = (char)tolower(ch);
        if (m == "bfs" || m == "dfs") mode = m;
        else {
            cerr << "Unrecognized mode '" << argv[1] << "', defaulting to BFS.\n";
        }
    }

    if (argc >= 4) {
        start = argv[2];
        goal  = argv[3];
        start = strip_non_digits(start);
        goal  = strip_non_digits(goal);
    } else {
        cout << "Enter START (3x3 grid, digits 0..8; 0 = blank):\n";
        if (!read_grid_state(start)) {
            cerr << "Failed to read 9 digits for START.\n";
            return 1;
        }
        cout << "Enter GOAL (3x3 grid, digits 0..8; 0 = blank):\n";
        if (!read_grid_state(goal)) {
            cerr << "Failed to read 9 digits for GOAL.\n";
            return 1;
        }
    }

    if (!is_valid_perm(start) || !is_valid_perm(goal)) {
        cerr << "Invalid input. Ensure both states are permutations of 0..8 exactly once.\n";
        return 1;
    }

    if (!is_solvable(start, goal)) {
        cout << "This instance is UNSOLVABLE (inversion parity mismatch).\n";
        return 0;
    }

    if (mode == "bfs") {
        auto res = bfs_solve(start, goal);
        print_summary(res);
    } else {
        // DFS with iterative deepening; you can increase max depth if needed.
        auto res = iddfs_solve(start, goal, /*max_depth=*/60);
        print_summary(res);
    }

    return 0;
}
