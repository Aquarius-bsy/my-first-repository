#include <cstdlib>
#include <cstring>
#include<iostream>
#include<algorithm>

#define SIZE 8

using namespace std;

struct Position {
    int r, c;
};

// 8 directions
int dr[8] = {1,-1,0,0,1,1,-1,-1};
int dc[8] = {0,0,1,-1,1,-1,1,-1};
char input1[10], input2[10], input3[10];
char turn = 'W';
char board[SIZE][SIZE];

// ---------------------------------------
// Check bounds
// ---------------------------------------
bool inBounds(int r, int c) {
    return r >= 0 && r < SIZE && c >= 0 && c < SIZE;
}

// ---------------------------------------
// Convert input like "c5"
// ---------------------------------------
Position parsePos(const char* s) {
    Position p;
    p.c = s[0] - 'a';
    p.r = atoi(s+1) - 1;
    return p;
}

// ---------------------------------------
// Print ASCII board
// ---------------------------------------
void printBoard() {
    printf("\n       a   b   c   d   e   f   g   h\n");
    printf("     +---+---+---+---+---+---+---+---+\n");

    for (int r = 0; r < SIZE; r++) {
        printf("  %2d |", r + 1);  // row number

        for (int c = 0; c < SIZE; c++) {
            char ch = board[r][c];
            printf(" %c |", ch);
        }
        printf("\n");
        printf("     +---+---+---+---+---+---+---+---+\n");
    }
}


// ---------------------------------------
// Check straight or diagonal clear path
// ---------------------------------------
bool isClearPath(Position a, Position b) {
    int drow = b.r - a.r;
    int dcol = b.c - a.c;

    // Not straight or diagonal → invalid
    if (!(drow == 0 || dcol == 0 || abs(drow) == abs(dcol)))
        return false;

    int stepR = (drow == 0) ? 0 : (drow > 0 ? 1 : -1);
    int stepC = (dcol == 0) ? 0 : (dcol > 0 ? 1 : -1);

    int r = a.r + stepR;
    int c = a.c + stepC;

    while (r != b.r || c != b.c) {
        if (!inBounds(r,c)) return false;
        if (board[r][c] != '.') return false;
        r += stepR;
        c += stepC;
    }

    if (board[b.r][b.c] != '.') return false;
    return true;
}

// ---------------------------------------
// Move validation
// ---------------------------------------
bool validMove(Position from, Position to, char player) {
    if (!inBounds(from.r, from.c) || !inBounds(to.r,to.c)) return false;
    if (board[from.r][from.c] != player) return false;
    if (board[to.r][to.c] != '.') return false;
    return isClearPath(from,to);
}

// ---------------------------------------
// Arrow validation
// ---------------------------------------
bool validArrow(Position from, Position to) {
    if (!inBounds(to.r,to.c)) return false;
    if (board[to.r][to.c] != '.') return false;
    return isClearPath(from,to);
}

// -----------------------------------------------------
// Check if player has ANY legal move
// -----------------------------------------------------
bool hasAnyMove(char player) {
    for (int r=0; r<SIZE; r++)
        for (int c=0; c<SIZE; c++)
            if (board[r][c] == player) {

                for (int d = 0; d<8; d++) {
                    int nr = r + dr[d], nc = c + dc[d];
                    while (inBounds(nr,nc) && board[nr][nc]=='.') {
                        // Try arrow in ANY direction
                        for (int d2=0; d2<8; d2++) {
                            int ar = nr + dr[d2], ac = nc + dc[d2];
                            while (inBounds(ar,ac) && board[ar][ac]=='.') {
                                return true; // found legal move
                                ar += dr[d2];
                                ac += dc[d2];
                            }
                        }
                        nr += dr[d];
                        nc += dc[d];
                    }
                }
            }
    return false;
}

void playPVP()
{
    while (true) {
        printBoard();
        printf("\nTurn: %s\n", turn=='W' ? "White" : "Black");
        printf("Enter move (FROM TO ARROW), e.g. c1 c5 c7:\n");

        scanf("%s %s %s", input1, input2, input3);
        Position f = parsePos(input1);
        Position t = parsePos(input2);
        Position a = parsePos(input3);

        if (!validMove(f,t,turn)) {
            printf("Invalid move!\n");
            continue;
        }

        board[f.r][f.c] = '.';
        board[t.r][t.c] = turn;

        if (!validArrow(t,a)) {
            printf("Invalid arrow!\n");
            board[f.r][f.c] = turn;
            board[t.r][t.c] = '.';
            continue;
        }

        board[a.r][a.c] = 'X';

        // Check game end
        char next = (turn=='W' ? 'B':'W');
        if (!hasAnyMove(next)) {
            printBoard();
            printf("\nGame Over! %s wins!\n", turn=='W'?"White":"Black");
            break;
        }

        turn = next;
    }
}
// ----------------------------------------------------------------------
// MAIN
// ----------------------------------------------------------------------
int main() {

    // Initialize empty
    for (int r=0; r<SIZE; r++)
        for (int c=0; c<SIZE; c++)
            board[r][c] = '.';

    // Initial positions (your version)
    board[2][0] = 'W'; // a3
    board[0][2] = 'W'; // c1
    board[0][5] = 'W'; // f1
    board[2][7] = 'W'; // h3

    board[5][0] = 'B'; // a6
    board[7][2] = 'B'; // c8
    board[7][5] = 'B'; // f8
    board[5][7] = 'B'; // h6

    
    int modechoose;
    printf("Please choose your model:\n1.PVP\n2.PVE You play as White\n3.PVE You play as Black\n\n");
    scanf("%d", modechoose);
    if(modechoose == 1)
    playPVP();
    //else if(mood)
    

    return 0;
}