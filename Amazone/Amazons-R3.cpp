/*
优化了形势判断方式，强化相对占领的得分
对搜索进行剪枝，在前中期布局阶段减少非阻拦性行为的搜索
*/

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

// 8 directions
int dr[8] = {1,-1,0,0,1,1,-1,-1};
int dc[8] = {0,0,1,-1,1,-1,1,-1};

int step;
int maxGrade, minOpponent, maxPreGrade;//minimax algorithm, pre for cutting branch

Position bestFrom, bestTo, bestArrow;

char turn = 'B';
char Cpu, Opn;
char board[SIZE][SIZE];

bool hinting, finishStep;


// ---------------------------------------
// Make preparation for the board
// ---------------------------------------
void prepare() {
    // Initialize empty
    for (int r=0; r<SIZE; r++)
        for (int c=0; c<SIZE; c++)
            board[r][c] = '.';

    // Initial positions (your version)
    board[2][0] = 'B'; // a3
    board[0][2] = 'B'; // c1
    board[0][5] = 'B'; // f1
    board[2][7] = 'B'; // h3

    board[5][0] = 'W'; // a6
    board[7][2] = 'W'; // c8
    board[7][5] = 'W'; // f8
    board[5][7] = 'W'; // h6
}

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
                        // for (int d2=0; d2<8; d2++) {
                        //     int ar = nr + dr[d2], ac = nc + dc[d2];
                        //     while (inBounds(ar,ac) && board[ar][ac]=='.') {
                        //         return true; // found legal move
                        //         ar += dr[d2];
                        //         ac += dc[d2];
                        //     }
                        // }
                        // nr += dr[d];
                        // nc += dc[d];
                        return true;
                    }
                }
            }
    return false;
}

// -----------------------------------------------------
// Search the closest road from 'from' to a target chess
// 
// int Search(Position from,char aim) {
//     int sta = 1, en = 1;
//     int lr[100], lc[100], ld[100];
//     bool check[SIZE][SIZE];
//     Position to,now;
//     for(int r=0; r<SIZE; r++) {
//         for(int c=0; c<SIZE; c++)
//             check[r][c] = 0;
//     }
//     lr[1] = from.r, lc[1] = from.c, ld[1] = 0, check[from.r][from.c] = 1;   
//     while(sta <= en) {
//         now.r = lr[sta];
//         now.c = lc[sta];
//         for(int r=0; r<SIZE; r++) {
//             for(int c=0; c<SIZE; c++) {
//                 to.r = r;
//                 to.c = c;
//                 if(board[to.r][to.c] == aim) {
//                     board[to.r][to.c] = '.';
//                     if(validArrow(now, to)) {
//                         board[to.r][to.c] = aim;
//                         return ld[sta]+1;
//                     }
//                     board[to.r][to.c] = aim;
//                 }
//                 if(validArrow(now, to) && !check[r][c]) {
//                     ld[++en] = ld[sta]+1;
//                     lr[en] = to.r;
//                     lc[en] = to.c;
//                     check[to.r][to.c] = 1;                   
//                 }
//             }
//         }
//         sta++;
//     }
//     //cout<<en<<endl;
//     return 10;
// }
//-----------------------------------------------------

// -----------------------------------------------------
// Search the closest road from 'from' to a target chess
// -----------------------------------------------------
int Search(Position from,char aim) {
    int sta = 1, en = 1;
    int lr[100], lc[100], ld[100];
    bool check[SIZE][SIZE];
    Position to,now;

    for(int r=0; r<SIZE; r++) {
        for(int c=0; c<SIZE; c++)
            check[r][c] = 0;
    }
    lr[1] = from.r, lc[1] = from.c, ld[1] = 0, check[from.r][from.c] = 1;
    
    while(sta <= en) {
        now.r = lr[sta];
        now.c = lc[sta];
        for(int r=0; r<SIZE; r++) {
            for(int c=0; c<SIZE; c++) {
                to.r = r;
                to.c = c;

                if(board[to.r][to.c] == aim) {
                    board[to.r][to.c] = '.';
                    if(validArrow(now, to)) {
                        board[to.r][to.c] = aim;
                        return ld[sta]+1;
                    }
                    board[to.r][to.c] = aim;
                }

                if(validArrow(now, to) && !check[r][c]) {
                    ld[++en] = ld[sta]+1;
                    lr[en] = to.r;
                    lc[en] = to.c;
                    check[to.r][to.c] = 1;
                    
                }
            }
        }
        sta++;
    }
    //cout<<en<<endl;
    return 10;
}

// ---------------------------------------
// Evaluate contemporary situation
// ---------------------------------------
int Evaluate() {
    int GradeB = 0, GradeW = 0;
    int stepB, stepW;
    Position from;

    for(int r=0; r<SIZE; r++) {
        for(int c=0; c<SIZE; c++) {
            if(board[r][c] != '.')continue;
            from.r = r;
            from.c = c;

            stepB = Search(from,'B');
            stepW = Search(from,'W');

            //the steps it costs
            GradeB+= -stepB;
            GradeW+= -stepW;

            //absolutely conquer
            if(stepB == 10 && stepW == 10) continue;
            if(stepB == 10) {
                GradeW += 50;
                continue;
            }
            if(stepW == 10) {
                GradeB += 50;
                continue;
            }

            //relatively conquer
            if(stepB - stepW > 0) GradeW+= 15;
            if(stepW - stepB > 0) GradeB+= 15;

            //partly conquer
            if(stepB - stepW > 1) GradeW+= 15;
            if(stepW - stepB > 1) GradeB+= 15;

            //mostly conquer
            if(stepB - stepW > 2) GradeW+= 10;
            if(stepW - stepB > 2) GradeB+= 10;

            

            // if(checker) {
            //     //cout<<r<<" "<<c<<" "<<stepB<<" "<<stepW<<" "<<endl;
            // }
        }
    }
    //cout<<GradeB<<" "<<GradeW<<endl;
    return Cpu == 'B' ?  GradeB - GradeW : GradeW - GradeB;
}

// ---------------------------------------
// Find Opn's arrow
// ---------------------------------------
void arrowForOpponent(Position from, Position to) {
    Position arrow;
    int Grade;

    for(int r=0; r<SIZE; r++) {
        for(int c=0; c<SIZE; c++) {
            arrow.r = r;
            arrow.c = c;
            if(validArrow(to, arrow)) {
                // if(Search(arrow, Cpu)>1) {
                //     Grade = maxPreGrade;
                // }
                //else {
                    board[arrow.r][arrow.c] = 'X';
                    Grade = Evaluate();
                    board[arrow.r][arrow.c] = '.';
                //}
                
                if(Grade < minOpponent) {
                    minOpponent = Grade;
                }
                
            }
        }
    }
}

// ---------------------------------------
// Find Opn's move
// ---------------------------------------
void moveForOpponent(Position from) {
    Position to;

    for(int r=0; r<SIZE; r++) {
        for(int c=0; c<SIZE; c++) {
            to.r = r;
            to.c = c;
            if(validMove(from, to, Opn)) {
                board[from.r][from.c] = '.';
                board[r][c] = Opn;

                arrowForOpponent(from, to);

                board[r][c] = '.';
                board[from.r][from.c] = Opn;
            }
        }
    }
}

// ---------------------------------------
// Calculate the next step for Opn
// ---------------------------------------
int findForOpponent() {
    Position from;
    minOpponent = 99999;
    
    for(int r=0; r<SIZE; r++) {
        for(int c=0; c<SIZE; c++) {
            if(board[r][c] == Opn) {
                from.r = r;
                from.c = c;
                moveForOpponent(from);
            }
        }
    }
    return minOpponent;
}

// ---------------------------------------
// Search the best arrow for Cpu
// ---------------------------------------
void searchArrow(Position from, Position to) {
    Position arrow;
    int Grade, preGrade;

    for(int r=0; r<SIZE; r++)   {
        for(int c=0; c<SIZE; c++) {
            arrow.r = r;
            arrow.c = c;

            if(validArrow(to, arrow)) {
                //if(Search(arrow, Opn) > 1 && step <= 60) continue;

                board[arrow.r][arrow.c] = 'X';
                //if(checker1(from, to, arrow))checker = 1;
                preGrade = Evaluate();
                //cout<<from.r<<" "<<from.c<<" "<<to.r<<" "<<to.c<<" "<<arrow.r<<" "<<arrow.c<<" ";
                //cout<< preGrade <<" "<< maxPreGrade <<endl;
                if(maxPreGrade - preGrade > 60) {
                    board[arrow.r][arrow.c] = '.';
                    continue;
                }
                if(preGrade > maxPreGrade) maxPreGrade = preGrade;

                Grade = findForOpponent();
                //cout<< Grade <<"  "<< maxGrade <<endl;
                //checker = 0;
                //cout<<" "<<Grade<<endl;
                if(Grade > maxGrade) {
                    maxGrade = Grade;
                    bestFrom = from, bestTo = to, bestArrow = arrow;
                    //cout<<"find"<<endl;
                }

                board[arrow.r][arrow.c] = '.';
            }
        }
    }
}

// ---------------------------------------
// Search the best move for Cpu
// ---------------------------------------
void searchMove(Position from) {
    Position to;

    for(int r=0; r<SIZE; r++) {
        for(int c=0; c<SIZE; c++) {
            to.r = r;
            to.c = c;

            if(validMove(from, to, Cpu)) {
                board[from.r][from.c] = '.';
                board[to.r][to.c] = Cpu;

                searchArrow(from, to);

                board[from.r][from.c] = Cpu;
                board[to.r][to.c] = '.';
            }
        }
    }
}

// ---------------------------------------
// Find the best choice for Cpu
// ---------------------------------------
void findTheBest() {
    Position from;
    maxGrade = -99999, maxPreGrade = -99999;

    for(int r=0; r<SIZE; r++) {
        for(int c=0; c<SIZE; c++) {
            if(board[r][c] == Cpu) {
                from.r = r;
                from.c = c;
                searchMove(from);
            }
        }
    }
}

// ---------------------------------------
// Print the movement of Cpu
// ---------------------------------------
void outPut() {
    //cout<<"OutPut"<<endl;
    char output1[3], output2[3], output3[3];
    output1[0] = bestFrom.c + 'a', output1[1] = bestFrom.r + '1', output1[2] = '\0';
    output2[0] = bestTo.c + 'a', output2[1] = bestTo.r + '1', output2[2] = '\0';
    output3[0] = bestArrow.c + 'a', output3[1] = bestArrow.r + '1', output3[2] = '\0';
    if(!hinting) {
        printf("\nTurn %s\n", turn == 'W'? "White" : "Black");
        printf("Your opponent choose to move from %s to %s, with an arrow at %s\n", output1, output2, output3);
    }
    else {
        printf("%s %s %s might be a good choice.\n\n", output1, output2, output3);
        return;
    }

    board[bestFrom.r][bestFrom.c] = '.';
    board[bestTo.r][bestTo.c] = Cpu;
    board[bestArrow.r][bestArrow.c] = 'X';
}

// ---------------------------------------
// Player's turn
// ---------------------------------------
void turnForPlayer() {
    printf("\nTurn: %s\n", turn=='W' ? "White" : "Black");
    printf("Enter your move (FROM TO ARROW), e.g. c1 c5 c7\n");
    printf("or ask for a hint by cin : 'a hint needed'\n\n");
            
    char input1[10], input2[10], input3[10];
    scanf("%s %s %s", input1, input2, input3);
    if(input3[0] == 'n') {
        Cpu = turn;
        Opn = Cpu == 'B' ? 'W' : 'B';
        hinting = 1;

        findTheBest();
        outPut();

        swap(Cpu, Opn);
        //hinting = 0;
        return;
    }

    Position f = parsePos(input1);
    Position t = parsePos(input2);
    Position a = parsePos(input3);

    if (!validMove(f,t,turn)) {
        printf("Invalid move!\n");
        finishStep = 0;
        return;
    }

    board[f.r][f.c] = '.';
    board[t.r][t.c] = turn;

    if (!validArrow(t,a)) {
        printf("Invalid arrow!\n");
        board[f.r][f.c] = turn;
        board[t.r][t.c] = '.';
        finishStep = 0;
        return;
    }

    finishStep = 1;
    board[a.r][a.c] = 'X';  
}

// ---------------------------------------
// PVP Game for mode 1
// ---------------------------------------
void playPVP() {
    while (true) {
        if(!hinting) printBoard();
        else hinting = 0;
        turnForPlayer();
        if(!finishStep) continue;

        // Check game end
        char next = (turn=='W' ? 'B':'W');
        if (!hasAnyMove(next)) {
            printBoard();
            printf("\nGame Over! %s wins!\n", turn=='W'?"White":"Black");
            break;
        }
        if(!hinting) {
            step++;
            turn = next;
        }
        //else hinting = 0;
    }
}

// ---------------------------------------
// PVE Game for mode 2 or 3
// ---------------------------------------
void playPVE() {
    Opn = Cpu == 'B' ? 'W' : 'B';
    
    while(true) {
        if(!hinting) printBoard();
        else hinting = 0;
        //Player's turn
        if(turn != Cpu) {
            turnForPlayer();
            if(!finishStep) continue;
        }
        //Computer's turn
        else {
            findTheBest();
            outPut();
        }
        // Check game end
        char next = (turn=='W' ? 'B':'W');
        if (!hasAnyMove(next)) {
            printBoard();
            printf("\nGame Over! %s wins!\n", turn=='W'?"White":"Black");
            break;
        }
        if(!hinting) {
            step++;
            turn = next;
        }
        else hinting = 0;
    }
}

// ----------------------------------------------------------------------
// Play
// ----------------------------------------------------------------------
void play()
{
    int modechoose;
    printf("Please choose your mode\nmode 1 : PVP (You play as both black & white)\nmode 2 : PVE (You play as White)\nmode 3 : PVE (You play as Black)\n\n");
    scanf("%d", &modechoose);
    if(modechoose == 1) {
        printf("You choosed mode 1 : PVP\nYou play as both Black & White\n");
        playPVP();
    }
    
    else if(modechoose == 2) {
        printf("You choosed mode 2 : PVE\nYou play as Black\n");
        Cpu='W';
        playPVE();
    }
    else if(modechoose == 3) {
        printf("You choosed mode 3 : PVE\nYou play as White\n");
        Cpu='B';
        playPVE();
    }
    else {
        printf("More modes waiting to be explore\n");
        play();
    }
}

// ----------------------------------------------------------------------
// MAIN
// ----------------------------------------------------------------------
int main() {
    prepare();
    play();
    return 0;
}
