#ifndef _GAME_H_INCLUDED_
#define _GAME_H_INCLUDED_

#include <array>
#include <cstdint>
#include <string>
#include <vector>

const int GameSize = 3;

class Game {
public:
    enum Color: uint8_t { X, O, NONE };
    enum { Size = GameSize };
    typedef std::array<Color, Game::Size> Row;
private:
    std::array<Row, Game::Size> mBoard;
    Color mNextTurn, mWinner;
public:
    Game(Color first = Color::X);

    static const std::string & ColorToSymbol(Game::Color color);

    const Row & operator[](int r) const;
    bool operator==(const Game & other);

    bool Finished() const;
    Color GetWinner() const;
    Color OnMove() const;

    bool MakeMove(int x, int y);

    std::string ToString() const;
    void Serialize(std::vector<uint8_t> & data) const;
    static bool Deserialize(std::vector<uint8_t> & data, Game & target);

private:
    void checkWinner(int x, int y);
};


#endif // _GAME_H_INCLUDED_