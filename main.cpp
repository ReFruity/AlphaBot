#include <list>
#include <vector>
#include <climits>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <assert.h>

using namespace std;

enum Player {
    WhitePlayer,
    BlackPlayer
};

enum CellState {
    Empty,
    White,
    Black
};

struct Cell {
    int x, y;

    Cell(int x, int y) : x(x), y(y) { }

    Cell(char x, int y) : x(x - 'a'), y(y) { }

    Cell(const char *notation) : x(notation[0] - 'a'), y(notation[1] - '1') { }
};

std::ostream &operator<<(std::ostream &strm, Cell &cell) {
    return strm << 'a' + cell.x << cell.y + 1;
}

struct Move {
    Cell from, to;

    Move(const Cell &from, const Cell &to) : from(from), to(to) { }

    Move(const char *notation) : from(notation), to(notation + 2) { }
};

std::ostream &operator<<(std::ostream &strm, Move &move) {
    return strm << move.from << move.to;
}

struct Position {
    Player playerToMove;
    int movesMade;
    // [a-z][1-8]
    CellState boardState[8][8];
    // TODO: Remove debug variable
    static int entities;

    Position() {
        this->playerToMove = WhitePlayer;
        this->movesMade = 0;
        for (int y = 0; y < 2; y++) {
            fillY(y, White);
        }
        for (int y = 2; y < 6; y++) {
            fillY(y, Empty);
        }
        for (int y = 6; y < 8; y++) {
            fillY(y, Black);
        }
        entities++;
    }

    void fillY(int y, CellState cellState) {
        for (int x = 0; x < 8; x++) {
            boardState[x][y] = cellState;
        }
    }

    Position(const Position &other) {
        this->playerToMove = other.playerToMove;
        this->movesMade = other.movesMade;
        memcpy(this->boardState, other.boardState, 8 * 8);
        entities++;
    }

    ~Position() {
        entities--;
    }

    CellState getCellState(Cell cell) const {
        return boardState[cell.x][cell.y];
    }

    CellState getCellState(const char *notation) const {
        return getCellState(Cell(notation));
    }

    vector<Position> getChildPositions() const {
        auto childPositions = vector<Position>();
        CellState cellState = this->playerToMove == WhitePlayer ? White : Black;

        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                if (this->boardState[x][y] != cellState) continue;

                const vector<Move> &possibleMoves = this->getPossibleMoves(Cell(x, y));

                for (auto it = possibleMoves.begin(); it != possibleMoves.end(); it++) {
                    Position childPosition = Position(*this);
                    childPosition.makeMove(*it);
                    childPositions.push_back(childPosition);
                }
            }
        }

        return childPositions;
    }

    vector<Move> getPossibleMoves(Cell cell) const {
        CellState cellState = getCellState(cell);
        vector<Move> result;

        if (cellState == White) {
            if (cell.y == 7) return result;

            if (cell.x > 0 && boardState[cell.x - 1][cell.y + 1] != White) {
                result.push_back(Move(cell, Cell(cell.x - 1, cell.y + 1)));
            }

            if (boardState[cell.x][cell.y + 1] == Empty) {
                result.push_back(Move(cell, Cell(cell.x, cell.y + 1)));
            }

            if (cell.x < 7 && boardState[cell.x + 1][cell.y + 1] != White) {
                result.push_back(Move(cell, Cell(cell.x + 1, cell.y + 1)));
            }
        }

        if (cellState == Black) {
            if (cell.y == 0) return result;

            if (cell.x > 0 && boardState[cell.x - 1][cell.y - 1] != Black) {
                result.push_back(Move(cell, Cell(cell.x - 1, cell.y + 1)));
            }

            if (boardState[cell.x][cell.y - 1] == Empty) {
                result.push_back(Move(cell, Cell(cell.x, cell.y - 1)));
            }

            if (cell.x < 7 && boardState[cell.x + 1][cell.y - 1] != Black) {
                result.push_back(Move(cell, Cell(cell.x + 1, cell.y - 1)));
            }
        }

        return result;
    }

    vector<Move> getPossibleMoves(const char *notation) const {
        return getPossibleMoves(Cell(notation));
    }

    void makeMove(Move move) {
        boardState[move.to.x][move.to.y] = boardState[move.from.x][move.from.y];
        boardState[move.from.x][move.from.y] = Empty;
        playerToMove = playerToMove == WhitePlayer ? BlackPlayer : WhitePlayer;
        movesMade++;
    }

    void makeMove(const char *notation) {
        makeMove(Move(notation));
    }

    bool isFinal() const {
        return this->whiteWins() || this->blackWins();
    }

    bool whiteWins() const {
        for (int x = 0; x < 8; x++) {
            if (this->boardState[x][7] == White) return true;
        }

        return false;
    }

    bool blackWins() const {
        for (int x = 0; x < 8; x++) {
            if (this->boardState[x][0] == Black) return true;
        }

        return false;
    }

    int countScore() const {
        return this->whiteScore() - this->blackScore();
    }

    int whiteScore() const {
        for (int y = 7; y >= 0; y--) {
            for (int x = 0; x < 8; x++) {
                if (this->boardState[x][y] == White) {
                    return y;
                }
            }
        }

        return 0;
    }

    int blackScore() const {
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                if (this->boardState[x][y] == Black) {
                    return 7 - y;
                }
            }
        }

        return 0;
    }
};

int Position::entities = 0;

int alphaBetaPruning(const Position &position, int alpha, int beta, int depth);

const int NEGATIVE_INF = INT_MIN, POSITIVE_INF = INT_MAX;

bool debug = false;

void test() {
    Position position = Position();
    position.makeMove("e2e4");
    assert(position.getCellState("e2") == Empty);
    assert(position.getCellState("e4") == White);
    position.makeMove("e7f8");
    assert(position.getCellState("e7") == Empty);
    assert(position.getCellState("f8") == Black);

    assert(Position().getPossibleMoves("e2").size() == 3);
    assert(Position().getPossibleMoves("a2").size() == 2);
    assert(Position().getPossibleMoves("a1").size() == 0);

    auto actualChildPositions = Position().getChildPositions();
    assert(actualChildPositions.size() == 22);
    assert(all_of(actualChildPositions.begin(), actualChildPositions.end(), [](Position p) { return p.playerToMove == BlackPlayer; }));
    assert(all_of(actualChildPositions.begin(), actualChildPositions.end(), [](Position p) { return p.movesMade == 1; }));

    position = Position();
    position.makeMove("e2e8");
    assert(position.whiteWins());

    position = Position();
    position.makeMove("e7e1");
    assert(position.blackWins());

    position = Position();
    Position copy = Position(position);
    copy.makeMove("e2e4");
    assert(position.getCellState("e2") == White);
    assert(copy.getCellState("e2") == Empty);

    cout << "All tests passed." << endl;
}

int main(int argc, char *argv[]) {
    debug = argc == 2 && strcmp(argv[1], "debug") == 0;

    if (debug) {
        test();
        freopen("input.txt", "r", stdin);
        freopen("output.txt", "w", stdout);
    }

    const int MAX_DEPTH = 10;
    Position startingPosition = Position();
    cout << alphaBetaPruning(startingPosition, NEGATIVE_INF, POSITIVE_INF, MAX_DEPTH) << endl;
    cout << Position::entities << endl;

    return 0;
}

int alphaBetaPruning(const Position &position, int alpha, int beta, int depth) {
    if (depth == 0 || position.isFinal()) {
        return position.countScore();
    }

    auto childPositions = position.getChildPositions();
    int score;

    if (position.playerToMove == WhitePlayer) {
        score = NEGATIVE_INF;
        for (auto it = childPositions.begin(); it != childPositions.end(); it++) {
            score = max(score, alphaBetaPruning(*it, alpha, beta, depth - 1));
            alpha = max(alpha, score);
            if (beta <= alpha) {
                // TODO: Debug prints
                if (debug) {
                    cout << "beta pruning " << childPositions.size() << endl;
                }
                // Бета отсечение
                break;
            }
        }
    }
    else {
        score = POSITIVE_INF;
        for (auto it = childPositions.begin(); it != childPositions.end(); it++) {
            score = min(score, alphaBetaPruning(*it, alpha, beta, depth - 1));
            beta = min(beta, score);
            if (beta <= alpha) {
                if (debug) {
                    cout << "alpha pruning " << childPositions.size() << endl;
                }
                // Альфа отсечение
                break;
            }
        }
    }

    return score;
}