#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BOARD_SIZE 8
#define MAX_MOVES (BOARD_SIZE*2*3)

enum {
    EMPTY, WHITE, BLACK
};

struct GameMove {
    int sx, sy;
    int ex, ey;
};

inline int op_color(int color) {
    return color == WHITE ? BLACK : WHITE;
}

char board[BOARD_SIZE][BOARD_SIZE];
int color;

void init() {
    memset(board, EMPTY, sizeof(board));
    for (int i = 0; i < BOARD_SIZE; ++i) {
        board[0][i] = board[1][i] = WHITE;
        board[BOARD_SIZE - 1][i] = board[BOARD_SIZE - 2][i] = BLACK;
    }
    color = WHITE;
}

void make_move(GameMove &move) {
    board[move.ex][move.ey] = board[move.sx][move.sy];
    board[move.sx][move.sy] = EMPTY;
    color = op_color(color);
}

void make_move(char *str) {
    char sy, ey;
    int sx, ex;
    sscanf(str, "%c%d%c%d", &sy, &sx, &ey, &ex);
    GameMove move = {sx - 1, sy - 'a', ex - 1, ey - 'a'};
    make_move(move);
}

int gen_moves(GameMove *moves) {
    int cnt = 0;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board[i][j] == color) {
                for (int dj = -1; dj <= 1; ++dj) {
                    int ni = i + (color == WHITE ? 1 : -1);
                    int nj = j + dj;
                    if (ni >= 0 && ni < BOARD_SIZE && nj >= 0 && nj < BOARD_SIZE &&
                        board[ni][nj] != color && (dj || board[ni][nj] == EMPTY)) {
                        GameMove move = {i, j, ni, nj};
                        moves[cnt++] = move;
                    }
                }
            }
        }
    }
    return cnt;
}

void think(char *str) {
    GameMove moves[MAX_MOVES];
    int cnt = gen_moves(moves);
    GameMove &move = moves[rand() % cnt];
    sprintf(str, "%c%d%c%d", move.sy + 'a', move.sx + 1, move.ey + 'a', move.ex + 1);
    make_move(move);
}

int main() {
    srand(1);
    init();
    for (; ;) {
        char str[100];
        gets(str);
        if (!strcmp(str, "Name")) {
            printf("AlphaBot\n");
            fflush(stdout);
        }
        else if (!strcmp(str, "Quit")) {
            break;
        }
        else {
            if (strcmp(str, "Start")) {
                make_move(str);
            }
            think(str);
            printf("%s\n", str);
            fflush(stdout);
        }
    }
    return 0;
}