//#ifndef GAME_H
//#define GAME_H
//
//#include <SFML/Graphics.hpp>
//#include "player.h"
//#include "inventory.h"
//
//const int M = 20;
//const int N = 40;
//const int ts = 18;
//
//struct PlayerState {
//    float x, y; // Changed to float for smooth movement
//    float dx, dy;
//   
//    float speed; // Added for speed control
//    
//    int score;
//    int powerUps;
//    int capturedTiles;
//    bool constructing;
//    bool frozen;
//    bool alive;
//    sf::Color pathColor;
//    sf::Clock frozenClock;
//    bool showBackgroundMenu;
//    PlayerState() : x(0), y(0), dx(0), dy(0), score(0), powerUps(0), capturedTiles(0),
//        constructing(false), frozen(false), alive(true),
//        pathColor(sf::Color::White), showBackgroundMenu(false) {
//    }
//};
//
//struct Enemy {
//    int x, y, dx, dy;
//    bool frozen;
//    float moveTimer;
//    sf::Clock frozenClock;
//    Enemy();
//    void move(int grid[M][N], float deltaTime);
//    void activateFreeze();
//};
//
//void drop(int y, int x, int grid[M][N]);
//void playXonixGame(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& currentUser, Inventory& inventory);
//void playMultiplayerXonixGame(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& player1User, const std::string& player2User, Inventory& inventory);
//
//#endif
//

#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include "player.h"
#include "inventory.h"

const int M = 30;
const int N = 40;
const int ts = 20; // Tile size in pixels
struct PlayerState {
    float x, y; // Float for potential smooth movement
    float dx, dy;
    float speed; // Defined but unused in current game.cpp
    int score;
    int powerUps;
    int capturedTiles;
    bool constructing;
    bool frozen;
    bool alive;
    sf::Color pathColor;
    sf::Clock frozenClock;
    bool showBackgroundMenu;
    PlayerState() : x(0), y(0), dx(0), dy(0), speed(0), score(0), powerUps(0), capturedTiles(0),
        constructing(false), frozen(false), alive(true),
        pathColor(sf::Color::White), showBackgroundMenu(false) {
    }
};

struct Enemy {
    float x, y; // Float for smooth movement
    float dx, dy;
    bool frozen;
	//Clock frozenClock;
    float speed; // Speed in pixels/second
    sf::Clock frozenClock;
    Enemy(int grid[M][N], int tileSize);
    void move(int grid[M][N], float deltaTime, int tileSize);
    void activateFreeze();
    
	Enemy() : x(0), y(0), dx(0), dy(0), frozen(false), speed(100.0f) {} // Default constructor
};

void drop(int y, int x, int grid[M][N]);
void playXonixGame(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& currentUser, Inventory& inventory);
void playMultiplayerXonixGame(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& player1User, const std::string& player2User, Inventory& inventory);

#endif