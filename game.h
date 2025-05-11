#ifndef GAME_H
#define GAME_H
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>


#define M 30
#define N 40
#include<iostream>

using namespace std;


using namespace sf;
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


struct PlayerState {
    float x, y;
    int dx, dy;
    int score;
    int powerUps;
    int capturedTiles;
    bool constructing;
    bool frozen;
    Clock frozenClock;
    bool showBackgroundMenu;
    Color pathColor;
    bool alive;
public:
    PlayerState() : x(0), y(0), dx(0), dy(0), score(0), powerUps(0), capturedTiles(0),
        constructing(false), frozen(false), showBackgroundMenu(false), alive(true) {
    }
    PlayerState(float x, float y, int dx, int dy, int score, int powerUps, int capturedTiles,
        bool constructing, bool frozen, bool showBackgroundMenu)
        : x(x), y(y), dx(dx), dy(dy), score(score), powerUps(powerUps),
        capturedTiles(capturedTiles), constructing(constructing),
        frozen(frozen), showBackgroundMenu(showBackgroundMenu) {
    }

};

// Struct to hold single-player game state for saving/loading
struct SaveGameState {
    char username[50]; // Max 50 chars for username
    int grid[M][N];   // Game grid
    // Player state
    float player_x, player_y;
    int player_dx, player_dy;
    int player_score;
    int player_powerUps;
    int player_capturedTiles;
    bool player_frozen;
    float player_frozenTime; // Elapsed seconds
    bool player_showBackgroundMenu;
    // Enemies (max 10)
    int enemyCount;
    float enemy_x[10], enemy_y[10];
    float enemy_dx[10], enemy_dy[10];
    float enemy_speed[10];
    bool enemy_frozen[10];
    float enemy_frozenTime[10]; // Elapsed seconds
    // Game settings
    int levelIndex;
    int bonusCount;
    int bonusThreshold;
    int bonusMultiplier;
    // Timers
    float timer;
    // Inventory
    int backgroundID;
    int soundID;
    // Music
    float musicOffset; // Seconds
public:
    SaveGameState() : player_x(0), player_y(0), player_dx(0), player_dy(0),
        player_score(0), player_powerUps(0), player_capturedTiles(0),
        player_frozen(false), player_frozenTime(0), player_showBackgroundMenu(false),
        enemyCount(0), levelIndex(0), bonusCount(0), bonusThreshold(0),
        bonusMultiplier(0), timer(0), backgroundID(0), soundID(0),
        musicOffset(0) {
        memset(username, 0, sizeof(username));
        memset(grid, 0, sizeof(grid));
        memset(enemy_x, 0, sizeof(enemy_x));
        memset(enemy_y, 0, sizeof(enemy_y));
        memset(enemy_dx, 0, sizeof(enemy_dx));
        memset(enemy_dy, 0, sizeof(enemy_dy));
        memset(enemy_speed, 0, sizeof(enemy_speed));
        memset(enemy_frozen, false, sizeof(enemy_frozen));
        memset(enemy_frozenTime, 0, sizeof(enemy_frozenTime));
    }
};

// Pause menu options
enum PauseMenuOption {
    PAUSE_NONE = 0,
    PAUSE_RESUME = 1,
    PAUSE_SAVE = 2,
    PAUSE_EXIT = 3
};

class Inventory;
class PlayerList;

// Function declarations
void drop(int y, int x, int grid[M][N]);
int selectLevel(RenderWindow& window, Font& font, Inventory& inventory);
void playXonixGame(PlayerList& pl, RenderWindow& window, Font& font, const string& currentUser, Inventory& inventory, SaveGameState* savedState);
//void playXonixGame(PlayerList& pl, RenderWindow& window, Font& font, const string& currentUser, Inventory& inventory);
void playMultiplayerXonixGame(PlayerList& pl, RenderWindow& window, Font& font, const string& player1User, const string& player2User, Inventory& inventory);
bool saveGame(const SaveGameState& state);
bool loadGame(const string& username, SaveGameState& state);

#endif