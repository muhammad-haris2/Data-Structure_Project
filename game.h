#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include "player.h"

const int M = 30;  // Rows (600 / 20)
const int N = 40;  // Columns (800 / 20)
const int ts = 20; // Tile size

struct Enemy {
    int x, y, dx, dy;
    bool frozen;
    sf::Clock freezeClock;
    float moveTimer;

    Enemy();
    void move(int grid[M][N], float deltaTime);
    void activateFreeze();
};

struct PlayerState {
    int x, y, dx, dy;
    int score;
    int powerUps;
    int capturedTiles;
    bool alive;
    bool constructing;
    bool frozen;
    sf::Clock freezeClock;
    sf::Color pathColor;
    PlayerState() : x(0), y(0), dx(0), dy(0), score(0), powerUps(0), capturedTiles(0), alive(true), constructing(false), frozen(false), pathColor() {}
};

void drop(int y, int x, int grid[M][N]);
void playXonixGame(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& currentUser);
void playMultiplayerXonixGame(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& player1, const std::string& player2);

#endif