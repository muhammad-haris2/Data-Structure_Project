//#include "save.h"
//#include <fstream>
//#include <sstream>
//#include <cstdlib>
//#include <ctime>
//
//TileList::~TileList() {
//    clear();
//}
//
//void TileList::clear() {
//    TileNode* current = head;
//    while (current) {
//        TileNode* next = current->next;
//        delete current;
//        current = next;
//    }
//    head = nullptr;
//}
//
//void TileList::addTile(int x, int y, int state, int owner) {
//    TileNode* newNode = new TileNode(x, y, state, owner);
//    newNode->next = head;
//    head = newNode;
//}
//
//void TileList::loadFromArrays(int grid[M][N], int tileOwner[M][N]) {
//    clear();
//    for (int i = 0; i < M; ++i) {
//        for (int j = 0; j < N; ++j) {
//            if (grid[i][j] != 0) { // Save non-empty tiles
//                addTile(i, j, grid[i][j], tileOwner[i][j]);
//            }
//        }
//    }
//}
//
//void TileList::saveToArrays(int grid[M][N], int tileOwner[M][N]) const {
//    // Reset arrays
//    for (int i = 0; i < M; ++i) {
//        for (int j = 0; j < N; ++j) {
//            grid[i][j] = 0;
//            tileOwner[i][j] = 0;
//        }
//    }
//    // Restore tiles
//    TileNode* current = head;
//    while (current) {
//        grid[current->x][current->y] = current->state;
//        tileOwner[current->x][current->y] = current->owner;
//        current = current->next;
//    }
//}
//
//std::string SaveGame::generateSaveID() {
//    time_t now = time(nullptr);
//    std::stringstream ss;
//    ss << "SAVE_" << now << "_" << (rand() % 10000);
//    return ss.str();
//}
//
//bool SaveGame::saveGameState(const GameState& state, const std::string& filename) {
//    std::ofstream outFile(filename);
//    if (!outFile.is_open()) {
//        return false;
//    }
//
//    outFile << state.saveID << "\n";
//    outFile << state.timestamp << "\n";
//    outFile << state.player1ID << "\n";
//    outFile << state.player2ID << "\n";
//    // Player 1 state
//    outFile << state.player1State.x << " " << state.player1State.y << " "
//        << state.player1State.dx << " " << state.player1State.dy << " "
//        << state.player1State.score << " " << state.player1State.powerUps << " "
//        << state.player1State.capturedTiles << " " << state.player1State.constructing << " "
//        << state.player1State.frozen << " " << state.player1State.alive << " "
//        << state.player1State.pathColor.r << " " << state.player1State.pathColor.g << " "
//        << state.player1State.pathColor.b << " " << state.player1State.showBackgroundMenu << "\n";
//    // Player 2 state
//    outFile << state.player2State.x << " " << state.player2State.y << " "
//        << state.player2State.dx << " " << state.player2State.dy << " "
//        << state.player2State.score << " " << state.player2State.powerUps << " "
//        << state.player2State.capturedTiles << " " << state.player2State.constructing << " "
//        << state.player2State.frozen << " " << state.player2State.alive << " "
//        << state.player2State.pathColor.r << " " << state.player2State.pathColor.g << " "
//        << state.player2State.pathColor.b << " " << state.player2State.showBackgroundMenu << "\n";
//    outFile << state.isMultiplayer << "\n";
//    outFile << state.levelIndex << "\n";
//    outFile << state.gameTimer << "\n";
//    outFile << state.enemyCount << "\n";
//    // Enemies
//    for (int i = 0; i < state.enemyCount; ++i) {
//        outFile << state.enemies[i].x << " " << state.enemies[i].y << " "
//            << state.enemies[i].dx << " " << state.enemies[i].dy << " "
//            << state.enemies[i].frozen << " " << state.enemies[i].speed << "\n";
//    }
//    // Bonuses
//    outFile << state.bonusCountP1 << " " << state.bonusThresholdP1 << " " << state.bonusMultiplierP1 << "\n";
//    outFile << state.bonusCountP2 << " " << state.bonusThresholdP2 << " " << state.bonusMultiplierP2 << "\n";
//    // Tiles
//    TileNode* current = state.tiles.getHead();
//    while (current) {
//        outFile << current->x << " " << current->y << " " << current->state << " " << current->owner << "\n";
//        current = current->next;
//    }
//    outFile << "-1 -1 -1 -1\n"; // End of tiles marker
//
//    outFile.close();
//    return true;
//}
//
//bool SaveGame::loadGameState(GameState& state, const std::string& filename) {
//    std::ifstream inFile(filename);
//    if (!inFile.is_open()) {
//        return false;
//    }
//
//    state.tiles.clear();
//    std::string line;
//    std::getline(inFile, state.saveID);
//    inFile >> state.timestamp; inFile.ignore();
//    std::getline(inFile, state.player1ID);
//    std::getline(inFile, state.player2ID);
//    // Player 1 state
//    inFile >> state.player1State.x >> state.player1State.y
//        >> state.player1State.dx >> state.player1State.dy
//        >> state.player1State.score >> state.player1State.powerUps
//        >> state.player1State.capturedTiles >> state.player1State.constructing
//        >> state.player1State.frozen >> state.player1State.alive;
//    int r, g, b;
//    inFile >> r >> g >> b >> state.player1State.showBackgroundMenu;
//    state.player1State.pathColor = sf::Color(r, g, b);
//    // Player 2 state
//    inFile >> state.player2State.x >> state.player2State.y
//        >> state.player2State.dx >> state.player2State.dy
//        >> state.player2State.score >> state.player2State.powerUps
//        >> state.player2State.capturedTiles >> state.player2State.constructing
//        >> state.player2State.frozen >> state.player2State.alive;
//    inFile >> r >> g >> b >> state.player2State.showBackgroundMenu;
//    state.player2State.pathColor = sf::Color(r, g, b);
//    inFile >> state.isMultiplayer;
//    inFile >> state.levelIndex;
//    inFile >> state.gameTimer;
//    inFile >> state.enemyCount;
//    // Enemies
//    for (int i = 0; i < state.enemyCount; ++i) {
//        inFile >> state.enemies[i].x >> state.enemies[i].y
//            >> state.enemies[i].dx >> state.enemies[i].dy
//            >> state.enemies[i].frozen >> state.enemies[i].speed;
//        state.enemies[i].frozenClock.restart();
//    }
//    // Bonuses
//    inFile >> state.bonusCountP1 >> state.bonusThresholdP1 >> state.bonusMultiplierP1;
//    inFile >> state.bonusCountP2 >> state.bonusThresholdP2 >> state.bonusMultiplierP2;
//    // Tiles
//    int x, y, tileState, owner;
//    while (inFile >> x >> y >> tileState >> owner) {
//        if (x == -1 && y == -1 && tileState == -1 && owner == -1) {
//            break;
//        }
//        state.tiles.addTile(x, y, tileState, owner);
//    }
//
//    inFile.close();
//    return true;
//}
//
//bool SaveGame::validateSaveID(const std::string& saveID) {
//    std::ifstream file("saves/" + saveID + ".txt");
//    return file.good();
//}