//equipped with Amazons-R4
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include<iostream>
#include<algorithm>

#define SIZE 8

using namespace std;

struct Position {
    int r, c;
};
struct Step {
    Position f, t, a;
    int g;
}aiChoice[9999];

// 8 directions
int dr[8] = {1,-1,0,0,1,1,-1,-1};
int dc[8] = {0,0,1,-1,1,-1,1,-1};//move

int step, turnID, numGo;//basic
int maxGrade, minOpponent, maxPreGrade;//to find the best
int staB, staW, endB, endW, ldB[64], ldW[64], lrB[64], lrW[64], lcB[64], lcW[64], disQB[SIZE][SIZE], disQW[SIZE][SIZE];//for search

Position bestFrom, bestTo, bestArrow,chess[2][4];

char turn = 'B';
char Cpu, Opn;
char board[SIZE][SIZE];

// -----------------------------------------------------
// Change the site of chess
// -----------------------------------------------------
void moveChess(Position f, Position t) {
    for(int i=0; i<2; i++) {
        for(int j=0; j<4; j++) {
            if(chess[i][j].r == f.r && chess[i][j].c == f.c) {
                chess[i][j].r = t.r;
                chess[i][j].c = t.c;
                return;
            }
        }
    }
}

void prepare() {
    // Initialize empty
    for (int r=0; r<SIZE; r++)
        for (int c=0; c<SIZE; c++)
            board[r][c] = '.';

    // Initial positions (your version)
    board[2][0] = 'B'; chess[0][0].r = 2, chess[0][0].c = 0;// a3
    board[0][2] = 'B'; chess[0][1].r = 0, chess[0][1].c = 2;// c1
    board[0][5] = 'B'; chess[0][2].r = 0, chess[0][2].c = 5;// f1
    board[2][7] = 'B'; chess[0][3].r = 2, chess[0][3].c = 7;// h3

    board[5][0] = 'W'; chess[1][0].r = 5, chess[1][0].c = 0;// a6
    board[7][2] = 'W'; chess[1][1].r = 7, chess[1][1].c = 2;// c8
    board[7][5] = 'W'; chess[1][2].r = 7, chess[1][2].c = 5;// f8
    board[5][7] = 'W'; chess[1][3].r = 5, chess[1][3].c = 7;// h6
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
// Check bounds
// ---------------------------------------
bool inBounds(int r, int c) {
    return r >= 0 && r < SIZE && c >= 0 && c < SIZE;
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

// ---------------------------------------
// Search the coverage for both colors
// ---------------------------------------
void searchB() {
    Position now, to;
    bool check[SIZE][SIZE];
    int staB = 1, endB = 0;

    for(int r=0; r<SIZE; r++) {
        for(int c=0; c<SIZE; c++) {
            check[r][c] = 0;
            disQB[r][c] = 99;
        }
    }

    for(int i=0; i<4; i++) {
        ldB[++endB] = 0;
        lrB[endB] = chess[0][i].r;
        lcB[endB] = chess[0][i].c;
        check[lrB[endB]][lcB[endB]] = 1;
    }

    while(staB <=endB) {
        now.r = lrB[staB];
        now.c = lcB[staB];
        board[now.r][now.c] = 'B'; 


        for(int i=0; i<8; i++) {
            to.r = lrB[staB] + dr[i];
            to.c = lcB[staB] + dc[i];
            while(validMove(now, to, 'B')) {
                if(check[to.r][to.c]) {
                    to.r += dr[i];
                    to.c += dc[i];
                    continue;
                }
                
                ldB[++endB] = ldB[staB] + 1;
                lrB[endB] = to.r;
                lcB[endB] = to.c;
                check[to.r][to.c] = 1;

                to.r += dr[i];
                to.c += dc[i];
            }
        }
        
        disQB[now.r][now.c] = ldB[staB];
        if(staB > 4) board[now.r][now.c] = '.';
        staB++;
    }
}

void searchW() {
    Position now, to;
    bool check[SIZE][SIZE];
    int endW = 0, staW = 1;

    for(int r=0; r<SIZE; r++) {
        for(int c=0; c<SIZE; c++) {
            check[r][c] = 0;
            disQW[r][c] = 99;
        }
    }

    for(int i=0; i<4; i++) {
        ldW[++endW] = 0;
        lrW[endW] = chess[1][i].r;
        lcW[endW] = chess[1][i].c;
        check[lrW[endW]][lcW[endW]] = 1;
    }

    while(staW <=endW) {

        now.r = lrW[staW];
        now.c = lcW[staW];
        board[now.r][now.c] = 'W';

        for(int i=0; i<8; i++) {
            to.r = lrW[staW] + dr[i];
            to.c = lcW[staW] + dc[i];
            while(validMove(now, to, 'W')) {
                if(check[to.r][to.c]) {
                    to.r += dr[i];
                    to.c += dc[i];
                    continue;
                }
                
                ldW[++endW] = ldW[staW] + 1;
                lrW[endW] = to.r;
                lcW[endW] = to.c;
                check[to.r][to.c] = 1;

                to.r += dr[i];
                to.c += dc[i];
            }
        }
        
        disQW[now.r][now.c] = ldW[staW];
        if(staW > 4)board[now.r][now.c] = '.';
        staW++;
    }
}


// ---------------------------------------
// Evaluate contemporary situation
// ---------------------------------------
int Evaluate() {
    int GradeB = 0, GradeW = 0;

    searchB();
    searchW();
    
    int disDelta;
    for(int r=0; r<SIZE; r++) {
        for(int c=0; c<SIZE; c++) {
            
            //disDelta = pow(disQB[r][c],4)*disKB[r][c] - pow(disQW[r][c],4)*disQW[r][c];
            disDelta = disQB[r][c]-disQW[r][c];

            //relatively conquer
            if(disDelta > 0) GradeW += 15;
            if(disDelta < 0) GradeB += 15;
            //partly conquer
            if(disDelta > 30) GradeW += 5;
            if(disDelta < -30) GradeB += 5;
            //mostly conquer
            if(disDelta > 100) GradeW += 2;
            if(disDelta < -100) GradeB += 2;
            //absolutely conquer
            if(disDelta > 500) GradeW += 1;
            if(disDelta < -500) GradeB += 1;
            
        }
    }    
    

    return Cpu == 'B' ?  GradeB - GradeW : GradeW - GradeB;
}

// ---------------------------------------
// Find Opn's arrow
// ---------------------------------------
void arrowForOpponent(Position from, Position to) {
    Position arrow;
    int Grade;

    for(int i=0; i<8; i++) {
        arrow.r = to.r + dr[i];
        arrow.c = to.c + dc[i];
        while(validArrow(to, arrow)) {
            board[arrow.r][arrow.c] = 'X';
            Grade = Evaluate();
            board[arrow.r][arrow.c] = '.';
                
            if(Grade < minOpponent) {
                minOpponent = Grade;
            }

            arrow.r += dr[i];
            arrow.c += dc[i];
        }
    }
}

// ---------------------------------------
// Find Opn's move
// ---------------------------------------
void moveForOpponent(Position from) {
    Position to;

    for(int i=0; i<8; i++) {
        to.r = from.r + dr[i];
        to.c = from.c + dc[i];
        while(validMove(from, to, Opn)) {
            board[from.r][from.c] = '.';
            board[to.r][to.c] = Opn;
            moveChess(from, to);

            arrowForOpponent(from, to);

            moveChess(to, from);
            board[from.r][from.c] = Opn;
            board[to.r][to.c] = '.';

            to.r += dr[i];
            to.c += dc[i];
        }
    }

}

// ---------------------------------------
// Calculate the next step for Opn
// ---------------------------------------
int findForOpponent() {
    Position from;
    minOpponent = 99999;
    int unTurn = Cpu == 'B' ? 1 : 0;
    
    for(int i=0; i<4; i++) {
        from.r = chess[unTurn][i].r;
        from.c = chess[unTurn][i].c;
        moveForOpponent(from);
    }
    
    return minOpponent;
}

// ---------------------------------------
// Soort the aiChoices
// ---------------------------------------
bool aiCmp(Step Go1, Step Go2) {
    if(Go1.g > Go2.g) return 1;
    return 0;
}

// ---------------------------------------
// Add an aiChoice
// ---------------------------------------
void add(Position from, Position to, Position arrow, int preGrade) {
    numGo++;
    aiChoice[numGo].f = from;
    aiChoice[numGo].t = to;
    aiChoice[numGo].a = arrow;
    aiChoice[numGo].g = preGrade;
}

// ---------------------------------------
// Search the best arrow for Cpu
// ---------------------------------------
void searchArrow(Position from, Position to) {
    Position arrow;
    int preGrade;

    for(int i=0; i<8; i++) {

        arrow.r = to.r + dr[i];
        arrow.c = to.c + dc[i];

        while(validArrow(to, arrow)) {

            board[arrow.r][arrow.c] = 'X';
            preGrade = Evaluate();

            add(from, to, arrow, preGrade);

            board[arrow.r][arrow.c] = '.';

            arrow.r += dr[i];
            arrow.c += dc[i];
        }
    }
}

// ---------------------------------------
// Search the best move for Cpu
// ---------------------------------------
void searchMove(Position from) {
    Position to;

    for(int i=0; i<8; i++) {
        to.r = from.r + dr[i];
        to.c = from.c + dc[i];
        while(validMove(from, to, Cpu)) {
            board[from.r][from.c] = '.';
            board[to.r][to.c] = Cpu;
            moveChess(from, to);

            searchArrow(from, to);

            moveChess(to, from);
            board[from.r][from.c] = Cpu;
            board[to.r][to.c] = '.';

            to.r += dr[i];
            to.c += dc[i];
        }
    }
}

// ---------------------------------------
// Find the best choice for Cpu
// ---------------------------------------
void findTheBest() {
    
    Position from, to, arrow;
    int Grade, nowTurn = Cpu == 'B' ? 0 : 1;
    maxGrade = -99999, numGo = 0;

    // for(int i=0; i<4; i++) {
    //     cout<<chess[nowTurn][i].r<<" "<<chess[nowTurn][i].c<<endl;
    // }

    for(int i=0; i<4; i++) {
        from.r = chess[nowTurn][i].r;
        from.c = chess[nowTurn][i].c;
        searchMove(from);
    }

    //cout << numGo <<endl;
    sort(aiChoice+1, aiChoice + numGo + 1, aiCmp);
    for(int i=1; i<=50 && i<=numGo; i++) {

        from = aiChoice[i].f;
        to = aiChoice[i].t;
        arrow = aiChoice[i].a;
        board[from.r][from.c] = '.'; 
        board[to.r][to.c] = Cpu;
        board[arrow.r][arrow.c] = 'X';
        
        moveChess(aiChoice[i].f, aiChoice[i].t);
        
        Grade = findForOpponent() + aiChoice[i].g;
        if(Grade > maxGrade)
        {
            bestFrom = from, bestTo = to, bestArrow = arrow;
            maxGrade = Grade;
        }

        board[arrow.r][arrow.c] = '.';
        board[to.r][to.c] = '.';
        board[from.r][from.c] = Cpu; 

        moveChess(aiChoice[i].t, aiChoice[i].f);

        //cout<<aiChoice[i].f.r<<" "<<aiChoice[i].f.c<<" "<<aiChoice[i].t.r<<" "<<aiChoice[i].t.c<<" "<<aiChoice[i].a.r<<" "<<aiChoice[i].a.c<<" "<<Grade<<endl;
    }
    
}




// ----------------------------------------------------------------------
// MAIN
// ----------------------------------------------------------------------
int main() {
    prepare();
    cin >> turnID;
    // 读入历史回合，恢复当前局面

    Cpu = 'W'; Opn = 'B'; // 默认自己是白方
    
    int x0,x1,x2,y0,y1,y2;
    for (int i = 0; i < turnID; i++) {
        // 对手的行动
        cin >> y0 >> x0 >> y1 >> x1 >> y2 >> x2;
        if (x0 == -1) Cpu = 'B', Opn = 'W'; // 第一回合是-1，说明我是黑方
        else {
            Position f, t, a;
            f.r = x0; f.c = y0;
            t.r = x1; t.c = y1;
            a.r = x2; a.c = y2;

            board[f.r][f.c] = '.';
            board[t.r][t.c] = Opn;
            board[a.r][a.c] = 'X';
            moveChess(f, t);
        }
        
        // 自己的行动
        if (i < turnID - 1) {
            cin >> y0 >> x0 >> y1 >> x1 >> y2 >> x2;
            if (x0 >= 0) {
                Position f, t, a;
                f.r = x0; f.c = y0;
                t.r = x1; t.c = y1;
                a.r = x2; a.c = y2;

                board[f.r][f.c] = '.';
                board[t.r][t.c] = Cpu;
                board[a.r][a.c] = 'X'; 
                moveChess(f, t);
            }
        }
    }
    turn = Cpu;
    //printBoard();
    // for(int i=0; i<2; i++) {
    //     for(int j=0; j<4; j++) {
    //         cout<<chess[i][j].r<<" "<<chess[i][j].c<<endl;
    //     }
    // }
    findTheBest();
    printf("%d %d %d %d %d %d\n",bestFrom.c,bestFrom.r,bestTo.c,bestTo.r,bestArrow.c,bestArrow.r);
    //printBoard();
    Position f, t, a;
                f.r = bestFrom.r; f.c = bestFrom.c;
                t.r = bestTo.r; t.c = bestTo.c;
                a.r = bestArrow.r; a.c = bestArrow.c;

                board[f.r][f.c] = '.';
                board[t.r][t.c] = turn;
                board[a.r][a.c] = 'X'; 
    //printBoard();
    return 0;
}
