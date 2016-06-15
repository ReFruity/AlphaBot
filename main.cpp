#include <vector>
#include <climits>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <cassert>
#include <ctime>

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

    Cell() { }

    Cell(int x, int y) : x(x), y(y) { }

    Cell(char x, int y) : x(x - 'a'), y(y) { }

    Cell(const char notation[2]) : x(notation[0] - 'a'), y(notation[1] - '1') { }
};

ostream &operator<<(ostream &strm, Cell &cell) {
    return strm << char('a' + cell.x) << cell.y + 1;
}

struct Move {
    Cell from, to;

    Move() { }

    Move(const Cell &from, const Cell &to) : from(from), to(to) { }

    Move(const char notation[4]) : from(notation), to(notation + 2) { }
};

ostream &operator<<(ostream &strm, Move &move) {
    return strm << move.from << move.to;
}

struct Position {
    Player playerToMove;
    int moveNumber;
    // [a-z][1-8]
    CellState boardState[8][8];

    Position() {
        this->playerToMove = WhitePlayer;
        this->moveNumber = 1;
        for (int y = 0; y < 2; y++) {
            fillY(y, White);
        }
        for (int y = 2; y < 6; y++) {
            fillY(y, Empty);
        }
        for (int y = 6; y < 8; y++) {
            fillY(y, Black);
        }
    }

    void fillY(int y, CellState cellState) {
        for (int x = 0; x < 8; x++) {
            boardState[x][y] = cellState;
        }
    }

    Position(const Position &other) {
        this->playerToMove = other.playerToMove;
        this->moveNumber = other.moveNumber;
        memcpy(this->boardState, other.boardState, sizeof(CellState) * 8 * 8);
    }

    CellState getCellState(Cell cell) const {
        return boardState[cell.x][cell.y];
    }

    CellState getCellState(const char notation[2]) const {
        return getCellState(Cell(notation));
    }

    vector<Position> getChildPositions() const {
        auto result = vector<Position>();
        const vector<Move> allPossibleMoves = this->getAllPossibleMoves();

        for (auto it = allPossibleMoves.begin(); it != allPossibleMoves.end(); it++) {
            Position childPosition = Position(*this);
            childPosition.makeMove(*it);
            result.push_back(childPosition);
        }

        return result;
    }

    vector<Move> getAllPossibleMoves() const {
        auto result = vector<Move>();
        CellState cellState = this->playerToMove == WhitePlayer ? White : Black;

        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                if (this->boardState[x][y] != cellState) continue;

                const vector<Move> &possibleMoves = this->getPossibleMoves(Cell(x, y));
                result.insert(result.end(), possibleMoves.begin(), possibleMoves.end());
            }
        }

        return result;
    }

    vector<Move> getPossibleMoves(const Cell &cell) const {
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
                result.push_back(Move(cell, Cell(cell.x - 1, cell.y - 1)));
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

    vector<Move> getPossibleMoves(const char notation[2]) const {
        return getPossibleMoves(Cell(notation));
    }

    void makeMove(Move move) {
        boardState[move.to.x][move.to.y] = boardState[move.from.x][move.from.y];
        boardState[move.from.x][move.from.y] = Empty;
        playerToMove = playerToMove == WhitePlayer ? BlackPlayer : WhitePlayer;
        if (playerToMove == WhitePlayer) moveNumber++;
    }

    void makeMove(const char notation[2]) {
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

    int score() const {
        return p1() + p2() + winScore();
    }

    int p1() const {
        int result = 0;

        for (int y = 7; y >= 0; y--) {
            for (int x = 0; x < 8; x++) {
                if (this->boardState[x][y] == White) {
                    result++;
                }
                if (this->boardState[x][y] == Black) {
                    result--;
                }
            }
        }

        return result;
    }

    int p2() const {
        return whiteLeadingPawn() - blackLeadingPawn();
    }

    int whiteLeadingPawn() const {
        for (int y = 7; y >= 0; y--) {
            for (int x = 0; x < 8; x++) {
                if (this->boardState[x][y] == White) {
                    return y;
                }
            }
        }

        return 0;
    }

    int blackLeadingPawn() const {
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                if (this->boardState[x][y] == Black) {
                    return 7 - y;
                }
            }
        }

        return 0;
    }

    int interceptors(const Cell &cell) const {
        return this->interceptors(cell.x, cell.y);
    }

    int interceptors(int x0, int y0) const {
        int result = 0;

        if (boardState[x0][y0] == White) {
            for (int y = y0 + 1; y <= 7; y++) {
                for (int x = max(x0 - (y - y0), 0); x <= min(x0 + (y - y0), 7); x++) {
                    if (boardState[x][y] == Black) {
                        result++;
                    }
                }
            }
        }
        else {
            for (int y = y0 - 1; y >= 0; y--) {
                for (int x = max(x0 - (y0 - y), 0); x <= min(x0 + (y0 - y), 7); x++) {
                    if (boardState[x][y] == White) {
                        result++;
                    }
                }
            }
        }

        return result;
    }

    int winScore() const {
        if (whiteWins()) return 1000;
        if (blackWins()) return -1000;
        return 0;
    }
};

int alphaBetaPruning(const Position &position, int alpha, int beta, int depth);

Move findBestMove(const Position &position, const int maxDepth);

const int NEGATIVE_INF = INT_MIN, POSITIVE_INF = INT_MAX;

clock_t deadline;

bool debug = false;

void test() {
    Position position = Position();
    position.makeMove("e2e3");
    assert(position.getCellState("e2") == Empty);
    assert(position.getCellState("e3") == White);
    position.makeMove("e7f8");
    assert(position.getCellState("e7") == Empty);
    assert(position.getCellState("f8") == Black);

    position = Position();
    assert(position.getPossibleMoves("e2").size() == 3);
    assert(position.getPossibleMoves("a2").size() == 2);
    assert(position.getPossibleMoves("h2").size() == 2);
    assert(position.getPossibleMoves("a1").size() == 0);

    assert(position.getPossibleMoves("e7").size() == 3);
    assert(position.getPossibleMoves("a7").size() == 2);
    assert(position.getPossibleMoves("h7").size() == 2);
    assert(position.getPossibleMoves("a8").size() == 0);

    auto actualChildPositions = position.getChildPositions();
    assert(actualChildPositions.size() == 22);
    assert(all_of(actualChildPositions.begin(), actualChildPositions.end(), [](Position p) { return p.playerToMove == BlackPlayer; }));
    assert(all_of(actualChildPositions.begin(), actualChildPositions.end(), [](Position p) { return p.moveNumber == 1; }));
    assert(actualChildPositions[0].getCellState("a2") == Empty);
    assert(actualChildPositions[0].getCellState("a3") == White);
    assert(actualChildPositions[1].getCellState("a2") == Empty);
    assert(actualChildPositions[1].getCellState("b3") == White);
    assert(actualChildPositions[20].getCellState("h2") == Empty);
    assert(actualChildPositions[20].getCellState("g3") == White);
    assert(actualChildPositions[21].getCellState("h2") == Empty);
    assert(actualChildPositions[21].getCellState("h3") == White);

    position.makeMove("e2e3");
    actualChildPositions = position.getChildPositions();
    assert(actualChildPositions.size() == 22);
    assert(all_of(actualChildPositions.begin(), actualChildPositions.end(), [](Position p) { return p.playerToMove == WhitePlayer; }));
    assert(all_of(actualChildPositions.begin(), actualChildPositions.end(), [](Position p) { return p.moveNumber == 2; }));
    assert(actualChildPositions[0].getCellState("a7") == Empty);
    assert(actualChildPositions[0].getCellState("a6") == Black);
    assert(actualChildPositions[1].getCellState("a7") == Empty);
    assert(actualChildPositions[1].getCellState("b6") == Black);
    assert(actualChildPositions[20].getCellState("h7") == Empty);
    assert(actualChildPositions[20].getCellState("g6") == Black);
    assert(actualChildPositions[21].getCellState("h7") == Empty);
    assert(actualChildPositions[21].getCellState("h6") == Black);

    position.makeMove("e7e6");
    actualChildPositions = position.getChildPositions();
    assert(actualChildPositions.size() == 23);
    assert(all_of(actualChildPositions.begin(), actualChildPositions.end(), [](Position p) { return p.playerToMove == BlackPlayer; }));
    assert(all_of(actualChildPositions.begin(), actualChildPositions.end(), [](Position p) { return p.moveNumber == 2; }));

    position = Position();
    position.makeMove("e2e8");
    assert(position.whiteWins());

    position = Position();
    position.makeMove("e7e1");
    assert(position.blackWins());

    position = Position();
    Position copy = Position(position);
    copy.makeMove("e2e3");
    assert(position.getCellState("e2") == White);
    assert(copy.getCellState("e2") == Empty);

    position = Position();
    position = Position(position);
    assert(position.boardState[0][0] == White);
    assert(position.boardState[4][4] == Empty);
    assert(position.boardState[7][7] == Black);

    int actual;

    position = Position();
    CellState boardState1[8][8] = {
            Empty, Empty, Empty, Empty, Black, Empty, Empty, Empty,
            Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty,
            Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty,
            Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty,
            Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty,
            Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty,
            Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty,
            Empty, Empty, Empty, Empty, White, Empty, Empty, Empty,
    };
    memcpy(position.boardState, boardState1, sizeof(CellState) * 8 * 8);
    assert(position.whiteLeadingPawn() == 4);
    assert(position.blackLeadingPawn() == 3);

    actual = alphaBetaPruning(position, NEGATIVE_INF, POSITIVE_INF, 2);

    position = Position();
    CellState boardState2[8][8] = {
            Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty,
            Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty,
            Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty,
            White, Empty, Empty, Empty, Empty, Empty, Empty, Empty,
            Empty, Empty, Empty, Empty, Empty, Empty, Empty, Black,
            Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty,
            Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty,
            Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty,
    };
    memcpy(position.boardState, boardState2, sizeof(CellState) * 8 * 8);
    assert(position.whiteLeadingPawn() == 0);
    assert(position.blackLeadingPawn() == 0);
    assert(position.score() == 0);

    actual = alphaBetaPruning(position, NEGATIVE_INF, POSITIVE_INF, 2);
    assert(actual == 0);

    position = Position();
    assert(position.interceptors("e2") == 16);
    assert(position.interceptors("e7") == 16);

    position = Position();
    CellState boardState3[8][8] = {
            Empty, White, Empty, Empty, Black, Empty, Empty, Empty,
            Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty,
            Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty,
            Empty, White, Empty, Empty, Empty, Empty, Empty, Empty,
            White, Empty, Empty, Empty, Empty, Empty, Empty, Black,
            Empty, Empty, Empty, Empty, Empty, Empty, Black, Empty,
            Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty,
            Empty, Empty, Empty, Empty, White, Empty, Empty, Black,
    };
    memcpy(position.boardState, boardState3, sizeof(CellState) * 8 * 8);
    assert(position.interceptors("a5") == 3);
    assert(position.interceptors("h5") == 3);

    cout << "All tests passed." << endl;
}

int main(int argc, char *argv[]) {
    debug = argc == 2 && strcmp(argv[1], "debug") == 0;

    if (debug) {
        test();
        freopen("input.txt", "r", stdin);
        freopen("output.txt", "w", stdout);
    }

    const int MAX_DEPTH = 4;
    Position position;
    char input[10];
    Move bestMove;

    while(true) {
        cin >> input;

        if (strcmp(input, "Name") == 0) {
            cout << "AlphaBot" << MAX_DEPTH << endl;
            continue;
        }
        else if (strcmp(input, "Quit") == 0) {
            return 0;
        }
        else if (strcmp(input, "Start") == 0) {
        }
        else {
            position.makeMove(input);
        }

        deadline = clock_t(clock() + CLOCKS_PER_SEC * 2.8);
        if (debug) deadline += 1e8;
        bestMove = findBestMove(position, MAX_DEPTH);
        position.makeMove(bestMove);
        cout << bestMove << endl;
        cout.flush();
    }
}

Move findBestMove(const Position &position, const int maxDepth) {
    int alpha = NEGATIVE_INF;
    int beta = POSITIVE_INF;
    int bestScore;
    Move bestMove;

    if (position.playerToMove == WhitePlayer)
        bestScore = NEGATIVE_INF;
    else
        bestScore = POSITIVE_INF;

    auto allMoves = position.getAllPossibleMoves();
    for (auto it = allMoves.begin(); it != allMoves.end(); it++) {
        Position childPosition = Position(position);
        childPosition.makeMove(*it);
        int score = alphaBetaPruning(childPosition, alpha, beta, maxDepth - 1);
        if (position.playerToMove == WhitePlayer) {
            if (score > bestScore) {
                bestScore = score;
                bestMove = *it;
            }

            alpha = max(alpha, bestScore);
        }
        else {
            if (score < bestScore) {
                bestScore = score;
                bestMove = *it;
            }

            beta = min(beta, score);
        }
    }

    return bestMove;
}

int alphaBetaPruning(const Position &position, int alpha, int beta, int depth) {

    if (depth == 0 || position.isFinal() || clock() > deadline) {
        return position.score();
    }

    auto childPositions = position.getChildPositions();
    int score;

    if (position.playerToMove == WhitePlayer) {
        score = NEGATIVE_INF;
        for (auto it = childPositions.begin(); it != childPositions.end(); it++) {
            score = max(score, alphaBetaPruning(*it, alpha, beta, depth - 1));
            alpha = max(alpha, score);
            if (beta <= alpha) {
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
                // Альфа отсечение
                break;
            }
        }
    }

    return score;
}