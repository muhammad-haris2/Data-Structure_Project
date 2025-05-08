#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include "player.h"
#include "inventory.h"

const int M = 20;
const int N = 40;
const int ts = 18;

struct PlayerState {
    int x, y, dx, dy;
    int score;
    int powerUps;
    int capturedTiles;
    bool constructing;
    bool frozen;
    bool alive;
    sf::Color pathColor;
    sf::Clock freezeClock;
    bool showBackgroundMenu; // Added to toggle background selection UI
    PlayerState() : x(0), y(0), dx(0), dy(0), score(0), powerUps(0), capturedTiles(0),
        constructing(false), frozen(false), alive(true),
        pathColor(sf::Color::White), showBackgroundMenu(false) {
    }
};

struct Enemy {
    int x, y, dx, dy;
    bool frozen;
    float moveTimer;
    sf::Clock freezeClock;
    Enemy();
    void move(int grid[M][N], float deltaTime);
    void activateFreeze();
};

void drop(int y, int x, int grid[M][N]);
void playXonixGame(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& currentUser);
void playMultiplayerXonixGame(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& player1User, const std::string& player2User);

#endif