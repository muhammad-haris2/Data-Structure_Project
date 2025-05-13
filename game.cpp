#include "game.h"
#include <SFML/Audio.hpp>
#include <time.h>
#include <string>
#include "menu.h"
#include "fstream"
#include <sstream>
#include <iostream>

using namespace std;
using namespace sf;

// Tile type enumeration
enum TileType {
    EMPTY = 0,      // Empty tile
    BORDER = 1,     // Border tile
    PATH = 2,       // Player's path
    CAPTURED = 3,   // Captured tile
};

// Helper function to manually copy string to destination with size limit
void manualStringCopy(char* dest, const char* src, int maxSize) {
    int i = 0;
    while (i < maxSize - 1 && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

// Helper function to convert string to lowercase
void toLowerCase(char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] = str[i] + 32; // Convert A-Z to a-z
        }
    }
}

// Helper function to map background names to colors
Color getColorForBackground(const char* name) {
    char lowerName[50];
    manualStringCopy(lowerName, name, sizeof(lowerName));
    toLowerCase(lowerName);
    if (strcmp(lowerName, "blue") == 0) return Color(135, 206, 250); // Light blue
    if (strcmp(lowerName, "white") == 0) return Color::White;
    if (strcmp(lowerName, "red") == 0) return Color::Red;
    return Color::White; // Default
}

// Helper function to create a styled button
RectangleShape createButton(float x, float y, float width, float height, Color fillColor) {
    RectangleShape button(Vector2f(width, height));
    button.setPosition(x, y);
    button.setFillColor(fillColor);
    button.setOutlineColor(Color(50, 50, 50));
    button.setOutlineThickness(2);
    return button;
}

// Enemy implementation
Enemy::Enemy(int grid[M][N], int tileSize) {
    int maxX = N - 2, maxY = M - 2;
    do {
        x = (rand() % maxX + 1) * tileSize + tileSize / 2.0f;
        y = (rand() % maxY + 1) * tileSize + tileSize / 2.0f;
    } while (grid[static_cast<int>(y / tileSize)][static_cast<int>(x / tileSize)] != EMPTY);

    float angle = static_cast<float>(rand()) / RAND_MAX * 2 * 3.14159f;
    dx = cos(angle);
    dy = sin(angle);
    frozen = false;
    speed = 90.0f;
}

void Enemy::move(int grid[M][N], float deltaTime, int tileSize) {
    if (frozen && frozenClock.getElapsedTime().asSeconds() < 3) return;
    frozen = false;

    float moveX = dx * speed * deltaTime;
    float moveY = dy * speed * deltaTime;
    float newX = x + moveX;
    float newY = y + moveY;

    int gridX = static_cast<int>(newX / tileSize);
    int gridY = static_cast<int>(newY / tileSize);
    bool collideHorizontal = false, collideVertical = false;
    static int lastGridX = -1, lastGridY = -1, collisionCount = 0;

    if (gridX < 0 || gridX >= N) {
        collideHorizontal = true;
        if (gridX < 0) newX = tileSize / 2.0f + 2.0f;
        else newX = (N - 0.5f) * tileSize - 2.0f;
    }
    if (gridY < 0 || gridY >= M) {
        collideVertical = true;
        if (gridY < 0) newY = tileSize / 2.0f + 2.0f;
        else newY = (M - 0.5f) * tileSize - 2.0f;
    }

    if (!collideHorizontal && !collideVertical && gridY >= 0 && gridY < M && gridX >= 0 && gridX < N) {
        if (grid[gridY][gridX] == 1 || grid[gridY][gridX] == 2 || grid[gridY][gridX] == 3) {
            float tileCenterX = gridX * tileSize + tileSize / 2.0f;
            float tileCenterY = gridY * tileSize + tileSize / 2.0f;
            float dxToTile = newX - tileCenterX;
            float dyToTile = newY - tileCenterY;

            if (abs(dxToTile) > abs(dyToTile)) {
                collideHorizontal = true;
                if (dxToTile > 0) newX = tileCenterX + tileSize / 2.0f + 2.0f;
                else newX = tileCenterX - tileSize / 2.0f - 2.0f;
            }
            else {
                collideVertical = true;
                if (dyToTile > 0) newY = tileCenterY + tileSize / 2.0f + 2.0f;
                else newY = tileCenterY - tileSize / 2.0f - 2.0f;
            }
        }
    }

    if (collideHorizontal || collideVertical) {
        if (gridX == lastGridX && gridY == lastGridY) {
            collisionCount++;
        }
        else {
            collisionCount = 1;
            lastGridX = gridX;
            lastGridY = gridY;
        }

        if (collisionCount >= 3) {
            for (int dy = -1; dy <= 1 && collisionCount >= 3; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int testX = gridX + dx;
                    int testY = gridY + dy;
                    if (testX >= 0 && testX < N && testY >= 0 && testY < M && grid[testY][testX] == EMPTY) {
                        newX = testX * tileSize + tileSize / 2.0f;
                        newY = testY * tileSize + tileSize / 2.0f;
                        collisionCount = 0;
                        lastGridX = -1;
                        lastGridY = -1;
                        break;
                    }
                }
            }
        }

        if (collideHorizontal) dx = -dx;
        if (collideVertical) dy = -dy;

        float angle;
        do {
            angle = static_cast<float>(rand()) / RAND_MAX * 2 * 3.14159f;
        } while (abs(fmod(angle, 3.14159f / 2.0f)) < 0.2f || abs(fmod(angle, 3.14159f / 2.0f) - 3.14159f / 2.0f) < 0.2f);
        dx = cos(angle);
        dy = sin(angle);

        float len = sqrt(dx * dx + dy * dy);
        dx /= len;
        dy /= len;

        newX = x + dx * speed * deltaTime;
        newY = y + dy * speed * deltaTime;
    }
    else {
        collisionCount = 0;
        lastGridX = -1;
        lastGridY = -1;
    }

    x = newX;
    y = newY;

    if (x < tileSize / 2.0f) { x = tileSize / 2.0f + 2.0f; dx = abs(dx); }
    if (x > (N - 0.5f) * tileSize) { x = (N - 0.5f) * tileSize - 2.0f; dx = -abs(dx); }
    if (y < tileSize / 2.0f) { y = tileSize / 2.0f + 2.0f; dy = abs(dy); }
    if (y > (M - 0.5f) * tileSize) { y = (M - 0.5f) * tileSize - 2.0f; dy = -abs(dy); }
}

void Enemy::activateFreeze() {
    frozen = true;
    frozenClock.restart();
}

// Drop implementation for flood fill
void drop(int y, int x, int grid[M][N]) {
    if (y < 0 || y >= M || x < 0 || x >= N) return;
    if (grid[y][x] == EMPTY) grid[y][x] = -1;
    if (y > 0 && grid[y - 1][x] == EMPTY) drop(y - 1, x, grid);
    if (y < M - 1 && grid[y + 1][x] == EMPTY) drop(y + 1, x, grid);
    if (x > 0 && grid[y][x - 1] == EMPTY) drop(y, x - 1, grid);
    if (x < N - 1 && grid[y][x + 1] == EMPTY) drop(y, x + 1, grid);
}

// Save game state to file
bool saveGame(const SaveGameState& state) {
    char filepath[100];
    char prefix[] = "saves/";
    int i = 0;
    while (prefix[i] != '\0' && i < sizeof(filepath) - 1) {
        filepath[i] = prefix[i];
        i++;
    }
    int j = 0;
    while (state.username[j] != '\0' && i < sizeof(filepath) - 5) {
        filepath[i] = state.username[j];
        i++;
        j++;
    }
    filepath[i] = '.'; filepath[i + 1] = 's'; filepath[i + 2] = 'a'; filepath[i + 3] = 'v'; filepath[i + 4] = '\0';

    ofstream file(filepath);
    if (!file.is_open()) return false;

    file << "username:" << state.username << "\n";
    file << "grid:";
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            file << state.grid[i][j];
            if (i != M - 1 || j != N - 1) file << ",";
        }
    }
    file << "\n";
    file << "player_x:" << state.player_x << "\n";
    file << "player_y:" << state.player_y << "\n";
    file << "player_dx:" << state.player_dx << "\n";
    file << "player_dy:" << state.player_dy << "\n";
    file << "player_score:" << state.player_score << "\n";
    file << "player_powerUps:" << state.player_powerUps << "\n";
    file << "player_capturedTiles:" << state.player_capturedTiles << "\n";
    file << "player_frozen:" << (state.player_frozen ? 1 : 0) << "\n";
    file << "player_frozenTime:" << state.player_frozenTime << "\n";
    file << "player_showBackgroundMenu:" << (state.player_showBackgroundMenu ? 1 : 0) << "\n";
    file << "enemyCount:" << state.enemyCount << "\n";
    for (int i = 0; i < state.enemyCount; i++) {
        file << "enemy_x_" << i << ":" << state.enemy_x[i] << "\n";
        file << "enemy_y_" << i << ":" << state.enemy_y[i] << "\n";
        file << "enemy_dx_" << i << ":" << state.enemy_dx[i] << "\n";
        file << "enemy_dy_" << i << ":" << state.enemy_dy[i] << "\n";
        file << "enemy_speed_" << i << ":" << state.enemy_speed[i] << "\n";
        file << "enemy_frozen_" << i << ":" << (state.enemy_frozen[i] ? 1 : 0) << "\n";
        file << "enemy_frozenTime_" << i << ":" << state.enemy_frozenTime[i] << "\n";
    }
    file << "levelIndex:" << state.levelIndex << "\n";
    file << "bonusCount:" << state.bonusCount << "\n";
    file << "bonusThreshold:" << state.bonusThreshold << "\n";
    file << "bonusMultiplier:" << state.bonusMultiplier << "\n";
    file << "timer:" << state.timer << "\n";
    file << "backgroundID:" << state.backgroundID << "\n";
    file << "soundID:" << state.soundID << "\n";
    file << "musicOffset:" << state.musicOffset << "\n";

    file.close();
    return true;
}

// Load game state from file
bool loadGame(const string& username, SaveGameState& state) {
    char filepath[100];
    char prefix[] = "saves/";
    int i = 0;
    while (prefix[i] != '\0' && i < sizeof(filepath) - 1) {
        filepath[i] = prefix[i];
        i++;
    }
    int j = 0;
    while (username[j] != '\0' && i < sizeof(filepath) - 5) {
        filepath[i] = username[j];
        i++;
        j++;
    }
    filepath[i] = '.'; filepath[i + 1] = 's'; filepath[i + 2] = 'a'; filepath[i + 3] = 'v'; filepath[i + 4] = '\0';

    ifstream file(filepath);
    if (!file.is_open()) return false;

    string line;
    while (getline(file, line)) {
        if (line.find("username:") == 0) {
            manualStringCopy(state.username, line.substr(9).c_str(), sizeof(state.username));
        }
        else if (line.find("grid:") == 0) {
            string gridData = line.substr(5);
            stringstream ss(gridData);
            string val;
            int idx = 0;
            while (getline(ss, val, ',') && idx < M * N) {
                int gridVal = stoi(val);
                if (gridVal < EMPTY || gridVal > CAPTURED) {
                    file.close();
                    return false; // Invalid tile type
                }
                state.grid[idx / N][idx % N] = gridVal;
                idx++;
            }
            if (idx != M * N) {
                file.close();
                return false; // Incorrect grid size
            }
        }
        else if (line.find("player_x:") == 0) state.player_x = atof(line.substr(9).c_str());
        else if (line.find("player_y:") == 0) state.player_y = atof(line.substr(9).c_str());
        else if (line.find("player_dx:") == 0) state.player_dx = atoi(line.substr(10).c_str());
        else if (line.find("player_dy:") == 0) state.player_dy = atoi(line.substr(10).c_str());
        else if (line.find("player_score:") == 0) state.player_score = atoi(line.substr(13).c_str());
        else if (line.find("player_powerUps:") == 0) state.player_powerUps = atoi(line.substr(16).c_str());
        else if (line.find("player_capturedTiles:") == 0) state.player_capturedTiles = atoi(line.substr(21).c_str());
        else if (line.find("player_frozen:") == 0) state.player_frozen = atoi(line.substr(14).c_str()) != 0;
        else if (line.find("player_frozenTime:") == 0) state.player_frozenTime = atof(line.substr(18).c_str());
        else if (line.find("player_showBackgroundMenu:") == 0) state.player_showBackgroundMenu = atoi(line.substr(26).c_str()) != 0;
        else if (line.find("enemyCount:") == 0) state.enemyCount = atoi(line.substr(11).c_str());
        else if (line.find("enemy_x_") == 0) {
            int idx = atoi(line.substr(8, 1).c_str());
            state.enemy_x[idx] = atof(line.substr(line.find(":") + 1).c_str());
        }
        else if (line.find("enemy_y_") == 0) {
            int idx = atoi(line.substr(8, 1).c_str());
            state.enemy_y[idx] = atof(line.substr(line.find(":") + 1).c_str());
        }
        else if (line.find("enemy_dx_") == 0) {
            int idx = atoi(line.substr(9, 1).c_str());
            state.enemy_dx[idx] = atof(line.substr(line.find(":") + 1).c_str());
        }
        else if (line.find("enemy_dy_") == 0) {
            int idx = atoi(line.substr(9, 1).c_str());
            state.enemy_dy[idx] = atof(line.substr(line.find(":") + 1).c_str());
        }
        else if (line.find("enemy_speed_") == 0) {
            int idx = atoi(line.substr(12, 1).c_str());
            state.enemy_speed[idx] = atof(line.substr(line.find(":") + 1).c_str());
        }
        else if (line.find("enemy_frozen_") == 0) {
            int idx = atoi(line.substr(13, 1).c_str());
            state.enemy_frozen[idx] = atoi(line.substr(line.find(":") + 1).c_str()) != 0;
        }
        else if (line.find("enemy_frozenTime_") == 0) {
            int idx = atoi(line.substr(17, 1).c_str());
            state.enemy_frozenTime[idx] = atof(line.substr(line.find(":") + 1).c_str());
        }
        else if (line.find("levelIndex:") == 0) state.levelIndex = atoi(line.substr(11).c_str());
        else if (line.find("bonusCount:") == 0) state.bonusCount = atoi(line.substr(11).c_str());
        else if (line.find("bonusThreshold:") == 0) state.bonusThreshold = atoi(line.substr(15).c_str());
        else if (line.find("bonusMultiplier:") == 0) state.bonusMultiplier = atoi(line.substr(16).c_str());
        else if (line.find("timer:") == 0) state.timer = atof(line.substr(6).c_str());
        else if (line.find("backgroundID:") == 0) state.backgroundID = atoi(line.substr(13).c_str());
        else if (line.find("soundID:") == 0) state.soundID = atoi(line.substr(8).c_str());
        else if (line.find("musicOffset:") == 0) state.musicOffset = atof(line.substr(12).c_str());
    }

    file.close();
    if (state.enemyCount < 0 || state.enemyCount > 10 || state.levelIndex < 0 || state.levelIndex > 2) return false;
    return true;
}

// Level selection struct
struct Level {
    char name[20];
    int enemyCount;
    float enemySpeed;
};

// Single-player level selection
int selectLevel(RenderWindow& window, Font& font, Inventory& inventory) {
    window.setTitle("Select Level");

    Level levels[3];
    char easy[] = "Easy";
    char medium[] = "Medium";
    char hard[] = "Hard";
    manualStringCopy(levels[0].name, easy, sizeof(levels[0].name));
    levels[0].enemyCount = 2;
    levels[0].enemySpeed = 45.0f;
    manualStringCopy(levels[1].name, medium, sizeof(levels[1].name));
    levels[1].enemyCount = 4;
    levels[1].enemySpeed = 90.0f;
    manualStringCopy(levels[2].name, hard, sizeof(levels[2].name));
    levels[2].enemyCount = 6;
    levels[2].enemySpeed = 135.0f;

    Text title("Select Level", font, 36);
    title.setFillColor(Color::White);
    title.setStyle(Text::Bold);
    title.setPosition(330, 40);
    title.setOutlineColor(Color::Black);
    title.setOutlineThickness(1);

    RectangleShape levelButtonBgs[3];
    Text levelButtons[3];
    for (int i = 0; i < 3; ++i) {
        levelButtonBgs[i] = createButton(320, 150 + i * 60, 160, 40, Color(50, 150, 50));
        levelButtons[i].setFont(font);
        levelButtons[i].setString(levels[i].name);
        levelButtons[i].setCharacterSize(22);
        levelButtons[i].setFillColor(Color::White);
        levelButtons[i].setPosition(350, 155 + i * 60);
    }

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
                return -1;
            }
            if (event.type == Event::MouseButtonPressed) {
                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
                for (int i = 0; i < 3; ++i) {
                    if (levelButtons[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) return i;
                }
            }
        }

        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
        for (int i = 0; i < 3; ++i) {
            levelButtonBgs[i].setFillColor(levelButtons[i].getGlobalBounds().contains(mousePos.x, mousePos.y) ? Color(70, 180, 70) : Color(50, 150, 50));
        }

        window.clear();
        inventory.drawBackground(window);
        window.draw(title);
        for (int i = 0; i < 3; ++i) {
            window.draw(levelButtonBgs[i]);
            window.draw(levelButtons[i]);
        }
        window.display();
    }
    return -1;
}

// Single-player game
void playXonixGame(PlayerList& pl, RenderWindow& window, Font& font, const string& currentUser, Inventory& inventory, SaveGameState* savedState) {
    window.setTitle("Xonix Game");
    const int windowWidth = 800;
    const int windowHeight = 600;
    const int ts = 18;
    const float offsetX = (windowWidth - N * ts) / 2.0f;
    const float offsetY = (windowHeight - M * ts) / 2.0f;
    window.setSize(Vector2u(windowWidth, windowHeight));
    srand(time(0));

    int grid[M][N] = { {0} };
    int levelIndex = 0;
    int enemyCount = 2;
    float enemySpeed = 45.0f;
    PlayerState player;
    int bonusCount = 0, bonusThreshold = 10, bonusMultiplier = 2;
    float timer = 0, delay = 0.07;
    bool paused = false;
    PauseMenuOption selectedOption = PAUSE_NONE;
    bool showFeedback = false;
    string feedbackMessage = "";
    Color feedbackColor = Color::Green;
    Clock feedbackClock;

    // Pause menu UI
    Text pauseTitle("Paused", font, 36);
    pauseTitle.setFillColor(Color::White);
    pauseTitle.setStyle(Text::Bold);
    pauseTitle.setPosition(350, 100);
    pauseTitle.setOutlineColor(Color::Black);
    pauseTitle.setOutlineThickness(1);

    RectangleShape pauseButtonBgs[3];
    Text pauseButtons[3];
    const char* pauseButtonLabels[3] = { "Resume", "Save Game", "Exit" };
    for (int i = 0; i < 3; ++i) {
        pauseButtonBgs[i] = createButton(320, 200 + i * 60, 160, 40, Color(50, 150, 50));
        pauseButtons[i].setFont(font);
        pauseButtons[i].setString(pauseButtonLabels[i]);
        pauseButtons[i].setCharacterSize(22);
        pauseButtons[i].setFillColor(Color::White);
        pauseButtons[i].setPosition(350, 205 + i * 60);
    }

    // Feedback text
    Text feedbackText("", font, 20);
    feedbackText.setPosition(300, 500);
    feedbackText.setOutlineColor(Color::Black);
    feedbackText.setOutlineThickness(1);

    // Load saved state if provided
    if (savedState) {
        for (int i = 0; i < M; i++)
            for (int j = 0; j < N; j++)
                grid[i][j] = savedState->grid[i][j];
        player.x = savedState->player_x;
        player.y = savedState->player_y;
        player.dx = savedState->player_dx;
        player.dy = savedState->player_dy;
        player.score = savedState->player_score;
        player.powerUps = savedState->player_powerUps;
        player.capturedTiles = savedState->player_capturedTiles;
        player.frozen = savedState->player_frozen;
        player.showBackgroundMenu = savedState->player_showBackgroundMenu;
        if (player.frozen && savedState->player_frozenTime >= 0 && savedState->player_frozenTime < 3) {
            player.frozenClock.restart();
        }
        levelIndex = savedState->levelIndex;
        enemyCount = savedState->enemyCount;
        bonusCount = savedState->bonusCount;
        bonusThreshold = savedState->bonusThreshold;
        bonusMultiplier = savedState->bonusMultiplier;
        timer = savedState->timer;
        inventory.setBackground(savedState->backgroundID);
        inventory.setSound(savedState->soundID);
    }
    else {
        levelIndex = selectLevel(window, font, inventory);
        if (levelIndex == -1) return;

        Level levels[3];
        char easy[] = "Easy";
        char medium[] = "Medium";
        char hard[] = "Hard";
        manualStringCopy(levels[0].name, easy, sizeof(levels[0].name));
        levels[0].enemyCount = 2;
        levels[0].enemySpeed = 45.0f;
        manualStringCopy(levels[1].name, medium, sizeof(levels[1].name));
        levels[1].enemyCount = 4;
        levels[1].enemySpeed = 90.0f;
        manualStringCopy(levels[2].name, hard, sizeof(levels[2].name));
        levels[2].enemyCount = 6;
        levels[2].enemySpeed = 135.0f;

        enemyCount = levels[levelIndex].enemyCount;
        enemySpeed = levels[levelIndex].enemySpeed;

        player.x = 10;
        player.y = 0;
        for (int i = 0; i < M; i++)
            for (int j = 0; j < N; j++)
                if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
                    grid[i][j] = BORDER;
                else
                    grid[i][j] = EMPTY;
    }

    Texture t1, t2, t3;
    if (!t1.loadFromFile("images/tiles.png") || !t2.loadFromFile("images/gameover.png") || !t3.loadFromFile("images/enemy.png")) {
        cout << "Error: Could not load game textures.\n";
        return;
    }

    Sprite sTile(t1), sGameover(t2), sEnemy(t3);
    sGameover.setPosition(200.0f + offsetX, 150.0f + offsetY);
    sEnemy.setOrigin(20, 20);

    Text scoreText("", font, 20);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(20.0f + offsetX, 15.0f + offsetY);

    Text powerUpText("", font, 20);
    powerUpText.setFillColor(Color(100, 255, 100));
    powerUpText.setPosition(20.0f + offsetX, 40.0f + offsetY);

    // Background selection UI
    Text backgroundOptions[10];
    char bgNames[10][50];
    int bgCount = 0;
    string namesString = inventory.getBackgroundNames();
    int nameIndex = 0;
    while (nameIndex < namesString.length() && bgCount < 10) {
        int bgIndex = 0;
        while (nameIndex < namesString.length() && namesString[nameIndex] != ',' && bgIndex < 49) {
            bgNames[bgCount][bgIndex++] = namesString[nameIndex++];
        }
        bgNames[bgCount][bgIndex] = '\0';
        if (bgIndex > 0) {
            backgroundOptions[bgCount].setFont(font);
            backgroundOptions[bgCount].setString(bgNames[bgCount]);
            backgroundOptions[bgCount].setCharacterSize(18);
            backgroundOptions[bgCount].setPosition(20.0f + offsetX, 70.0f + offsetY + bgCount * 25.0f);
            backgroundOptions[bgCount].setFillColor(bgCount + 1 == inventory.getCurrentBackgroundID() ? getColorForBackground(bgNames[bgCount]) : Color::White);
            bgCount++;
        }
        if (nameIndex < namesString.length() && namesString[nameIndex] == ',') nameIndex++;
    }

    Enemy enemies[10];
    for (int i = 0; i < enemyCount; ++i) {
        if (savedState) {
            enemies[i].x = savedState->enemy_x[i];
            enemies[i].y = savedState->enemy_y[i];
            enemies[i].dx = savedState->enemy_dx[i];
            enemies[i].dy = savedState->enemy_dy[i];
            enemies[i].speed = savedState->enemy_speed[i];
            enemies[i].frozen = savedState->enemy_frozen[i];
            if (enemies[i].frozen && savedState->enemy_frozenTime[i] >= 0 && savedState->enemy_frozenTime[i] < 3) {
                enemies[i].frozenClock.restart();
            }
        }
        else {
            enemies[i] = Enemy(grid, ts);
            enemies[i].speed = enemySpeed;
        }
    }

    bool Game = true;
    Clock clock;
    Music& music = inventory.getCurrentSound();
    music.setLoop(true);
    if (savedState) music.setPlayingOffset(seconds(savedState->musicOffset));
    music.play();

    while (window.isOpen()) {
        float time = clock.getElapsedTime().asSeconds();
        if (!paused) clock.restart();

        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) {
                music.stop();
                Player* p = pl.getPlayerByUsername(currentUser);
                if (p) pl.savePlayerStats(currentUser, player.score, player.powerUps, p->preferredThemeID, p->preferredSoundID);
                else cerr << "Error: Player " << currentUser << " not found for saving stats.\n";
                window.close();
                return;
            }

            if (e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::P && !player.showBackgroundMenu) {
                    paused = !paused;
                    selectedOption = PAUSE_NONE;
                }
                if (e.key.code == Keyboard::Escape && !paused) {
                    if (!Game) {
                        music.stop();
                        Player* p = pl.getPlayerByUsername(currentUser);
                        if (p) pl.savePlayerStats(currentUser, player.score, player.powerUps, p->preferredThemeID, p->preferredSoundID);
                        else cerr << "Error: Player " << currentUser << " not found for saving stats.\n";
                        return;
                    }
                    else {
                        for (int i = 0; i < M; i++)
                            for (int j = 0; j < N; j++)
                                if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
                                    grid[i][j] = BORDER;
                                else
                                    grid[i][j] = EMPTY;
                        player = PlayerState();
                        player.x = 10;
                        player.y = 0;
                        Game = true;
                        bonusCount = 0;
                        bonusThreshold = 10;
                        bonusMultiplier = 2;
                        for (int i = 0; i < enemyCount; i++) {
                            enemies[i] = Enemy(grid, ts);
                            enemies[i].speed = enemySpeed;
                        }
                        music.stop();
                        music.play();
                    }
                }
                if (e.key.code == Keyboard::Space && player.powerUps > 0 && Game && !paused) {
                    for (int i = 0; i < enemyCount; i++) enemies[i].activateFreeze();
                    player.powerUps--;
                }
                if (e.key.code == Keyboard::B && !paused) player.showBackgroundMenu = !player.showBackgroundMenu;
                if (e.key.code == Keyboard::S && Keyboard::isKeyPressed(Keyboard::LControl) && !paused && Game) {
                    SaveGameState state;
                    manualStringCopy(state.username, currentUser.c_str(), sizeof(state.username));
                    for (int i = 0; i < M; i++)
                        for (int j = 0; j < N; j++)
                            state.grid[i][j] = grid[i][j];
                    state.player_x = player.x;
                    state.player_y = player.y;
                    state.player_dx = player.dx;
                    state.player_dy = player.dy;
                    state.player_score = player.score;
                    state.player_powerUps = player.powerUps;
                    state.player_capturedTiles = player.capturedTiles;
                    state.player_frozen = player.frozen;
                    state.player_frozenTime = player.frozen ? player.frozenClock.getElapsedTime().asSeconds() : 0;
                    state.player_showBackgroundMenu = player.showBackgroundMenu;
                    state.enemyCount = enemyCount;
                    for (int i = 0; i < enemyCount; i++) {
                        state.enemy_x[i] = enemies[i].x;
                        state.enemy_y[i] = enemies[i].y;
                        state.enemy_dx[i] = enemies[i].dx;
                        state.enemy_dy[i] = enemies[i].dy;
                        state.enemy_speed[i] = enemies[i].speed;
                        state.enemy_frozen[i] = enemies[i].frozen;
                        state.enemy_frozenTime[i] = enemies[i].frozen ? enemies[i].frozenClock.getElapsedTime().asSeconds() : 0;
                    }
                    state.levelIndex = levelIndex;
                    state.bonusCount = bonusCount;
                    state.bonusThreshold = bonusThreshold;
                    state.bonusMultiplier = bonusMultiplier;
                    state.timer = timer;
                    state.backgroundID = inventory.getCurrentBackgroundID();
                    state.soundID = inventory.getCurrentSoundID();
                    state.musicOffset = music.getPlayingOffset().asSeconds();

                    if (saveGame(state)) {
                        feedbackMessage = "Game Saved!";
                        feedbackColor = Color::Green;
                    }
                    else {
                        feedbackMessage = "Save Failed!";
                        feedbackColor = Color::Red;
                    }
                    showFeedback = true;
                    feedbackClock.restart();
                }
            }

            if (e.type == Event::MouseButtonPressed) {
                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
                if (player.showBackgroundMenu && !paused) {
                    for (int i = 0; i < bgCount; ++i) {
                        if (backgroundOptions[i].getGlobalBounds().contains(mousePos)) {
                            inventory.setBackground(i + 1);
                            player.showBackgroundMenu = false;
                            for (int j = 0; j < bgCount; ++j) {
                                backgroundOptions[j].setFillColor(j + 1 == inventory.getCurrentBackgroundID() ? getColorForBackground(bgNames[j]) : Color::White);
                            }
                            break;
                        }
                    }
                }
                if (paused) {
                    for (int i = 0; i < 3; ++i) {
                        if (pauseButtons[i].getGlobalBounds().contains(mousePos)) {
                            selectedOption = static_cast<PauseMenuOption>(i + 1);
                            break;
                        }
                    }
                }
            }
        }

        if (paused && selectedOption != PAUSE_NONE) {
            if (selectedOption == PAUSE_RESUME) {
                paused = false;
            }
            else if (selectedOption == PAUSE_SAVE) {
                SaveGameState state;
                manualStringCopy(state.username, currentUser.c_str(), sizeof(state.username));
                for (int i = 0; i < M; i++)
                    for (int j = 0; j < N; j++)
                        state.grid[i][j] = grid[i][j];
                state.player_x = player.x;
                state.player_y = player.y;
                state.player_dx = player.dx;
                state.player_dy = player.dy;
                state.player_score = player.score;
                state.player_powerUps = player.powerUps;
                state.player_capturedTiles = player.capturedTiles;
                state.player_frozen = player.frozen;
                state.player_frozenTime = player.frozen ? player.frozenClock.getElapsedTime().asSeconds() : 0;
                state.player_showBackgroundMenu = player.showBackgroundMenu;
                state.enemyCount = enemyCount;
                for (int i = 0; i < enemyCount; i++) {
                    state.enemy_x[i] = enemies[i].x;
                    state.enemy_y[i] = enemies[i].y;
                    state.enemy_dx[i] = enemies[i].dx;
                    state.enemy_dy[i] = enemies[i].dy;
                    state.enemy_speed[i] = enemies[i].speed;
                    state.enemy_frozen[i] = enemies[i].frozen;
                    state.enemy_frozenTime[i] = enemies[i].frozen ? enemies[i].frozenClock.getElapsedTime().asSeconds() : 0;
                }
                state.levelIndex = levelIndex;
                state.bonusCount = bonusCount;
                state.bonusThreshold = bonusThreshold;
                state.bonusMultiplier = bonusMultiplier;
                state.timer = timer;
                state.backgroundID = inventory.getCurrentBackgroundID();
                state.soundID = inventory.getCurrentSoundID();
                state.musicOffset = music.getPlayingOffset().asSeconds();

                if (saveGame(state)) {
                    feedbackMessage = "Game Saved!";
                    feedbackColor = Color::Green;
                }
                else {
                    feedbackMessage = "Save Failed!";
                    feedbackColor = Color::Red;
                }
                showFeedback = true;
                feedbackClock.restart();
                selectedOption = PAUSE_NONE;
            }
            else if (selectedOption == PAUSE_EXIT) {
                music.stop();
                Player* p = pl.getPlayerByUsername(currentUser);
                if (p) pl.savePlayerStats(currentUser, player.score, player.powerUps, p->preferredThemeID, p->preferredSoundID);
                else cerr << "Error: Player " << currentUser << " not found for saving stats.\n";
                return;
            }
        }

        if (!paused && Game && !player.frozen) {
            if (Keyboard::isKeyPressed(Keyboard::Left)) { player.dx = -1; player.dy = 0; }
            if (Keyboard::isKeyPressed(Keyboard::Right)) { player.dx = 1; player.dy = 0; }
            if (Keyboard::isKeyPressed(Keyboard::Up)) { player.dx = 0; player.dy = -1; }
            if (Keyboard::isKeyPressed(Keyboard::Down)) { player.dx = 0; player.dy = 1; }

            if (timer > delay) {
                player.x += player.dx;
                player.y += player.dy;

                if (player.x < 0) player.x = 0; if (player.x > N - 1) player.x = N - 1;
                if (player.y < 0) player.y = 0; if (player.y > M - 1) player.y = M - 1;

                player.constructing = (grid[static_cast<int>(player.y)][static_cast<int>(player.x)] == EMPTY);
                if (grid[static_cast<int>(player.y)][static_cast<int>(player.x)] == PATH) Game = false;
                if (grid[static_cast<int>(player.y)][static_cast<int>(player.x)] == EMPTY) {
                    grid[static_cast<int>(player.y)][static_cast<int>(player.x)] = PATH;
                    player.capturedTiles++;
                }
                timer = 0;
            }

            for (int i = 0; i < enemyCount; i++) enemies[i].move(grid, time, ts);

            if (grid[static_cast<int>(player.y)][static_cast<int>(player.x)] == BORDER || grid[static_cast<int>(player.y)][static_cast<int>(player.x)] == CAPTURED) {
                player.dx = player.dy = 0;

                for (int i = 0; i < enemyCount; i++) {
                    int enemyY = static_cast<int>(enemies[i].y / ts);
                    int enemyX = static_cast<int>(enemies[i].x / ts);
                    if (enemyY >= 0 && enemyY < M && enemyX >= 0 && enemyX < N)
                        drop(enemyY, enemyX, grid);
                }

                int newlyCaptured = 0;
                for (int i = 0; i < M; i++)
                    for (int j = 0; j < N; j++) {
                        if (grid[i][j] == -1) grid[i][j] = EMPTY;
                        else if (grid[i][j] == PATH) {
                            grid[i][j] = CAPTURED;
                            newlyCaptured++;
                        }
                    }

                if (newlyCaptured > 0) {
                    if (newlyCaptured > bonusThreshold) {
                        player.score += newlyCaptured * bonusMultiplier;
                        bonusCount++;
                        if (bonusCount == 3) bonusThreshold = 5;
                        if (bonusCount >= 5) bonusMultiplier = 4;
                    }
                    else {
                        player.score += newlyCaptured;
                    }
                    cout << "Single-player: Captured " << newlyCaptured << " tiles, Score: " << player.score << endl;

                    if (player.score >= 50 && (player.score - newlyCaptured) < 50) player.powerUps++;
                    if (player.score >= 70 && (player.score - newlyCaptured) < 70) player.powerUps++;
                    if (player.score >= 100 && (player.score - newlyCaptured) < 100) player.powerUps++;
                    if (player.score >= 130 && (player.score - newlyCaptured) < 130) player.powerUps++;
                    if (player.score >= 160 && ((player.score - newlyCaptured) / 30 < player.score / 30)) player.powerUps++;
                    cout << "Single-player: Power-Ups: " << player.powerUps << endl;
                }
            }

            for (int i = 0; i < enemyCount; i++) {
                enemies[i].move(grid, time, ts);

                int enemyY = static_cast<int>(enemies[i].y / ts);
                int enemyX = static_cast<int>(enemies[i].x / ts);

                if (enemyY >= 0 && enemyY < M && enemyX >= 0 && enemyX < N) {
                    if (static_cast<int>(player.x) == enemyX && static_cast<int>(player.y) == enemyY && player.constructing) {
                        Game = false;
                        break;
                    }
                    if (grid[enemyY][enemyX] == PATH) {
                        if (player.constructing) {
                            Game = false;
                            break;
                        }
                        else {
                            enemies[i].dx = -enemies[i].dx;
                            enemies[i].dy = -enemies[i].dy;
                        }
                    }
                }
            }
        }

        if (player.frozen && player.frozenClock.getElapsedTime().asSeconds() >= 3) player.frozen = false;
        if (!paused) timer += time;

        window.clear();
        inventory.drawBackground(window);

        for (int i = 0; i < M; i++)
            for (int j = 0; j < N; j++) {
                if (grid[i][j] == EMPTY) continue;
                if (grid[i][j] == BORDER || grid[i][j] == CAPTURED) sTile.setTextureRect(IntRect(0, 0, ts, ts));
                if (grid[i][j] == PATH) sTile.setTextureRect(IntRect(54, 0, ts, ts));
                sTile.setPosition(j * ts + offsetX, i * ts + offsetY);
                window.draw(sTile);
            }

        sTile.setTextureRect(IntRect(36, 0, ts, ts));
        sTile.setPosition(static_cast<int>(player.x) * ts + offsetX, static_cast<int>(player.y) * ts + offsetY);
        window.draw(sTile);

        sEnemy.rotate(10);
        for (int i = 0; i < enemyCount; i++) {
            if (enemies[i].frozen) sEnemy.setColor(Color::Cyan);
            else sEnemy.setColor(Color::White);
            sEnemy.setPosition(enemies[i].x + offsetX, enemies[i].y + offsetY);
            window.draw(sEnemy);
        }

        scoreText.setString("Score: " + to_string(player.score));
        powerUpText.setString("Power-Ups: " + to_string(player.powerUps));
        window.draw(scoreText);
        window.draw(powerUpText);

        if (player.showBackgroundMenu) {
            for (int i = 0; i < bgCount; i++) window.draw(backgroundOptions[i]);
        }

        if (paused) {
            window.draw(pauseTitle);
            Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
            for (int i = 0; i < 3; ++i) {
                pauseButtonBgs[i].setFillColor(pauseButtons[i].getGlobalBounds().contains(mousePos.x, mousePos.y) ? Color(70, 180, 70) : Color(50, 150, 50));
                window.draw(pauseButtonBgs[i]);
                window.draw(pauseButtons[i]);
            }
        }

        if (showFeedback && feedbackClock.getElapsedTime().asSeconds() < 2) {
            feedbackText.setString(feedbackMessage);
            feedbackText.setFillColor(feedbackColor);
            window.draw(feedbackText);
        }
        else {
            showFeedback = false;
        }

        if (!Game) window.draw(sGameover);

        window.display();
    }
}

void playMultiplayerXonixGame(PlayerList& pl, RenderWindow& window, Font& font, const string& player1User, const string& player2User, Inventory& inventory) {
    window.setTitle("Xonix Multiplayer Game");

    srand(time(0));

    int grid[M][N] = { 0 };
    int tileOwner[M][N] = { 0 };

    const int ts = 18;
    const float offsetX = (800 - N * ts) / 2.0f;
    const float offsetY = (600 - M * ts) / 2.0f;

    Texture t1, t2, t3;
    if (!t1.loadFromFile("images/tiles.png") || !t2.loadFromFile("images/gameover.png") || !t3.loadFromFile("images/enemy.png")) {
        cout << "Error: Could not load game textures.\n";
        return;
    }

    Sprite sTile(t1), sGameover(t2), sEnemy(t3);
    sGameover.setPosition(200.0f + offsetX, 150.0f + offsetY);
    sEnemy.setOrigin(20, 20);

    Text scoreTextP1("", font, 20);
    scoreTextP1.setFillColor(Color::White);
    scoreTextP1.setPosition(20.0f + offsetX, 15.0f + offsetY);

    Text powerUpTextP1("", font, 20);
    powerUpTextP1.setFillColor(Color(100, 255, 100));
    powerUpTextP1.setPosition(20.0f + offsetX, 40.0f + offsetY);

    Text scoreTextP2("", font, 20);
    scoreTextP2.setFillColor(Color::White);
    scoreTextP2.setPosition(static_cast<float>(N * ts - 190) + offsetX, 15.0f + offsetY);

    Text powerUpTextP2("", font, 20);
    powerUpTextP2.setFillColor(Color(100, 255, 100));
    powerUpTextP2.setPosition(static_cast<float>(N * ts - 190) + offsetX, 40.0f + offsetY);

    Text winnerText("", font, 30);
    winnerText.setFillColor(Color::Yellow);
    winnerText.setStyle(Text::Bold);
    winnerText.setPosition(300.0f + offsetX, 120.0f + offsetY);
    winnerText.setOutlineColor(Color::Black);
    winnerText.setOutlineThickness(1);

    // Background options menu
    Text backgroundOptions[10];
    char bgNames[10][50];
    int bgCount = 0;
    string namesString = inventory.getBackgroundNames();
    int nameIndex = 0;
    while (nameIndex < namesString.length() && bgCount < 10) {
        int bgIndex = 0;
        while (nameIndex < namesString.length() && namesString[nameIndex] != ',' && bgIndex < 49) {
            bgNames[bgCount][bgIndex++] = namesString[nameIndex++];
        }
        bgNames[bgCount][bgIndex] = '\0';
        if (bgIndex > 0) {
            backgroundOptions[bgCount].setFont(font);
            backgroundOptions[bgCount].setString(bgNames[bgCount]);
            backgroundOptions[bgCount].setCharacterSize(18);
            backgroundOptions[bgCount].setPosition(20.0f + offsetX, 70.0f + offsetY + bgCount * 25.0f);
            backgroundOptions[bgCount].setFillColor(bgCount + 1 == inventory.getCurrentBackgroundID() ? getColorForBackground(bgNames[bgCount]) : Color::White);
            bgCount++;
        }
        if (nameIndex < namesString.length() && namesString[nameIndex] == ',') nameIndex++;
    }

    int enemyCount = 3 + (rand() % 3);
    Enemy enemies[10];
    for (int i = 0; i < enemyCount; ++i) enemies[i] = Enemy(grid, ts);

    bool Game = true;
    bool gameEnded = false;
    PlayerState player1, player2;
    player1.x = 10;
    player1.y = 0;
    player1.pathColor = Color::Blue;
    player2.x = N - 10;
    player2.y = 0;
    player2.pathColor = Color::Red;

    float timer = 0, delay = 0.07;
    Clock clock;

    int bonusCountP1 = 0, bonusThresholdP1 = 10, bonusMultiplierP1 = 2;
    int bonusCountP2 = 0, bonusThresholdP2 = 10, bonusMultiplierP2 = 2;

    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            if (i == 0 || j == 0 || i == M - 1 || j == N - 1) {
                grid[i][j] = 1;
                tileOwner[i][j] = 0;
            }

    Music& music = inventory.getCurrentSound();
    music.setLoop(true);
    music.play();

    while (window.isOpen()) {
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;

        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) {
                music.stop();
                Player* p1 = pl.getPlayerByUsername(player1User);
                if (p1) pl.savePlayerStats(player1User, player1.score, player1.powerUps, p1->preferredThemeID, p1->preferredSoundID);
                Player* p2 = pl.getPlayerByUsername(player2User);
                if (p2) pl.savePlayerStats(player2User, player2.score, player2.powerUps, p2->preferredThemeID, p2->preferredSoundID);
                if (!gameEnded && Game) {
                    pl.updatePlayerLosses(player1User, 1);
                    pl.updatePlayerLosses(player2User, 1);
                }
                window.close();
                return;
            }

            if (e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::Escape) {
                    if (!Game || (!player1.alive && !player2.alive)) {
                        music.stop();
                        Player* p1 = pl.getPlayerByUsername(player1User);
                        if (p1) pl.savePlayerStats(player1User, player1.score, player1.powerUps, p1->preferredThemeID, p1->preferredSoundID);
                        Player* p2 = pl.getPlayerByUsername(player2User);
                        if (p2) pl.savePlayerStats(player2User, player2.score, player2.powerUps, p2->preferredThemeID, p2->preferredSoundID);
                        if (!gameEnded && Game) {
                            pl.updatePlayerLosses(player1User, 1);
                            pl.updatePlayerLosses(player2User, 1);
                        }
                        return;
                    }
                    else {
                        for (int i = 0; i < M; i++)
                            for (int j = 0; j < N; j++) {
                                if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
                                    grid[i][j] = 1;
                                else
                                    grid[i][j] = 0;
                                tileOwner[i][j] = 0;
                            }
                        player1 = PlayerState();
                        player2 = PlayerState();
                        player1.x = 10;
                        player1.y = 0;
                        player1.pathColor = Color::Blue;
                        player2.x = N - 10;
                        player2.y = 0;
                        player2.pathColor = Color::Red;
                        Game = true;
                        gameEnded = false;
                        bonusCountP1 = bonusCountP2 = 0;
                        bonusThresholdP1 = bonusThresholdP2 = 10;
                        bonusMultiplierP1 = bonusMultiplierP2 = 2;
                        enemyCount = 3 + (rand() % 3);
                        for (int i = 0; i < enemyCount; i++) enemies[i] = Enemy(grid, ts);
                        music.stop();
                        music.play();
                    }
                }
                if (e.key.code == Keyboard::Space && player1.powerUps > 0 && Game && player1.alive) {
                    for (int i = 0; i < enemyCount; i++) enemies[i].activateFreeze();
                    player2.frozen = true;
                    player2.frozenClock.restart();
                    player1.powerUps--;
                }
                if (e.key.code == Keyboard::Return && player2.powerUps > 0 && Game && player2.alive) {
                    for (int i = 0; i < enemyCount; i++) enemies[i].activateFreeze();
                    player1.frozen = true;
                    player1.frozenClock.restart();
                    player2.powerUps--;
                }
                if (e.key.code == Keyboard::B) {
                    player1.showBackgroundMenu = !player1.showBackgroundMenu;
                    player2.showBackgroundMenu = player1.showBackgroundMenu;
                }
            }

            if (e.type == Event::MouseButtonPressed && player1.showBackgroundMenu) {
                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
                for (int i = 0; i < bgCount; ++i) {
                    if (backgroundOptions[i].getGlobalBounds().contains(mousePos)) {
                        inventory.setBackground(i + 1);
                        player1.showBackgroundMenu = false;
                        player2.showBackgroundMenu = false;
                        for (int j = 0; j < bgCount; ++j) {
                            backgroundOptions[j].setFillColor(j + 1 == inventory.getCurrentBackgroundID() ? getColorForBackground(bgNames[j]) : Color::White);
                        }
                        break;
                    }
                }
            }
        }

        if (Game) {
            if (!player1.frozen && player1.alive) {
                if (Keyboard::isKeyPressed(Keyboard::A)) { player1.dx = -1; player1.dy = 0; }
                if (Keyboard::isKeyPressed(Keyboard::D)) { player1.dx = 1; player1.dy = 0; }
                if (Keyboard::isKeyPressed(Keyboard::W)) { player1.dx = 0; player1.dy = -1; }
                if (Keyboard::isKeyPressed(Keyboard::S)) { player1.dx = 0; player1.dy = 1; }
            }

            if (!player2.frozen && player2.alive) {
                if (Keyboard::isKeyPressed(Keyboard::Left)) { player2.dx = -1; player2.dy = 0; }
                if (Keyboard::isKeyPressed(Keyboard::Right)) { player2.dx = 1; player2.dy = 0; }
                if (Keyboard::isKeyPressed(Keyboard::Up)) { player2.dx = 0; player2.dy = -1; }
                if (Keyboard::isKeyPressed(Keyboard::Down)) { player2.dx = 0; player2.dy = 1; }
            }

            if (timer > delay) {
                if (player1.alive && !player1.frozen) {
                    player1.x += player1.dx;
                    player1.y += player1.dy;

                    if (player1.x < 0) player1.x = 0; if (player1.x > N - 1) player1.x = N - 1;
                    if (player1.y < 0) player1.y = 0; if (player1.y > M - 1) player1.y = M - 1;

                    player1.constructing = (grid[static_cast<int>(player1.y)][static_cast<int>(player1.x)] == 0);
                    if (grid[static_cast<int>(player1.y)][static_cast<int>(player1.x)] == 2 && tileOwner[static_cast<int>(player1.y)][static_cast<int>(player1.x)] == 1) {
                        player1.alive = false;
                    }
                    else if (grid[static_cast<int>(player1.y)][static_cast<int>(player1.x)] == 3 ||
                        (grid[static_cast<int>(player1.y)][static_cast<int>(player1.x)] == 1 && tileOwner[static_cast<int>(player1.y)][static_cast<int>(player1.x)] == 2)) {
                        player1.alive = false;
                    }
                    else if (grid[static_cast<int>(player1.y)][static_cast<int>(player1.x)] == 0) {
                        grid[static_cast<int>(player1.y)][static_cast<int>(player1.x)] = 2;
                        tileOwner[static_cast<int>(player1.y)][static_cast<int>(player1.x)] = 1;
                        player1.capturedTiles++;
                    }
                }

                if (player2.alive && !player2.frozen) {
                    player2.x += player2.dx;
                    player2.y += player2.dy;

                    if (player2.x < 0) player2.x = 0; if (player2.x > N - 1) player2.x = N - 1;
                    if (player2.y < 0) player2.y = 0; if (player2.y > M - 1) player2.y = M - 1;

                    player2.constructing = (grid[static_cast<int>(player2.y)][static_cast<int>(player2.x)] == 0);
                    if (grid[static_cast<int>(player2.y)][static_cast<int>(player2.x)] == 3 && tileOwner[static_cast<int>(player2.y)][static_cast<int>(player2.x)] == 2) {
                        player2.alive = false;
                    }
                    else if (grid[static_cast<int>(player2.y)][static_cast<int>(player2.x)] == 2 ||
                        (grid[static_cast<int>(player2.y)][static_cast<int>(player2.x)] == 1 && tileOwner[static_cast<int>(player2.y)][static_cast<int>(player2.x)] == 1)) {
                        player2.alive = false;
                    }
                    else if (grid[static_cast<int>(player2.y)][static_cast<int>(player2.x)] == 0) {
                        grid[static_cast<int>(player2.y)][static_cast<int>(player2.x)] = 3;
                        tileOwner[static_cast<int>(player2.y)][static_cast<int>(player2.x)] = 2;
                        player2.capturedTiles++;
                    }
                }

                if (player1.alive && player2.alive && static_cast<int>(player1.x) == static_cast<int>(player2.x) && static_cast<int>(player1.y) == static_cast<int>(player2.y)) {
                    if (player1.constructing && player2.constructing) {
                        player1.alive = false;
                        player2.alive = false;
                    }
                    else if (player1.constructing) {
                        player1.alive = false;
                    }
                    else if (player2.constructing) {
                        player2.alive = false;
                    }
                    else {
                        player1.alive = false;
                        player2.alive = false;
                    }
                }

                timer = 0;
            }

            for (int i = 0; i < enemyCount; i++) {
                enemies[i].move(grid, time, ts);

                int enemyX = static_cast<int>(enemies[i].x / ts);
                int enemyY = static_cast<int>(enemies[i].y / ts);

                if (enemyY >= 0 && enemyY < M && enemyX >= 0 && enemyX < N) {
                    int enemyGridVal = grid[enemyY][enemyX];

                    if (player1.alive && player1.constructing) {
                        if ((static_cast<int>(player1.x) == enemyX && static_cast<int>(player1.y) == enemyY) || enemyGridVal == 2) {
                            player1.alive = false;
                        }
                    }

                    if (player2.alive && player2.constructing) {
                        if ((static_cast<int>(player2.x) == enemyX && static_cast<int>(player2.y) == enemyY) || enemyGridVal == 3) {
                            player2.alive = false;
                        }
                    }

                    if (enemyGridVal == 1 || enemyGridVal == 2 || enemyGridVal == 3) {
                        enemies[i].dx = -enemies[i].dx;
                        enemies[i].dy = -enemies[i].dy;
                    }
                }
            }

            if (player1.alive && grid[static_cast<int>(player1.y)][static_cast<int>(player1.x)] == 1) {
                player1.dx = player1.dy = 0;
                for (int i = 0; i < enemyCount; i++) {
                    int enemyY = static_cast<int>(enemies[i].y / ts);
                    int enemyX = static_cast<int>(enemies[i].x / ts);
                    if (enemyY >= 0 && enemyY < M && enemyX >= 0 && enemyX < N)
                        drop(enemyY, enemyX, grid);
                }

                int newlyCaptured = 0;
                for (int i = 0; i < M; i++)
                    for (int j = 0; j < N; j++) {
                        if (grid[i][j] == -1) {
                            grid[i][j] = 0;
                            tileOwner[i][j] = 0;
                        }
                        else if (grid[i][j] == 2 && tileOwner[i][j] == 1) {
                            grid[i][j] = 1;
                            tileOwner[i][j] = 1;
                            newlyCaptured++;
                        }
                    }

                if (newlyCaptured > 0) {
                    if (newlyCaptured > bonusThresholdP1) {
                        player1.score += newlyCaptured * bonusMultiplierP1;
                        bonusCountP1++;
                        if (bonusCountP1 == 3) bonusThresholdP1 = 5;
                        if (bonusCountP1 >= 5) bonusMultiplierP1 = 4;
                    }
                    else {
                        player1.score += newlyCaptured;
                    }
                    cout << "Multiplayer P1: Captured " << newlyCaptured << " tiles, Score: " << player1.score << endl;

                    if (player1.score >= 50 && (player1.score - newlyCaptured) < 50) player1.powerUps++;
                    if (player1.score >= 70 && (player1.score - newlyCaptured) < 70) player1.powerUps++;
                    if (player1.score >= 100 && (player1.score - newlyCaptured) < 100) player1.powerUps++;
                    if (player1.score >= 130 && (player1.score - newlyCaptured) < 130) player1.powerUps++;
                    if (player1.score >= 160 && ((player1.score - newlyCaptured) / 30 < player1.score / 30)) player1.powerUps++;
                    cout << "Multiplayer P1: Power-Ups: " << player1.powerUps << endl;
                }
            }

            if (player2.alive && grid[static_cast<int>(player2.y)][static_cast<int>(player2.x)] == 1) {
                player2.dx = player2.dy = 0;
                for (int i = 0; i < enemyCount; i++) {
                    int enemyY = static_cast<int>(enemies[i].y / ts);
                    int enemyX = static_cast<int>(enemies[i].x / ts);
                    if (enemyY >= 0 && enemyY < M && enemyX >= 0 && enemyX < N)
                        drop(enemyY, enemyX, grid);
                }

                int newlyCaptured = 0;
                for (int i = 0; i < M; i++)
                    for (int j = 0; j < N; j++) {
                        if (grid[i][j] == -1) {
                            grid[i][j] = 0;
                            tileOwner[i][j] = 0;
                        }
                        else if (grid[i][j] == 3 && tileOwner[i][j] == 2) {
                            grid[i][j] = 1;
                            tileOwner[i][j] = 2;
                            newlyCaptured++;
                        }
                    }

                if (newlyCaptured > 0) {
                    if (newlyCaptured > bonusThresholdP2) {
                        player2.score += newlyCaptured * bonusMultiplierP2;
                        bonusCountP2++;
                        if (bonusCountP2 == 3) bonusThresholdP2 = 5;
                        if (bonusCountP2 >= 5) bonusMultiplierP2 = 4;
                    }
                    else {
                        player2.score += newlyCaptured;
                    }
                    cout << "Multiplayer P2: Captured " << newlyCaptured << " tiles, Score: " << player2.score << endl;

                    if (player2.score >= 50 && (player2.score - newlyCaptured) < 50) player2.powerUps++;
                    if (player2.score >= 70 && (player2.score - newlyCaptured) < 70) player2.powerUps++;
                    if (player2.score >= 100 && (player2.score - newlyCaptured) < 100) player2.powerUps++;
                    if (player2.score >= 130 && (player2.score - newlyCaptured) < 130) player2.powerUps++;
                    if (player2.score >= 160 && ((player2.score - newlyCaptured) / 30 < player2.score / 30)) player2.powerUps++;
                    cout << "Multiplayer P2: Power-Ups: " << player2.powerUps << endl;
                }
            }
        }

        if (player1.frozen && player1.frozenClock.getElapsedTime().asSeconds() >= 3) player1.frozen = false;
        if (player2.frozen && player2.frozenClock.getElapsedTime().asSeconds() >= 3) player2.frozen = false;

        if (!player1.alive && !player2.alive && !gameEnded) {
            Game = false;
            gameEnded = true;
            string winner;
            if (player1.score > player2.score) {
                winner = "Player 1 Wins!";
                pl.updatePlayerWins(player1User, 1);
                pl.updatePlayerLosses(player2User, 1);
            }
            else if (player2.score > player1.score) {
                winner = "Player 2 Wins!";
                pl.updatePlayerWins(player2User, 1);
                pl.updatePlayerLosses(player1User, 1);
            }
            else {
                winner = "It's a Tie!";
                pl.updatePlayerLosses(player1User, 1);
                pl.updatePlayerLosses(player2User, 1);
            }
            winnerText.setString(winner);
        }

        window.clear();
        inventory.drawBackground(window);

        for (int i = 0; i < M; i++)
            for (int j = 0; j < N; j++) {
                if (grid[i][j] == 0) continue;
                sTile.setColor(Color::White);
                if (grid[i][j] == 1) {
                    sTile.setTextureRect(IntRect(0, 0, ts, ts));
                    if (tileOwner[i][j] == 1)
                        sTile.setColor(Color(135, 206, 250, 200));
                    else if (tileOwner[i][j] == 2)
                        sTile.setColor(Color(255, 99, 71, 200));
                }
                if (grid[i][j] == 2) {
                    sTile.setTextureRect(IntRect(54, 0, ts, ts));
                    sTile.setColor(player1.pathColor);
                }
                if (grid[i][j] == 3) {
                    sTile.setTextureRect(IntRect(54, 0, ts, ts));
                    sTile.setColor(player2.pathColor);
                }
                sTile.setPosition(j * ts + offsetX, i * ts + offsetY);
                window.draw(sTile);
            }

        if (player1.alive) {
            sTile.setColor(Color::White);
            sTile.setTextureRect(IntRect(36, 0, ts, ts));
            sTile.setPosition(static_cast<int>(player1.x) * ts + offsetX, static_cast<int>(player1.y) * ts + offsetY);
            window.draw(sTile);
        }

        if (player2.alive) {
            sTile.setColor(Color::White);
            sTile.setTextureRect(IntRect(72, 0, ts, ts));
            sTile.setPosition(static_cast<int>(player2.x) * ts + offsetX, static_cast<int>(player2.y) * ts + offsetY);
            window.draw(sTile);
        }

        sEnemy.rotate(10);
        for (int i = 0; i < enemyCount; i++) {
            if (enemies[i].frozen) sEnemy.setColor(Color::Cyan);
            else sEnemy.setColor(Color::White);
            sEnemy.setPosition(enemies[i].x + offsetX, enemies[i].y + offsetY);
            window.draw(sEnemy);
        }

        scoreTextP1.setString("P1 Score: " + to_string(player1.score));
        powerUpTextP1.setString("P1 Power-Ups: " + to_string(player1.powerUps));
        scoreTextP2.setString("P2 Score: " + to_string(player2.score));
        powerUpTextP2.setString("P2 Power-Ups: " + to_string(player2.powerUps));

        window.draw(scoreTextP1);
        window.draw(powerUpTextP1);
        window.draw(scoreTextP2);
        window.draw(powerUpTextP2);

        if (player1.showBackgroundMenu) {
            for (int i = 0; i < bgCount; i++) window.draw(backgroundOptions[i]);
        }

        if (!Game) {
            window.draw(sGameover);
            window.draw(winnerText);
        }

        window.display();
    }
}