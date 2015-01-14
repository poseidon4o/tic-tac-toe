#include "Game.h"

#include <cstring>
#include <algorithm>
#include <sstream>
#include <cassert>

using namespace std;

Game::Game(Game::Color first): mNextTurn(first), mWinner(Color::NONE) {
    for (Row & row : mBoard) {
        row.fill(Color::NONE);
    }
}

bool Game::Finished() const {
    if (GetWinner() != Game::Color::NONE) {
        return true;
    }
    bool full = true;
    for (int c = 0; c < Game::Size; ++c) {
        for (int r = 0; r < Game::Size; ++r) {
            full = full && mBoard[c][r] != Game::Color::NONE;
        }
    }
    return full;
}

Game::Color Game::GetWinner() const {
    return mWinner;
}

Game::Color Game::OnMove() const {
    return mNextTurn;
}

bool Game::operator==(const Game & other) {
    return 0 == memcmp(&mBoard, &other.mBoard, sizeof(mBoard)) && mWinner == other.mWinner && mNextTurn == other.mNextTurn;
}

const Game::Row & Game::operator[](int r) const {
    return mBoard[r];
}

bool Game::MakeMove(int x, int y) {
    if (x < 0 || x >= Game::Size || y < 0 || y >= Game::Size) {
        return false;
    }

    if (mBoard[x][y] != Color::NONE) {
        return false;
    }

    mBoard[x][y] = mNextTurn;
    checkWinner(x, y);

    mNextTurn = static_cast<Game::Color>(!mNextTurn);
    return true;
}

void Game::checkWinner(int x, int y) {
    array<int, 4> sums = { 0, 0, 0, 0 };

    for (int c = 0; c < Game::Size; ++c) {
        sums[0] += mBoard[c][y] == mNextTurn;
        sums[1] += mBoard[x][c] == mNextTurn;
        // main diagonal
        sums[2] += mBoard[c][c] == mNextTurn && (x == y);
        // secondary diagonal
        sums[3] += mBoard[Game::Size - c - 1][c] == mNextTurn && (x + y == Game::Size - 1);
    }

    bool isWinner = any_of(sums.begin(), sums.end(), [](const int & sum) -> bool {
        return sum == Game::Size;
    });

    mWinner = isWinner ? mNextTurn : NONE;
}

const string & Game::ColorToSymbol(Game::Color color) {
    static array<string, 3> map = { "X", "O", " " };
    return map[color];
}

string Game::ToString() const {
    stringstream stream;
    if (Finished()) {
        stream << "Winner [" << ColorToSymbol(GetWinner()) << "]\n";
    } else {
        stream << "On move [" << ColorToSymbol(mNextTurn) << "]\n";
    }

    for (int c = 0; c < Game::Size; ++c) {
        for (int r = 0; r < Game::Size; ++r) {
            stream << ColorToSymbol(mBoard[c][r]) << (r == Game::Size - 1 ? '\n' : '|');
        }
        if (c != Game::Size - 1) {
            for (int r = 0; r < Game::Size; ++r) {
                stream << '-' << (r == Game::Size - 1 ? '\n' : '+');
            }
        }
    }

    return stream.str();
}

static void append(vector<uint8_t> & dst, const char * data, int size) {
    for (int c = 0; c < size; ++c) {
        dst.push_back(data[c]);
    }
}

void Game::Serialize(vector<uint8_t> & data) const {
    int initialSize = data.size();
    int mySize = sizeof(*this);
    data.reserve(mySize);

    append(data, reinterpret_cast<const char *>(&mBoard), sizeof(mBoard)); // 9
    append(data, reinterpret_cast<const char *>(&mNextTurn), sizeof(mNextTurn)); // 1
    append(data, reinterpret_cast<const char *>(&mWinner), sizeof(mWinner)); // 1

    static_assert(sizeof(Game) == (sizeof(Game::mBoard) + sizeof(Game::mNextTurn) + sizeof(Game::mWinner)), "Deserialize mismatching sizes!");
}

bool Game::Deserialize(vector<uint8_t> & data, Game & target) {
    if (data.size() != sizeof(Game)) {
        return false;
    }
    
    int c = 0;

    Game game;
    memcpy(reinterpret_cast<char*>(&game.mBoard), data.data() + c, sizeof(game.mBoard));
    c += sizeof(game.mBoard);
    
    memcpy(reinterpret_cast<char*>(&game.mNextTurn), data.data() + c, sizeof(game.mNextTurn));
    c += sizeof(game.mNextTurn);

    memcpy(reinterpret_cast<char*>(&game.mWinner), data.data() + c, sizeof(game.mWinner));
    c += sizeof(game.mWinner);

    target = game;
    return true;
}
