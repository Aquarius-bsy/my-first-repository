#include <iostream>
#include <vector>
#include <string>
#include <cctype>

using namespace std;

const int N = 8;
vector<vector<char>> board(N, vector<char>(N, '.'));

struct Pos {
    int r, c;
};

bool inBounds(int r, int c) {
    return r >= 0 && r < N && c >= 0 && c < N;
}

Pos parsePos(const string& s) {
    int c = tolower(s[0]) - 'a';
    int r = stoi(s.substr(1)) - 1;
    return {r, c};
}

void printBoard() {
    cout << "\n    a   b   c   d   e   f   g   h\n";
    cout << "  +---+---+---+---+---+---+---+---+\n";

    for (int r = 0; r < N; r++) {
        cout << r + 1 << " |";
        for (int c = 0; c < N; c++) {
            cout << " " << board[r][c] << " |";
        }
        cout << " " << r + 1 << "\n";
        cout << "  +---+---+---+---+---+---+---+---+\n";
    }

    cout << "    a   b   c   d   e   f   g   h\n\n";
}//understand

bool isClearPath(Pos a, Pos b) {
    int dr = b.r - a.r;
    int dc = b.c - a.c;

    if (dr != 0) dr = dr > 0 ? 1 : -1;
    if (dc != 0) dc = dc > 0 ? 1 : -1;

    if (a.r == b.r) dr = 0;
    if (a.c == b.c) dc = 0;

    if (abs(b.r - a.r) != abs(b.c - a.c) && dr != 0 && dc != 0)
        return false;

    int r = a.r + dr, c = a.c + dc;

    while (r != b.r || c != b.c) {
        if (!inBounds(r, c)) return false;
        if (board[r][c] != '.') return false;
        r += dr;
        c += dc;
    }

    if (!inBounds(b.r, b.c)) return false;
    if (board[b.r][b.c] != '.') return false;

    return true;
}

bool validMove(Pos from, Pos to, char player) {
    if (!inBounds(from.r, from.c) || !inBounds(to.r, to.c)) return false;
    if (board[from.r][from.c] != player) return false;
    if (board[to.r][to.c] != '.') return false;

    return isClearPath(from, to);
}

bool validArrow(Pos from, Pos to) {
    if (!inBounds(to.r, to.c)) return false;
    if (board[to.r][to.c] != '.') return false;

    return isClearPath(from, to);
}

// 检测某一方是否还有任何合法走法
bool hasAnyMove(char player) {
    int directions[8][2] = {
        {-1,0},{1,0},{0,-1},{0,1},
        {-1,-1},{-1,1},{1,-1},{1,1}
    };

    // 找到所有该玩家的亚马逊
    vector<Pos> amazons;
    for (int r = 0; r < N; r++)
        for (int c = 0; c < N; c++)
            if (board[r][c] == player)
                amazons.push_back({r,c});

    // 对每个亚马逊进行检查
    for (auto &A : amazons) {
        for (auto &d : directions) {
            int r = A.r + d[0], c = A.c + d[1];

            while (inBounds(r,c) && board[r][c]=='.') {

                // 临时移动棋子
                board[A.r][A.c] = '.';
                board[r][c] = player;

                // 对箭方向进行检测
                for (auto &d2 : directions) {
                    int ar = r + d2[0], ac = c + d2[1];
                    while (inBounds(ar,ac) && board[ar][ac]=='.') {
                        // 恢复棋盘
                        board[A.r][A.c] = player;
                        board[r][c] = '.';
                        return true; // 找到合法走法
                    }
                }

                // 恢复棋盘
                board[A.r][A.c] = player;
                board[r][c] = '.';

                r += d[0];
                c += d[1];
            }
        }
    }

    return false;
}

int main() {
    // Custom starting positions (8×8)
    vector<Pos> white = {{2, 0}, {0, 2}, {0, 5}, {2, 7}}; // a3 c1 f1 h3
    vector<Pos> black = {{5, 0}, {7, 2}, {7, 5}, {5, 7}}; // a6 c8 f8 h6

    for (auto& p : white) board[p.r][p.c] = 'W';
    for (auto& p : black) board[p.r][p.c] = 'B';

    string from, to, arrow;
    char turn = 'W';

    while (true) {
        printBoard();

        // 判断是否已经无路可走
        if (!hasAnyMove(turn)) {
            cout << "Game Over! ";
            cout << (turn == 'W' ? "White" : "Black") << " has no legal moves.\n";
            cout << (turn == 'W' ? "Black" : "White") << " wins!\n";
            break;
        }

        cout << "Turn: " << (turn=='W' ? "White" : "Black") << "\n";
        cout << "Enter move (FROM TO ARROW, e.g. c1 c5 c7):\n";

        cin >> from >> to >> arrow;

        Pos f = parsePos(from);
        Pos t = parsePos(to);
        Pos a = parsePos(arrow);

        if (!validMove(f, t, turn)) {
            cout << "Invalid move! Try again.\n";
            continue;
        }

        board[f.r][f.c] = '.';
        board[t.r][t.c] = turn;

        if (!validArrow(t, a)) {
            board[f.r][f.c] = turn;
            board[t.r][t.c] = '.';
            cout << "Invalid arrow! Try again.\n";
            continue;
        }

        board[a.r][a.c] = 'X';

        turn = (turn == 'W' ? 'B' : 'W');
    }

    return 0;
}
