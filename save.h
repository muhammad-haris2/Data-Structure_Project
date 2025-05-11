//#ifndef SAVE_H
//#define SAVE_H
//
//#include <string>
//#include <ctime>
//#include <SFML/Graphics.hpp>
//#include "game.h" // For PlayerState, Enemy
//
//const int M = 30; // Grid rows
//const int N = 40; // Grid columns
//
//// Tile Node for Linked List
//struct TileNode {
//    int x, y; // Tile coordinates
//    int state; // Tile state (0=empty, 1=captured, 2=P1 constructing, 3=P2 constructing)
//    int owner; // Tile owner (0=none, 1=P1, 2=P2)
//    TileNode* next;
//    TileNode(int x, int y, int state, int owner) : x(x), y(y), state(state), owner(owner), next(nullptr) {}
//};
//
//// Linked List for Tiles
//class TileList {
//private:
//    TileNode* head;
//public:
//    TileList() : head(nullptr) {}
//    ~TileList();
//    void addTile(int x, int y, int state, int owner);
//    void clear();
//    TileNode* getHead() const { return head; }
//    void loadFromArrays(int grid[M][N], int tileOwner[M][N]);
//    void saveToArrays(int grid[M][N], int tileOwner[M][N]) const;
//};
//
//// GameState Structure
//struct GameState {
//    std::string saveID; // Unique identifier
//    time_t timestamp; // Save time
//    std::string player1ID; // Username for Player 1
//    std::string player2ID; // Username for Player 2 (empty for single-player)
//    PlayerState player1State; // Player 1 state
//    PlayerState player2State; // Player 2 state
//    bool isMultiplayer; // Game mode
//    int levelIndex; // Level (0=Easy, 1=Medium, 2=Hard)
//    float gameTimer; // Timer for movement delay
//    int enemyCount; // Number of enemies
//    Enemy enemies[10]; // Array of enemies
//    int bonusCountP1, bonusThresholdP1, bonusMultiplierP1; // P1 bonuses
//    int bonusCountP2, bonusThresholdP2, bonusMultiplierP2; // P2 bonuses
//    TileList tiles; // Linked list of tiles
//};
//
//// SaveGame Class
//class SaveGame {
//private:
//    static std::string generateSaveID();
//public:
//    static bool saveGameState(const GameState& state, const std::string& filename);
//    static bool loadGameState(GameState& state, const std::string& filename);
//    static bool validateSaveID(const std::string& saveID);
//};
//
//#endif