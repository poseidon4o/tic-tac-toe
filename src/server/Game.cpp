#include "Game.h"
#include <algorithm>
#include <sstream>

using namespace std;

Game::Game(Game::Color first): mNextTurn(first), mWinner(Color::NONE) {
    for (Row & row : mBoard) {
        row.fill(Color::NONE);
    }
}

bool Game::Finished() const {
    return GetWinner() != Game::Color::NONE;
}

Game::Color Game::GetWinner() const {
    return mWinner;
}

Game::Color Game::OnMove() const {
    return mNextTurn;
}

bool Game::MakeMove(int y, int x) {
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
    data.clear();
    data.reserve(sizeof(*this));
    append(data, reinterpret_cast<const char *>(&GameSize), sizeof(GameSize));
    append(data, reinterpret_cast<const char *>(&mBoard), sizeof(mBoard));
    append(data, reinterpret_cast<const char *>(&mNextTurn), sizeof(mNextTurn));
    append(data, reinterpret_cast<const char *>(&mWinner), sizeof(mWinner));
}