/*
下一步目标：如何将绝对占优近似视作绝对占有，实现多棋子联合布控 /kingstep & queenstep
实现了把所有step1搜完，再通过比较step1的局面决定是否在继续找step2，耗时可通过对step1情况数的筛选来调节
将全局找valid优化成直线找valid
*/
#include<fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include<iostream>
#include<algorithm>
#include<math.h>

#define SIZE 8

using namespace std;

struct Position {
    int r, c;
}chess[2][4];
struct Step {
    Position f, t, a;
    int g;
}aiChoice[9999];

// 8 directions
int dr[8] = {1,-1,0,0,1,1,-1,-1};
int dc[8] = {0,0,1,-1,1,-1,1,-1};

int step, modechoose, numGo;
int maxGrade, minOpponent;//minimax algorithm, pre for cutting branch
int ldB[64], ldW[64], lrB[64], lrW[64], lcB[64], lcW[64], disQB[SIZE][SIZE], disQW[SIZE][SIZE], disKB[SIZE][SIZE], disKW[SIZE][SIZE];//for search

Position bestFrom, bestTo, bestArrow;//find the best solution
Position stepFrom[64], stepTo[64], stepArrow[64];//document

char turn = 'B';
char Cpu, Opn;
char board[SIZE][SIZE];

bool hinting, finishStep;

bool hasSave() {
    ifstream fin("save.dat");
    bool ok = fin.is_open();
    fin.close();
    return ok;
}

void saveGame() {
    ofstream fout("save.dat");
    if (!fout.is_open()) {
        cout << "Save failed.\n";
        return;
    }

    fout << modechoose << '\n';
    fout << turn << '\n';
    fout << Cpu << ' ' << Opn << '\n';
    fout << step << '\n';

    // board
    for(int r=0;r<SIZE;r++){
        for(int c=0;c<SIZE;c++)
            fout << board[r][c];
        fout << '\n';
    }

    // chess positions
    for(int i=0;i<2;i++)
        for(int j=0;j<4;j++)
            fout << chess[i][j].r << ' ' << chess[i][j].c << '\n';

    // step records
    for(int i=0;i<step;i++){
        fout << stepFrom[i].r << ' ' << stepFrom[i].c << ' '
             << stepTo[i].r   << ' ' << stepTo[i].c   << ' '
             << stepArrow[i].r<< ' ' << stepArrow[i].c << '\n';
    }

    fout.close();
    cout << "Game saved successfully.\n";
}

bool loadGame() {
    ifstream fin("save.dat");
    if (!fin.is_open()) return false;

    fin >> modechoose;
    fin >> turn;
    fin >> Cpu >> Opn;
    fin >> step;

    for(int r=0;r<SIZE;r++)
        for(int c=0;c<SIZE;c++)
            fin >> board[r][c];

    for(int i=0;i<2;i++)
        for(int j=0;j<4;j++)
            fin >> chess[i][j].r >> chess[i][j].c;

    for(int i=0;i<step;i++){
        fin >> stepFrom[i].r >> stepFrom[i].c
            >> stepTo[i].r   >> stepTo[i].c
            >> stepArrow[i].r>> stepArrow[i].c;
    }

    fin.close();
    return true;
}


// ---------------------------------------
// Make preparation for the board
// ---------------------------------------
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
                    while (inBounds(nr,nc) && board[nr][nc] == '.') {
                        return true;
                    }
                }
            }
    saveGame();
    return false;
}

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

// ---------------------------------------
// Search the coverage for both colors
// ---------------------------------------
void QueenB() {
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

void QueenW() {
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

void KingB() {
    Position now, to;
    bool check[SIZE][SIZE];
    int staB = 1, endB = 0;

    for(int r=0; r<SIZE; r++) {
        for(int c=0; c<SIZE; c++) {
            check[r][c] = 0;
            disKB[r][c] = 99;
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
            if(validMove(now, to, 'B') && !check[to.r][to.c]) {
                
                ldB[++endB] = ldB[staB] + 1;
                lrB[endB] = to.r;
                lcB[endB] = to.c;
                check[to.r][to.c] = 1;

                to.r += dr[i];
                to.c += dc[i];
            }
        }
        
        disKB[now.r][now.c] = ldB[staB];
        if(staB > 4) board[now.r][now.c] = '.';
        staB++;
    }
}

void KingW() {
    Position now, to;
    bool check[SIZE][SIZE];
    int endW = 0, staW = 1;

    for(int r=0; r<SIZE; r++) {
        for(int c=0; c<SIZE; c++) {
            check[r][c] = 0;
            disKW[r][c] = 99;
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
            while(validMove(now, to, 'W') && !check[to.r][to.c]) {
                
                ldW[++endW] = ldW[staW] + 1;
                lrW[endW] = to.r;
                lcW[endW] = to.c;
                check[to.r][to.c] = 1;

                to.r += dr[i];
                to.c += dc[i];
            }
        }
        
        disKW[now.r][now.c] = ldW[staW];
        if(staW > 4)board[now.r][now.c] = '.';
        staW++;
    }
}

void searchQueen() {
    QueenW();
    QueenB();
}

void searchKing() {
    KingB();
    KingW();
}

// ---------------------------------------
// Evaluate contemporary situation
// ---------------------------------------
int Evaluate() {
    int GradeB = 0, GradeW = 0;

    searchQueen();
    //searchKing();
    
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
    for(int i=1; i<=100 && i<=numGo; i++) {

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

// ---------------------------------------
// One step regret
// ---------------------------------------
void Regret() {
    int cutstep = modechoose == 1 ? 1 : 2, nowTurn;
    if(cutstep > step) {
        printf("No step for regretting\n\n");
        return;
    }
    while(cutstep--) {
        
        step--;
        turn = turn == 'B' ? 'W' : 'B';

        nowTurn = turn == 'B' ? 0 : 1;

        for(int i=0;i<4;i++) {
            if(stepTo[step].r == chess[nowTurn][i].r && stepTo[step].c == chess[nowTurn][i].c) {
                chess[nowTurn][i].r = stepFrom[step].r;
                chess[nowTurn][i].c = stepFrom[step].c;
                break;
            }
        }
        
        board[stepArrow[step].r][stepArrow[step].c] = '.';
        board[stepTo[step].r][stepTo[step].c] = '.';
        board[stepFrom[step].r][stepFrom[step].c] = turn;
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

    stepFrom[step] = bestFrom;
    stepTo[step] = bestTo;
    stepArrow[step] = bestArrow;

    int nowTurn = turn == 'B' ? 0 : 1;
    for(int i=0; i<4; i++) {
        if(bestFrom.r == chess[nowTurn][i].r && bestFrom.c == chess[nowTurn][i].c) {
            chess[nowTurn][i].r = bestTo.r;
            chess[nowTurn][i].c = bestTo.c;
        }   
    }
    for(int i=0; i<2; i++) {
        for(int j=0; j<4; j++) {
            cout<<chess[i][j].r<<" "<<chess[i][j].c<<endl;
        }
    }
}

// ---------------------------------------
// Check the step record
// ---------------------------------------
void docuOutPut() {
    printf("\n%d\n",step);
    for(int i=0; i<step;i++) {
        printf("%d %d %d %d %d %d\n",stepFrom[i].r,stepFrom[i].c,stepTo[i].r,stepTo[i].c,stepArrow[i].r,stepArrow[i].c);
    }
    printf("\n");
}

// ---------------------------------------
// Player's turn
// ---------------------------------------
void turnForPlayer() {
    printf("\nTurn: %s\n", turn == 'W' ? "White" : "Black");
    printf("Enter your move (FROM TO ARROW), e.g. c1 c5 c7\n");
    printf("You can ask for a hint by cin : 'a hint needed', or make a regretion by cin : 'regret a step'\n\n");
            
    char input1[10], input2[10], input3[10];
    scanf("%s %s %s", input1, input2, input3);

    // 保存棋局：输入 "save the game"
    if (input1[0]=='s' && input2[0]=='t' && input3[0]=='g') {
        saveGame();
        finishStep = 0;
        return;
    }

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

    if(input3[0] == 'r')
    {
        docuOutPut();
        hinting = 1;
        finishStep = 0;
        return;
    }
    
    if(input3[0] == 's')
    {
        Regret();
        finishStep = 0;
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
    stepFrom[step] = f;
    stepTo[step] = t;
    stepArrow[step] = a;

    int nowTurn = turn == 'B' ? 0 : 1;
    for(int i=0; i<4; i++) {
        if(f.r == chess[nowTurn][i].r && f.c == chess[nowTurn][i].c) {
            chess[nowTurn][i].r = t.r;
            chess[nowTurn][i].c = t.c;
        }   
    }
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
            printf("\nGame Over! %s wins!\n\n", turn=='W'?"White":"Black");
            docuOutPut();
            break;
        }
        if(!hinting) {
            cout<<"hinting problem"<<endl;
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
        
        if(!hinting) {
            step++;
            turn = next;
        }
        else hinting = 0;
        if (!hasAnyMove(next)) {
            printBoard();
            printf("\nGame Over! %s wins!\n\n", turn=='W'?"White":"Black");
            docuOutPut();
            break;
        }
        
    }
}

// ----------------------------------------------------------------------
// Play
// ----------------------------------------------------------------------
void play()
{
    // printf("Amazons-R4\n\n");
    // printf("Please choose your mode\nmode 1 : PVP (You play as both black & white)\nmode 2 : PVE (You play as White)\nmode 3 : PVE (You play as Black)\n\n");
    // scanf("%d", &modechoose);
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
    cout << "Amazons-R5\n\n";

    if (hasSave()) {
        cout << "Detected a saved game.\n";
        cout << "1. Continue last game\n";
        cout << "2. Start a new game\n";
        cout << "Please choose (1 / 2): ";

        int choice;
        cin >> choice;

        if (choice == 1 && loadGame()) {
            cout << "Game loaded successfully.\n\n";
            play();      // 直接继续
            return 0;
        }
        // 选择 2 或 读档失败 → 新游戏
    }

    // 新游戏
    prepare();
    printf("Amazons-R4\n\n");
    printf("Please choose your mode\nmode 1 : PVP (You play as both black & white)\nmode 2 : PVE (You play as White)\nmode 3 : PVE (You play as Black)\n\n");
    scanf("%d", &modechoose);
    play();
    return 0;
}

