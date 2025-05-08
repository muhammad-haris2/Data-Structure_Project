//////#include "game.h"
//////#include <SFML/Audio.hpp>
//////#include <time.h>
//////#include <string>
//////#include <sstream>
//////#include <iostream>
//////#include <algorithm>
//////#include <cctype>
//////
//////using namespace std;
//////using namespace sf;
//////
//////// Helper function to map background names to colors
//////Color getColorForBackground(const string& name) {
//////    string lowerName = name;
//////    transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
//////    if (lowerName == "blue") return Color(135, 206, 250); // Light blue for readability
//////    if (lowerName == "white") return Color::White;
//////    if (lowerName == "red") return Color::Red;
//////    return Color::White; // Default for unknown names
//////}
//////
//////// Enemy implementation
//////Enemy::Enemy() {
//////    x = y = 300; // Start near center
//////    dx = rand() % 3 - 1; // {-1, 0, 1}
//////    dy = rand() % 3 - 1;
//////    frozen = false;
//////    moveTimer = 0;
//////    // Ensure non-zero movement
//////    if (dx == 0 && dy == 0) {
//////        dx = rand() % 2 ? 1 : -1;
//////    }
//////}
//////
//////void Enemy::move(int grid[M][N], float deltaTime) {
//////    if (frozen && frozenClock.getElapsedTime().asSeconds() < 3) return;
//////    frozen = false;
//////
//////    moveTimer += deltaTime;
//////    if (moveTimer < 0.1f) return; // Move every 0.1 seconds
//////    moveTimer = 0;
//////
//////    int newX = x + dx;
//////    int newY = y + dy;
//////
//////    // Check boundaries and grid collision
//////    if (newX / ts >= 0 && newX / ts < N && newY / ts >= 0 && newY / ts < M) {
//////        if (grid[newY / ts][newX / ts] == 1) {
//////            dx = -dx;
//////            dy = -dy;
//////        }
//////        else {
//////            x = newX;
//////            y = newY;
//////        }
//////    }
//////    else {
//////        dx = -dx;
//////        dy = -dy;
//////    }
//////
//////    // Ensure non-zero movement
//////    if (dx == 0 && dy == 0) {
//////        dx = rand() % 2 ? 1 : -1;
//////    }
//////}
//////
//////void Enemy::activateFreeze() {
//////    frozen = true;
//////    frozenClock.restart();
//////}
//////
//////// Drop implementation (flood-fill to mark unreachable areas)
//////void drop(int y, int x, int grid[M][N]) {
//////    if (y < 0 || y >= M || x < 0 || x >= N) return;
//////    if (grid[y][x] == 0) grid[y][x] = -1;
//////    if (y > 0 && grid[y - 1][x] == 0) drop(y - 1, x, grid);
//////    if (y < M - 1 && grid[y + 1][x] == 0) drop(y + 1, x, grid);
//////    if (x > 0 && grid[y][x - 1] == 0) drop(y, x - 1, grid);
//////    if (x < N - 1 && grid[y][x + 1] == 0) drop(y, x + 1, grid);
//////}
//////
//////// Single-player game
//////void playXonixGame(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const string& currentUser, Inventory& inventory) {
//////    window.setTitle("Xonix Game");
//////    const int windowWidth = 800;
//////    const int windowHeight = 600;
//////
//////    // Dynamically calculate tile size and grid dimensions
//////    const int ts = 20; // Tile size (fixed for simplicity, can be adjusted)
//////    const int N = windowWidth / ts; // Number of columns
//////    const int M = windowHeight / ts; // Number of rows
//////	window.setSize(Vector2u(windowWidth, windowHeight));
//////    srand(time(0));
//////
//////    int grid[M][N] = { 0 };
//////
//////    Texture t1, t2, t3;
//////    if (!t1.loadFromFile("images/tiles.png") || !t2.loadFromFile("images/gameover.png") || !t3.loadFromFile("images/enemy.png")) {
//////        cout << "Error: Could not load game textures.\n";
//////        return;
//////    }
//////
//////    Sprite sTile(t1), sGameover(t2), sEnemy(t3);
//////    sGameover.setPosition(200.0f, 150.0f);
//////    sEnemy.setOrigin(20, 20);
//////
//////    Text scoreText("", font, 20);
//////    scoreText.setFillColor(Color::White);
//////    scoreText.setPosition(20.0f, 15.0f);
//////
//////    Text powerUpText("", font, 20);
//////    powerUpText.setFillColor(Color::Green);
//////    powerUpText.setPosition(20.0f, 40.0f);
//////
//////    // Background selection UI
//////    vector<Text> backgroundOptions;
//////    string namesString = inventory.getBackgroundNames();
//////    vector<string> bgNames;
//////    stringstream ss(namesString);
//////    string name;
//////    while (getline(ss, name)) {
//////        if (!name.empty()) {
//////            bgNames.push_back(name);
//////        }
//////    }
//////    for (size_t i = 0; i < bgNames.size(); ++i) {
//////        Text option;
//////        option.setFont(font);
//////        option.setString(bgNames[i]);
//////        option.setCharacterSize(18);
//////        option.setPosition(20.0f, 70.0f + i * 25.0f);
//////        option.setFillColor(i + 1 == inventory.getCurrentBackgroundID() ? getColorForBackground(bgNames[i]) : Color::White);
//////        backgroundOptions.push_back(option);
//////    }
//////
//////    int enemyCount = 4;
//////    Enemy enemies[10];
//////
//////    bool Game = true;
//////    PlayerState player;
//////    player.x = 10;
//////    player.y = 0; // Explicitly start on border
//////    float timer = 0, delay = 0.07;
//////    Clock clock;
//////
//////    int bonusCount = 0, bonusThreshold = 10, bonusMultiplier = 2;
//////
//////    for (int i = 0; i < M; i++)
//////        for (int j = 0; j < N; j++)
//////            if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
//////                grid[i][j] = 1;
//////
//////    // Start background music
//////    sf::Music& music = inventory.getCurrentSound();
//////    music.setLoop(true);
//////    music.play();
//////
//////    while (window.isOpen()) {
//////        float time = clock.getElapsedTime().asSeconds();
//////        clock.restart();
//////        timer += time;
//////
//////        Event e;
//////        while (window.pollEvent(e)) {
//////            if (e.type == Event::Closed) {
//////                music.stop();
//////                Player* p = pl.getPlayerByUsername(currentUser);
//////                if (p) {
//////                    pl.savePlayerStats(currentUser, player.score, player.powerUps, p->preferredThemeID, p->preferredSoundID);
//////                }
//////                else {
//////                    cerr << "Error: Player " << currentUser << " not found for saving stats.\n";
//////                }
//////                window.close();
//////                return;
//////            }
//////
//////            if (e.type == Event::KeyPressed) {
//////                if (e.key.code == Keyboard::Escape) {
//////                    if (!Game) {
//////                        music.stop();
//////                        Player* p = pl.getPlayerByUsername(currentUser);
//////                        if (p) {
//////                            pl.savePlayerStats(currentUser, player.score, player.powerUps, p->preferredThemeID, p->preferredSoundID);
//////                        }
//////                        else {
//////                            cerr << "Error: Player " << currentUser << " not found for saving stats.\n";
//////                        }
//////                        return;
//////                    }
//////                    else {
//////                        for (int i = 1; i < M - 1; i++)
//////                            for (int j = 1; j < N - 1; j++)
//////                                grid[i][j] = 0;
//////                        player = PlayerState();
//////                        player.x = 10;
//////                        player.y = 0; // Reset to border
//////                        Game = true;
//////                        bonusCount = 0;
//////                        bonusThreshold = 10;
//////                        bonusMultiplier = 2;
//////                        for (int i = 0; i < enemyCount; i++)
//////                            enemies[i] = Enemy();
//////                        music.stop();
//////                        music.play(); // Restart music after reset
//////                    }
//////                }
//////                if (e.key.code == Keyboard::Space && player.powerUps > 0 && Game) {
//////                    for (int i = 0; i < enemyCount; i++)
//////                        enemies[i].activateFreeze();
//////                    player.powerUps--;
//////                }
//////                if (e.key.code == Keyboard::B) {
//////                    player.showBackgroundMenu = !player.showBackgroundMenu;
//////                }
//////            }
//////
//////            if (e.type == Event::MouseButtonPressed && player.showBackgroundMenu) {
//////                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
//////                for (size_t i = 0; i < backgroundOptions.size(); ++i) {
//////                    if (backgroundOptions[i].getGlobalBounds().contains(mousePos)) {
//////                        inventory.setBackground(i + 1);
//////                        player.showBackgroundMenu = false;
//////                        for (size_t j = 0; j < backgroundOptions.size(); ++j) {
//////                            backgroundOptions[j].setFillColor(j + 1 == inventory.getCurrentBackgroundID() ? getColorForBackground(bgNames[j]) : Color::White);
//////                        }
//////                        break;
//////                    }
//////                }
//////            }
//////        }
//////
//////        if (Game && !player.frozen) {
//////            if (Keyboard::isKeyPressed(Keyboard::Left)) { player.dx = -1; player.dy = 0; }
//////            if (Keyboard::isKeyPressed(Keyboard::Right)) { player.dx = 1; player.dy = 0; }
//////            if (Keyboard::isKeyPressed(Keyboard::Up)) { player.dx = 0; player.dy = -1; }
//////            if (Keyboard::isKeyPressed(Keyboard::Down)) { player.dx = 0; player.dy = 1; }
//////
//////            if (timer > delay) {
//////                player.x += player.dx;
//////                player.y += player.dy;
//////
//////                if (player.x < 0) player.x = 0; if (player.x > N - 1) player.x = N - 1;
//////                if (player.y < 0) player.y = 0; if (player.y > M - 1) player.y = M - 1;
//////
//////                player.constructing = (grid[player.y][player.x] == 0);
//////                if (grid[player.y][player.x] == 2) Game = false;
//////                if (grid[player.y][player.x] == 0) {
//////                    grid[player.y][player.x] = 2;
//////                    player.capturedTiles++;
//////                }
//////                timer = 0;
//////            }
//////
//////            for (int i = 0; i < enemyCount; i++) enemies[i].move(grid, time);
//////
//////            if (grid[player.y][player.x] == 1) {
//////                player.dx = player.dy = 0;
//////
//////                for (int i = 0; i < enemyCount; i++)
//////                    drop(enemies[i].y / ts, enemies[i].x / ts, grid);
//////
//////                int newlyCaptured = 0;
//////                for (int i = 0; i < M; i++)
//////                    for (int j = 0; j < N; j++) {
//////                        if (grid[i][j] == -1) grid[i][j] = 0;
//////                        else if (grid[i][j] == 2) { grid[i][j] = 1; newlyCaptured++; }
//////                    }
//////
//////                if (newlyCaptured > 0) {
//////                    if (newlyCaptured > bonusThreshold) {
//////                        player.score += newlyCaptured * bonusMultiplier;
//////                        bonusCount++;
//////                        if (bonusCount == 3) bonusThreshold = 5;
//////                        if (bonusCount >= 5) bonusMultiplier = 4;
//////                    }
//////                    else {
//////                        player.score += newlyCaptured;
//////                    }
//////                    cout << "Single-player: Captured " << newlyCaptured << " tiles, Score: " << player.score << endl;
//////
//////                    if (player.score >= 50 && (player.score - newlyCaptured) < 50) player.powerUps++;
//////                    if (player.score >= 70 && (player.score - newlyCaptured) < 70) player.powerUps++;
//////                    if (player.score >= 100 && (player.score - newlyCaptured) < 100) player.powerUps++;
//////                    if (player.score >= 130 && (player.score - newlyCaptured) < 130) player.powerUps++;
//////                    if (player.score >= 160 && ((player.score - newlyCaptured) / 30 < player.score / 30)) player.powerUps++;
//////                    cout << "Single-player: Power-Ups: " << player.powerUps << endl;
//////                }
//////            }
//////
//////            for (int i = 0; i < enemyCount; i++)
//////                if (grid[enemies[i].y / ts][enemies[i].x / ts] == 2) Game = false;
//////        }
//////
//////        if (player.frozen && player.frozenClock.getElapsedTime().asSeconds() >= 3) {
//////            player.frozen = false;
//////        }
//////
//////        window.clear();
//////        inventory.drawBackground(window);
//////
//////        for (int i = 0; i < M; i++)
//////            for (int j = 0; j < N; j++) {
//////                if (grid[i][j] == 0) continue;
//////                if (grid[i][j] == 1) sTile.setTextureRect(IntRect(0, 0, ts, ts));
//////                if (grid[i][j] == 2) sTile.setTextureRect(IntRect(54, 0, ts, ts));
//////                sTile.setPosition(j * ts, i * ts);
//////                window.draw(sTile);
//////            }
//////
//////        sTile.setTextureRect(IntRect(36, 0, ts, ts));
//////        sTile.setPosition(player.x * ts, player.y * ts);
//////        window.draw(sTile);
//////
//////        sEnemy.rotate(10);
//////        for (int i = 0; i < enemyCount; i++) {
//////            if (enemies[i].frozen) sEnemy.setColor(Color::Cyan);
//////            else sEnemy.setColor(Color::White);
//////            sEnemy.setPosition(enemies[i].x, enemies[i].y);
//////            window.draw(sEnemy);
//////        }
//////
//////        scoreText.setString("Score: " + to_string(player.score));
//////        powerUpText.setString("Power-Ups: " + to_string(player.powerUps));
//////        window.draw(scoreText);
//////        window.draw(powerUpText);
//////
//////        if (player.showBackgroundMenu) {
//////            for (const auto& option : backgroundOptions) {
//////                window.draw(option);
//////            }
//////        }
//////
//////        if (!Game) window.draw(sGameover);
//////
//////        window.display();
//////    }
//////}
//////
//////// Multiplayer mode
//////void playMultiplayerXonixGame(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const string& player1User, const string& player2User, Inventory& inventory) {
//////    window.setTitle("Xonix Multiplayer Game");
//////
//////    srand(time(0));
//////
//////    int grid[M][N] = { 0 };
//////    int tileOwner[M][N] = { 0 };
//////
//////    Texture t1, t2, t3;
//////    if (!t1.loadFromFile("images/tiles.png") || !t2.loadFromFile("images/gameover.png") || !t3.loadFromFile("images/enemy.png")) {
//////        cout << "Error: Could not load game textures.\n";
//////        return;
//////    }
//////
//////    Sprite sTile(t1), sGameover(t2), sEnemy(t3);
//////    sGameover.setPosition(200.0f, 150.0f);
//////    sEnemy.setOrigin(20, 20);
//////
//////    Text scoreTextP1("", font, 20);
//////    scoreTextP1.setFillColor(Color::White);
//////    scoreTextP1.setPosition(20.0f, 15.0f);
//////
//////    Text powerUpTextP1("", font, 20);
//////    powerUpTextP1.setFillColor(Color::Green);
//////    powerUpTextP1.setPosition(20.0f, 40.0f);
//////
//////    Text scoreTextP2("", font, 20);
//////    scoreTextP2.setFillColor(Color::White);
//////    scoreTextP2.setPosition(static_cast<float>(N * ts - 190), 15.0f);
//////
//////    Text powerUpTextP2("", font, 20);
//////    powerUpTextP2.setFillColor(Color::Green);
//////    powerUpTextP2.setPosition(static_cast<float>(N * ts - 190), 40.0f);
//////
//////    Text winnerText("", font, 30);
//////    winnerText.setFillColor(Color::Yellow);
//////    winnerText.setPosition(350.0f, 150.0f);
//////
//////    vector<Text> backgroundOptions;
//////    string namesString = inventory.getBackgroundNames();
//////    vector<string> bgNames;
//////    stringstream ss(namesString);
//////    string name;
//////    while (getline(ss, name)) {
//////        if (!name.empty()) {
//////            bgNames.push_back(name);
//////        }
//////    }
//////    for (size_t i = 0; i < bgNames.size(); ++i) {
//////        Text option;
//////        option.setFont(font);
//////        option.setString(bgNames[i]);
//////        option.setCharacterSize(18);
//////        option.setPosition(20.0f, 70.0f + i * 25.0f);
//////        option.setFillColor(i + 1 == inventory.getCurrentBackgroundID() ? getColorForBackground(bgNames[i]) : Color::White);
//////        backgroundOptions.push_back(option);
//////    }
//////
//////    int enemyCount = 4;
//////    Enemy enemies[10];
//////
//////    bool Game = true;
//////    PlayerState player1, player2;
//////    player1.x = 10;
//////    player1.y = 0;
//////    player1.pathColor = Color::Blue;
//////    player2.x = N - 10;
//////    player2.y = 0;
//////    player2.pathColor = Color::Red;
//////
//////    float timer = 0, delay = 0.07;
//////    Clock clock;
//////
//////    int bonusCountP1 = 0, bonusThresholdP1 = 10, bonusMultiplierP1 = 2;
//////    int bonusCountP2 = 0, bonusThresholdP2 = 10, bonusMultiplierP2 = 2;
//////
//////    for (int i = 0; i < M; i++)
//////        for (int j = 0; j < N; j++)
//////            if (i == 0 || j == 0 || i == M - 1 || j == N - 1) {
//////                grid[i][j] = 1;
//////                tileOwner[i][j] = 0;
//////            }
//////
//////    // Start background music
//////    sf::Music& music = inventory.getCurrentSound();
//////    music.setLoop(true);
//////    music.play();
//////
//////    while (window.isOpen()) {
//////        float time = clock.getElapsedTime().asSeconds();
//////        clock.restart();
//////        timer += time;
//////
//////        Event e;
//////        while (window.pollEvent(e)) {
//////            if (e.type == Event::Closed) {
//////                music.stop();
//////                Player* p1 = pl.getPlayerByUsername(player1User);
//////                if (p1) {
//////                    pl.savePlayerStats(player1User, player1.score, player1.powerUps, p1->preferredThemeID, p1->preferredSoundID);
//////                }
//////                else {
//////                    cerr << "Error: Player " << player1User << " not found for saving stats.\n";
//////                }
//////                Player* p2 = pl.getPlayerByUsername(player2User);
//////                if (p2) {
//////                    pl.savePlayerStats(player2User, player2.score, player2.powerUps, p2->preferredThemeID, p2->preferredSoundID);
//////                }
//////                else {
//////                    cerr << "Error: Player " << player2User << " not found for saving stats.\n";
//////                }
//////                window.close();
//////                return;
//////            }
//////
//////            if (e.type == Event::KeyPressed) {
//////                if (e.key.code == Keyboard::Escape) {
//////                    if (!Game || (!player1.alive && !player2.alive)) {
//////                        music.stop();
//////                        Player* p1 = pl.getPlayerByUsername(player1User);
//////                        if (p1) {
//////                            pl.savePlayerStats(player1User, player1.score, player1.powerUps, p1->preferredThemeID, p1->preferredSoundID);
//////                        }
//////                        else {
//////                            cerr << "Error: Player " << player1User << " not found for saving stats.\n";
//////                        }
//////                        Player* p2 = pl.getPlayerByUsername(player2User);
//////                        if (p2) {
//////                            pl.savePlayerStats(player2User, player2.score, player2.powerUps, p2->preferredThemeID, p2->preferredSoundID);
//////                        }
//////                        else {
//////                            cerr << "Error: Player " << player2User << " not found for saving stats.\n";
//////                        }
//////                        return;
//////                    }
//////                    else {
//////                        for (int i = 0; i < M; i++)
//////                            for (int j = 0; j < N; j++) {
//////                                if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
//////                                    grid[i][j] = 1;
//////                                else
//////                                    grid[i][j] = 0;
//////                                tileOwner[i][j] = 0;
//////                            }
//////                        player1 = PlayerState();
//////                        player2 = PlayerState();
//////                        player1.x = 10;
//////                        player1.y = 0;
//////                        player1.pathColor = Color::Blue;
//////                        player2.x = N - 10;
//////                        player2.y = 0;
//////                        player2.pathColor = Color::Red;
//////                        Game = true;
//////                        bonusCountP1 = bonusCountP2 = 0;
//////                        bonusThresholdP1 = bonusThresholdP2 = 10;
//////                        bonusMultiplierP1 = bonusMultiplierP2 = 2;
//////                        for (int i = 0; i < enemyCount; i++)
//////                            enemies[i] = Enemy();
//////                        music.stop();
//////                        music.play(); // Restart music after reset
//////                    }
//////                }
//////                if (e.key.code == Keyboard::Space && player1.powerUps > 0 && Game && player1.alive) {
//////                    for (int i = 0; i < enemyCount; i++)
//////                        enemies[i].activateFreeze();
//////                    player2.frozen = true;
//////                    player2.frozenClock.restart();
//////                    player1.powerUps--;
//////                }
//////                if (e.key.code == Keyboard::Return && player2.powerUps > 0 && Game && player2.alive) {
//////                    for (int i = 0; i < enemyCount; i++)
//////                        enemies[i].activateFreeze();
//////                    player1.frozen = true;
//////                    player1.frozenClock.restart();
//////                    player2.powerUps--;
//////                }
//////                if (e.key.code == Keyboard::B) {
//////                    player1.showBackgroundMenu = !player1.showBackgroundMenu;
//////                    player2.showBackgroundMenu = player1.showBackgroundMenu;
//////                }
//////            }
//////
//////            if (e.type == Event::MouseButtonPressed && player1.showBackgroundMenu) {
//////                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
//////                for (size_t i = 0; i < backgroundOptions.size(); ++i) {
//////                    if (backgroundOptions[i].getGlobalBounds().contains(mousePos)) {
//////                        inventory.setBackground(i + 1);
//////                        player1.showBackgroundMenu = false;
//////                        player2.showBackgroundMenu = false;
//////                        for (size_t j = 0; j < backgroundOptions.size(); ++j) {
//////                            backgroundOptions[j].setFillColor(j + 1 == inventory.getCurrentBackgroundID() ? getColorForBackground(bgNames[j]) : Color::White);
//////                        }
//////                        break;
//////                    }
//////                }
//////            }
//////        }
//////
//////        if (Game) {
//////            if (!player1.frozen && player1.alive) {
//////                if (Keyboard::isKeyPressed(Keyboard::A)) { player1.dx = -1; player1.dy = 0; }
//////                if (Keyboard::isKeyPressed(Keyboard::D)) { player1.dx = 1; player1.dy = 0; }
//////                if (Keyboard::isKeyPressed(Keyboard::W)) { player1.dx = 0; player1.dy = -1; }
//////                if (Keyboard::isKeyPressed(Keyboard::S)) { player1.dx = 0; player1.dy = 1; }
//////            }
//////
//////            if (!player2.frozen && player2.alive) {
//////                if (Keyboard::isKeyPressed(Keyboard::Left)) { player2.dx = -1; player2.dy = 0; }
//////                if (Keyboard::isKeyPressed(Keyboard::Right)) { player2.dx = 1; player2.dy = 0; }
//////                if (Keyboard::isKeyPressed(Keyboard::Up)) { player2.dx = 0; player2.dy = -1; }
//////                if (Keyboard::isKeyPressed(Keyboard::Down)) { player2.dx = 0; player2.dy = 1; }
//////            }
//////
//////            if (timer > delay) {
//////                if (player1.alive && !player1.frozen) {
//////                    player1.x += player1.dx;
//////                    player1.y += player1.dy;
//////
//////                    if (player1.x < 0) player1.x = 0; if (player1.x > N - 1) player1.x = N - 1;
//////                    if (player1.y < 0) player1.y = 0; if (player1.y > M - 1) player1.y = M - 1;
//////
//////                    player1.constructing = (grid[player1.y][player1.x] == 0);
//////                    if (grid[player1.y][player1.x] == 2 && tileOwner[player1.y][player1.x] == 1) {
//////                        player1.alive = false;
//////                    }
//////                    else if (grid[player1.y][player1.x] == 3 ||
//////                        (grid[player1.y][player1.x] == 1 && tileOwner[player1.y][player1.x] == 2)) {
//////                        player1.alive = false;
//////                    }
//////                    else if (grid[player1.y][player1.x] == 0) {
//////                        grid[player1.y][player1.x] = 2;
//////                        tileOwner[player1.y][player1.x] = 1;
//////                        player1.capturedTiles++;
//////                    }
//////                }
//////
//////                if (player2.alive && !player2.frozen) {
//////                    player2.x += player2.dx;
//////                    player2.y += player2.dy;
//////
//////                    if (player2.x < 0) player2.x = 0; if (player2.x > N - 1) player2.x = N - 1;
//////                    if (player2.y < 0) player2.y = 0; if (player2.y > M - 1) player2.y = M - 1;
//////
//////                    player2.constructing = (grid[player2.y][player2.x] == 0);
//////                    if (grid[player2.y][player2.x] == 3 && tileOwner[player2.y][player2.x] == 2) {
//////                        player2.alive = false;
//////                    }
//////                    else if (grid[player2.y][player2.x] == 2 ||
//////                        (grid[player2.y][player2.x] == 1 && tileOwner[player2.y][player2.x] == 1)) {
//////                        player2.alive = false;
//////                    }
//////                    else if (grid[player2.y][player2.x] == 0) {
//////                        grid[player2.y][player2.x] = 3;
//////                        tileOwner[player2.y][player2.x] = 2;
//////                        player2.capturedTiles++;
//////                    }
//////                }
//////
//////                if (player1.alive && player2.alive && player1.x == player2.x && player1.y == player2.y) {
//////                    if (player1.constructing && player2.constructing) {
//////                        player1.alive = false;
//////                        player2.alive = false;
//////                    }
//////                    else if (player1.constructing) {
//////                        player1.alive = false;
//////                    }
//////                    else if (player2.constructing) {
//////                        player2.alive = false;
//////                    }
//////                    else {
//////                        player1.alive = false;
//////                        player2.alive = false;
//////                    }
//////                }
//////
//////                timer = 0;
//////            }
//////
//////            for (int i = 0; i < enemyCount; i++) {
//////                enemies[i].move(grid, time);
//////                if (enemies[i].y / ts < M && enemies[i].x / ts < N) {
//////                    int enemyGridVal = grid[enemies[i].y / ts][enemies[i].x / ts];
//////                    if (enemyGridVal == 2 && player1.alive && player1.constructing) player1.alive = false;
//////                    if (enemyGridVal == 3 && player2.alive && player2.constructing) player2.alive = false;
//////                }
//////            }
//////
//////            if (player1.alive && grid[player1.y][player1.x] == 1) {
//////                player1.dx = player1.dy = 0;
//////                for (int i = 0; i < enemyCount; i++)
//////                    drop(enemies[i].y / ts, enemies[i].x / ts, grid);
//////
//////                int newlyCaptured = 0;
//////                for (int i = 0; i < M; i++)
//////                    for (int j = 0; j < N; j++) {
//////                        if (grid[i][j] == -1) {
//////                            grid[i][j] = 0;
//////                            tileOwner[i][j] = 0;
//////                        }
//////                        else if (grid[i][j] == 2 && tileOwner[i][j] == 1) {
//////                            grid[i][j] = 1;
//////                            tileOwner[i][j] = 1;
//////                            newlyCaptured++;
//////                        }
//////                    }
//////
//////                if (newlyCaptured > 0) {
//////                    if (newlyCaptured > bonusThresholdP1) {
//////                        player1.score += newlyCaptured * bonusMultiplierP1;
//////                        bonusCountP1++;
//////                        if (bonusCountP1 == 3) bonusThresholdP1 = 5;
//////                        if (bonusCountP1 >= 5) bonusMultiplierP1 = 4;
//////                    }
//////                    else {
//////                        player1.score += newlyCaptured;
//////                    }
//////                    cout << "Multiplayer P1: Captured " << newlyCaptured << " tiles, Score: " << player1.score << endl;
//////
//////                    if (player1.score >= 50 && (player1.score - newlyCaptured) < 50) player1.powerUps++;
//////                    if (player1.score >= 70 && (player1.score - newlyCaptured) < 70) player1.powerUps++;
//////                    if (player1.score >= 100 && (player1.score - newlyCaptured) < 100) player1.powerUps++;
//////                    if (player1.score >= 130 && (player1.score - newlyCaptured) < 130) player1.powerUps++;
//////                    if (player1.score >= 160 && ((player1.score - newlyCaptured) / 30 < player1.score / 30)) player1.powerUps++;
//////                    cout << "Multiplayer P1: Power-Ups: " << player1.powerUps << endl;
//////                }
//////            }
//////
//////            if (player2.alive && grid[player2.y][player2.x] == 1) {
//////                player2.dx = player2.dy = 0;
//////                for (int i = 0; i < enemyCount; i++)
//////                    drop(enemies[i].y / ts, enemies[i].x / ts, grid);
//////
//////                int newlyCaptured = 0;
//////                for (int i = 0; i < M; i++)
//////                    for (int j = 0; j < N; j++) {
//////                        if (grid[i][j] == -1) {
//////                            grid[i][j] = 0;
//////                            tileOwner[i][j] = 0;
//////                        }
//////                        else if (grid[i][j] == 3 && tileOwner[i][j] == 2) {
//////                            grid[i][j] = 1;
//////                            tileOwner[i][j] = 2;
//////                            newlyCaptured++;
//////                        }
//////                    }
//////
//////                if (newlyCaptured > 0) {
//////                    if (newlyCaptured > bonusThresholdP2) {
//////                        player2.score += newlyCaptured * bonusMultiplierP2;
//////                        bonusCountP2++;
//////                        if (bonusCountP2 == 3) bonusThresholdP2 = 5;
//////                        if (bonusCountP2 >= 5) bonusMultiplierP2 = 4;
//////                    }
//////                    else {
//////                        player2.score += newlyCaptured;
//////                    }
//////                    cout << "Multiplayer P2: Captured " << newlyCaptured << " tiles, Score: " << player2.score << endl;
//////
//////                    if (player2.score >= 50 && (player2.score - newlyCaptured) < 50) player2.powerUps++;
//////                    if (player2.score >= 70 && (player2.score - newlyCaptured) < 70) player2.powerUps++;
//////                    if (player2.score >= 100 && (player2.score - newlyCaptured) < 100) player2.powerUps++;
//////                    if (player2.score >= 130 && (player2.score - newlyCaptured) < 130) player2.powerUps++;
//////                    if (player2.score >= 160 && ((player2.score - newlyCaptured) / 30 < player2.score / 30)) player2.powerUps++;
//////                    cout << "Multiplayer P2: Power-Ups: " << player2.powerUps << endl;
//////                }
//////            }
//////        }
//////
//////        if (player1.frozen && player1.frozenClock.getElapsedTime().asSeconds() >= 3) {
//////            player1.frozen = false;
//////        }
//////        if (player2.frozen && player2.frozenClock.getElapsedTime().asSeconds() >= 3) {
//////            player2.frozen = false;
//////        }
//////
//////        if (!player1.alive && !player2.alive) {
//////            Game = false;
//////            string winner;
//////            if (player1.score > player2.score)
//////                winner = "Player 1 Wins!";
//////            else if (player2.score > player1.score)
//////                winner = "Player 2 Wins!";
//////            else
//////                winner = "It's a Tie!";
//////            winnerText.setString(winner);
//////        }
//////
//////        window.clear();
//////        inventory.drawBackground(window);
//////
//////        for (int i = 0; i < M; i++)
//////            for (int j = 0; j < N; j++) {
//////                if (grid[i][j] == 0) continue;
//////                sTile.setColor(Color::White);
//////                if (grid[i][j] == 1) {
//////                    sTile.setTextureRect(IntRect(0, 0, ts, ts));
//////                    if (tileOwner[i][j] == 1)
//////                        sTile.setColor(Color(135, 206, 250, 200));
//////                    else if (tileOwner[i][j] == 2)
//////                        sTile.setColor(Color(255, 99, 71, 200));
//////                }
//////                if (grid[i][j] == 2) {
//////                    sTile.setTextureRect(IntRect(54, 0, ts, ts));
//////                    sTile.setColor(player1.pathColor);
//////                }
//////                if (grid[i][j] == 3) {
//////                    sTile.setTextureRect(IntRect(54, 0, ts, ts));
//////                    sTile.setColor(player2.pathColor);
//////                }
//////                sTile.setPosition(j * ts, i * ts);
//////                window.draw(sTile);
//////            }
//////
//////        if (player1.alive) {
//////            sTile.setColor(Color::White);
//////            sTile.setTextureRect(IntRect(36, 0, ts, ts));
//////            sTile.setPosition(player1.x * ts, player1.y * ts);
//////            window.draw(sTile);
//////        }
//////
//////        if (player2.alive) {
//////            sTile.setColor(Color::White);
//////            sTile.setTextureRect(IntRect(72, 0, ts, ts));
//////            sTile.setPosition(player2.x * ts, player2.y * ts);
//////            window.draw(sTile);
//////        }
//////
//////        sEnemy.rotate(10);
//////        for (int i = 0; i < enemyCount; i++) {
//////            if (enemies[i].frozen) sEnemy.setColor(Color::Cyan);
//////            else sEnemy.setColor(Color::White);
//////            sEnemy.setPosition(enemies[i].x, enemies[i].y);
//////            window.draw(sEnemy);
//////        }
//////
//////        scoreTextP1.setString("P1 Score: " + to_string(player1.score));
//////        powerUpTextP1.setString("P1 Power-Ups: " + to_string(player1.powerUps));
//////        scoreTextP2.setString("P2 Score: " + to_string(player2.score));
//////        powerUpTextP2.setString("P2 Power-Ups: " + to_string(player2.powerUps));
//////
//////        window.draw(scoreTextP1);
//////        window.draw(powerUpTextP1);
//////        window.draw(scoreTextP2);
//////        window.draw(powerUpTextP2);
//////
//////        if (player1.showBackgroundMenu) {
//////            for (const auto& option : backgroundOptions) {
//////                window.draw(option);
//////            }
//////        }
//////
//////        if (!Game) {
//////            window.draw(sGameover);
//////            window.draw(winnerText);
//////        }
//////
//////        window.display();
//////    }
//////}
//////
//////
//////#include "game.h"
//////#include <SFML/Audio.hpp>
//////#include <time.h>
//////#include <string>
//////#include <sstream>
//////#include <iostream>
//////#include <algorithm>
//////#include <cctype>
//////
//////using namespace std;
//////using namespace sf;
//////
////// Helper function to map background names to colors
//////Color getColorForBackground(const string& name) {
//////    string lowerName = name;
//////    transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
//////    if (lowerName == "blue") return Color(135, 206, 250); // Light blue for readability
//////    if (lowerName == "white") return Color::White;
//////    if (lowerName == "red") return Color::Red;
//////    return Color::White; // Default for unknown names
//////}
//////
////// Helper function to create a styled button
//////RectangleShape createButton(float x, float y, float width, float height, Color fillColor) {
//////    RectangleShape button(Vector2f(width, height));
//////    button.setPosition(x, y);
//////    button.setFillColor(fillColor);
//////    button.setOutlineColor(Color(50, 50, 50));
//////    button.setOutlineThickness(2);
//////    return button;
//////}
//////
////// Enemy implementation
//////Enemy::Enemy(int grid[M][N], int tileSize) {
//////     Random position within playable area (not on borders)
//////    int maxX = N - 2, maxY = M - 2;
//////    do {
//////        x = (rand() % maxX + 1) * tileSize + tileSize / 2.0f;
//////        y = (rand() % maxY + 1) * tileSize + tileSize / 2.0f;
//////    } while (grid[static_cast<int>(y / tileSize)][static_cast<int>(x / tileSize)] != 0);
//////
//////     Random initial direction with normalized speed
//////    float angle = static_cast<float>(rand()) / RAND_MAX * 2 * 3.14159f;
//////    dx = cos(angle);
//////    dy = sin(angle);
//////    frozen = false;
//////    speed = 100.0f; // Default speed (pixels/second), adjustable by level
//////}
//////
//////void Enemy::move(int grid[M][N], float deltaTime, int tileSize) {
//////    if (frozen && frozenClock.getElapsedTime().asSeconds() < 3) return;
//////    frozen = false;
//////
//////     Move smoothly based on speed and deltaTime
//////    float moveX = dx * speed * deltaTime;
//////    float moveY = dy * speed * deltaTime;
//////    float newX = x + moveX;
//////    float newY = y + moveY;
//////
//////     Check boundaries and grid collision
//////    int gridX = static_cast<int>(newX / tileSize);
//////    int gridY = static_cast<int>(newY / tileSize);
//////    bool collide = false;
//////
//////    if (gridX < 0 || gridX >= N || gridY < 0 || gridY >= M) {
//////        collide = true;
//////    }
//////    else if (grid[gridY][gridX] == 1) {
//////        collide = true;
//////    }
//////
//////    if (collide) {
//////         Bounce with slight randomization
//////        float angle = atan2(dy, dx) + (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.5f;
//////        dx = cos(angle);
//////        dy = sin(angle);
//////         Ensure non-zero movement
//////        if (abs(dx) < 0.1f && abs(dy) < 0.1f) {
//////            dx = rand() % 2 ? 1.0f : -1.0f;
//////            dy = rand() % 2 ? 1.0f : -1.0f;
//////            float len = sqrt(dx * dx + dy * dy);
//////            dx /= len;
//////            dy /= len;
//////        }
//////    }
//////    else {
//////        x = newX;
//////        y = newY;
//////    }
//////
//////     Keep within bounds
//////    if (x < tileSize / 2.0f) x = tileSize / 2.0f;
//////    if (x > (N - 0.5f) * tileSize) x = (N - 0.5f) * tileSize;
//////    if (y < tileSize / 2.0f) y = tileSize / 2.0f;
//////    if (y > (M - 0.5f) * tileSize) y = (M - 0.5f) * tileSize;
//////}
//////
//////void Enemy::activateFreeze() {
//////    frozen = true;
//////    frozenClock.restart();
//////}
//////
////// Drop implementation (flood-fill to mark unreachable areas)
//////void drop(int y, int x, int grid[M][N]) {
//////    if (y < 0 || y >= M || x < 0 || x >= N) return;
//////    if (grid[y][x] == 0) grid[y][x] = -1;
//////    if (y > 0 && grid[y - 1][x] == 0) drop(y - 1, x, grid);
//////    if (y < M - 1 && grid[y + 1][x] == 0) drop(y + 1, x, grid);
//////    if (x > 0 && grid[y][x - 1] == 0) drop(y, x - 1, grid);
//////    if (x < N - 1 && grid[y][x + 1] == 0) drop(y, x + 1, grid);
//////}
//////
////// Level selection struct
//////struct Level {
//////    string name;
//////    int enemyCount;
//////    float enemySpeed;
//////};
//////
////// Single-player level selection
//////int selectLevel(RenderWindow& window, Font& font, Inventory& inventory) {
//////    window.setTitle("Select Level");
//////
//////     Level definitions
//////    vector<Level> levels = {
//////        {"Easy", 2, 50.0f},
//////        {"Medium", 4, 100.0f},
//////        {"Hard", 6, 150.0f}
//////    };
//////
//////     UI elements
//////    Text title("Select Level", font, 36);
//////    title.setFillColor(Color::White);
//////    title.setStyle(Text::Bold);
//////    title.setPosition(330, 40);
//////    title.setOutlineColor(Color::Black);
//////    title.setOutlineThickness(1);
//////
//////    vector<RectangleShape> levelButtonBgs;
//////    vector<Text> levelButtons;
//////    for (size_t i = 0; i < levels.size(); ++i) {
//////        RectangleShape bg = createButton(320, 150 + i * 60, 160, 40, Color(50, 150, 50));
//////        Text button(levels[i].name, font, 22);
//////        button.setFillColor(Color::White);
//////        button.setPosition(350, 155 + i * 60);
//////        levelButtonBgs.push_back(bg);
//////        levelButtons.push_back(button);
//////    }
//////
//////    while (window.isOpen()) {
//////        Event event;
//////        while (window.pollEvent(event)) {
//////            if (event.type == Event::Closed) {
//////                window.close();
//////                return -1; // Indicate closure
//////            }
//////
//////            if (event.type == Event::MouseButtonPressed) {
//////                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
//////                for (size_t i = 0; i < levelButtons.size(); ++i) {
//////                    if (levelButtons[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
//////                        return static_cast<int>(i); // Return level index
//////                    }
//////                }
//////            }
//////        }
//////
//////         Hover effects
//////        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
//////        for (size_t i = 0; i < levelButtonBgs.size(); ++i) {
//////            levelButtonBgs[i].setFillColor(levelButtons[i].getGlobalBounds().contains(mousePos.x, mousePos.y) ? Color(70, 180, 70) : Color(50, 150, 50));
//////        }
//////
//////        window.clear();
//////        inventory.drawBackground(window);
//////        window.draw(title);
//////        for (size_t i = 0; i < levelButtons.size(); ++i) {
//////            window.draw(levelButtonBgs[i]);
//////            window.draw(levelButtons[i]);
//////        }
//////        window.display();
//////    }
//////    return -1; // Fallback
//////}
//////
////// Single-player game
//////void playXonixGame(PlayerList& pl, RenderWindow& window, Font& font, const string& currentUser, Inventory& inventory) {
//////    window.setTitle("Xonix Game");
//////    const int windowWidth = 800;
//////    const int windowHeight = 600;
//////
//////     Dynamically calculate tile size and grid dimensions
//////    const int ts = 20; // Tile size
//////    const int N = windowWidth / ts; // Number of columns
//////    const int M = windowHeight / ts; // Number of rows
//////    window.setSize(Vector2u(windowWidth, windowHeight));
//////    srand(time(0));
//////
//////     Level selection
//////    int levelIndex = selectLevel(window, font, inventory);
//////    if (levelIndex == -1) return; // Window closed
//////
//////    vector<Level> levels = {
//////        {"Easy", 2, 50.0f},
//////        {"Medium", 4, 100.0f},
//////        {"Hard", 6, 150.0f}
//////    };
//////    int enemyCount = levels[levelIndex].enemyCount;
//////    float enemySpeed = levels[levelIndex].enemySpeed;
//////
//////    int grid[M][N] = { 0 };
//////
//////    Texture t1, t2, t3;
//////    if (!t1.loadFromFile("images/tiles.png") || !t2.loadFromFile("images/gameover.png") || !t3.loadFromFile("images/enemy.png")) {
//////        cout << "Error: Could not load game textures.\n";
//////        return;
//////    }
//////
//////    Sprite sTile(t1), sGameover(t2), sEnemy(t3);
//////    sGameover.setPosition(200.0f, 150.0f);
//////    sEnemy.setOrigin(20, 20);
//////
//////    Text scoreText("", font, 20);
//////    scoreText.setFillColor(Color::White);
//////    scoreText.setPosition(20.0f, 15.0f);
//////
//////    Text powerUpText("", font, 20);
//////    powerUpText.setFillColor(Color::Green);
//////    powerUpText.setPosition(20.0f, 40.0f);
//////
//////     Background selection UI
//////    vector<Text> backgroundOptions;
//////    string namesString = inventory.getBackgroundNames();
//////    vector<string> bgNames;
//////    stringstream ss(namesString);
//////    string name;
//////    while (getline(ss, name)) {
//////        if (!name.empty()) {
//////            bgNames.push_back(name);
//////        }
//////    }
//////    for (size_t i = 0; i < bgNames.size(); ++i) {
//////        Text option;
//////        option.setFont(font);
//////        option.setString(bgNames[i]);
//////        option.setCharacterSize(18);
//////        option.setPosition(20.0f, 70.0f + i * 25.0f);
//////        option.setFillColor(i + 1 == inventory.getCurrentBackgroundID() ? getColorForBackground(bgNames[i]) : Color::White);
//////        backgroundOptions.push_back(option);
//////    }
//////
//////    Enemy enemies[10];
//////    for (int i = 0; i < enemyCount; ++i) {
//////        enemies[i] = Enemy(grid, ts);
//////        enemies[i].speed = enemySpeed;
//////    }
//////
//////    bool Game = true;
//////    PlayerState player;
//////    player.x = 10;
//////    player.y = 0; // Start on border
//////    float timer = 0, delay = 0.07;
//////    Clock clock;
//////
//////    int bonusCount = 0, bonusThreshold = 10, bonusMultiplier = 2;
//////
//////    for (int i = 0; i < M; i++)
//////        for (int j = 0; j < N; j++)
//////            if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
//////                grid[i][j] = 1;
//////
//////     Start background music
//////    Music& music = inventory.getCurrentSound();
//////    music.setLoop(true);
//////    music.play();
//////
//////    while (window.isOpen()) {
//////        float time = clock.getElapsedTime().asSeconds();
//////        clock.restart();
//////        timer += time;
//////
//////        Event e;
//////        while (window.pollEvent(e)) {
//////            if (e.type == Event::Closed) {
//////                music.stop();
//////                Player* p = pl.getPlayerByUsername(currentUser);
//////                if (p) {
//////                    pl.savePlayerStats(currentUser, player.score, player.powerUps, p->preferredThemeID, p->preferredSoundID);
//////                }
//////                else {
//////                    cerr << "Error: Player " << currentUser << " not found for saving stats.\n";
//////                }
//////                window.close();
//////                return;
//////            }
//////
//////            if (e.type == Event::KeyPressed) {
//////                if (e.key.code == Keyboard::Escape) {
//////                    if (!Game) {
//////                        music.stop();
//////                        Player* p = pl.getPlayerByUsername(currentUser);
//////                        if (p) {
//////                            pl.savePlayerStats(currentUser, player.score, player.powerUps, p->preferredThemeID, p->preferredSoundID);
//////                        }
//////                        else {
//////                            cerr << "Error: Player " << currentUser << " not found for saving stats.\n";
//////                        }
//////                        return;
//////                    }
//////                    else {
//////                        for (int i = 1; i < M - 1; i++)
//////                            for (int j = 1; j < N - 1; j++)
//////                                grid[i][j] = 0;
//////                        player = PlayerState();
//////                        player.x = 10;
//////                        player.y = 0; // Reset to border
//////                        Game = true;
//////                        bonusCount = 0;
//////                        bonusThreshold = 10;
//////                        bonusMultiplier = 2;
//////                        for (int i = 0; i < enemyCount; i++) {
//////                            enemies[i] = Enemy(grid, ts);
//////                            enemies[i].speed = enemySpeed;
//////                        }
//////                        music.stop();
//////                        music.play(); // Restart music after reset
//////                    }
//////                }
//////                if (e.key.code == Keyboard::Space && player.powerUps > 0 && Game) {
//////                    for (int i = 0; i < enemyCount; i++)
//////                        enemies[i].activateFreeze();
//////                    player.powerUps--;
//////                }
//////                if (e.key.code == Keyboard::B) {
//////                    player.showBackgroundMenu = !player.showBackgroundMenu;
//////                }
//////            }
//////
//////            if (e.type == Event::MouseButtonPressed && player.showBackgroundMenu) {
//////                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
//////                for (size_t i = 0; i < backgroundOptions.size(); ++i) {
//////                    if (backgroundOptions[i].getGlobalBounds().contains(mousePos)) {
//////                        inventory.setBackground(i + 1);
//////                        player.showBackgroundMenu = false;
//////                        for (size_t j = 0; j < backgroundOptions.size(); ++j) {
//////                            backgroundOptions[j].setFillColor(j + 1 == inventory.getCurrentBackgroundID() ? getColorForBackground(bgNames[j]) : Color::White);
//////                        }
//////                        break;
//////                    }
//////                }
//////            }
//////        }
//////
//////        if (Game && !player.frozen) {
//////            if (Keyboard::isKeyPressed(Keyboard::Left)) { player.dx = -1; player.dy = 0; }
//////            if (Keyboard::isKeyPressed(Keyboard::Right)) { player.dx = 1; player.dy = 0; }
//////            if (Keyboard::isKeyPressed(Keyboard::Up)) { player.dx = 0; player.dy = -1; }
//////            if (Keyboard::isKeyPressed(Keyboard::Down)) { player.dx = 0; player.dy = 1; }
//////
//////            if (timer > delay) {
//////                player.x += player.dx;
//////                player.y += player.dy;
//////
//////                if (player.x < 0) player.x = 0; if (player.x > N - 1) player.x = N - 1;
//////                if (player.y < 0) player.y = 0; if (player.y > M - 1) player.y = M - 1;
//////
//////                player.constructing = (grid[player.y][player.x] == 0);
//////                if (grid[player.y][player.x] == 2) Game = false;
//////                if (grid[player.y][player.x] == 0) {
//////                    grid[player.y][player.x] = 2;
//////                    player.capturedTiles++;
//////                }
//////                timer = 0;
//////            }
//////
//////            for (int i = 0; i < enemyCount; i++) enemies[i].move(grid, time, ts);
//////
//////            if (grid[player.y][player.x] == 1) {
//////                player.dx = player.dy = 0;
//////
//////                for (int i = 0; i < enemyCount; i++)
//////                    drop(enemies[i].y / ts, enemies[i].x / ts, grid);
//////
//////                int newlyCaptured = 0;
//////                for (int i = 0; i < M; i++)
//////                    for (int j = 0; j < N; j++) {
//////                        if (grid[i][j] == -1) grid[i][j] = 0;
//////                        else if (grid[i][j] == 2) { grid[i][j] = 1; newlyCaptured++; }
//////                    }
//////
//////                if (newlyCaptured > 0) {
//////                    if (newlyCaptured > bonusThreshold) {
//////                        player.score += newlyCaptured * bonusMultiplier;
//////                        bonusCount++;
//////                        if (bonusCount == 3) bonusThreshold = 5;
//////                        if (bonusCount >= 5) bonusMultiplier = 4;
//////                    }
//////                    else {
//////                        player.score += newlyCaptured;
//////                    }
//////                    cout << "Single-player: Captured " << newlyCaptured << " tiles, Score: " << player.score << endl;
//////
//////                    if (player.score >= 50 && (player.score - newlyCaptured) < 50) player.powerUps++;
//////                    if (player.score >= 70 && (player.score - newlyCaptured) < 70) player.powerUps++;
//////                    if (player.score >= 100 && (player.score - newlyCaptured) < 100) player.powerUps++;
//////                    if (player.score >= 130 && (player.score - newlyCaptured) < 130) player.powerUps++;
//////                    if (player.score >= 160 && ((player.score - newlyCaptured) / 30 < player.score / 30)) player.powerUps++;
//////                    cout << "Single-player: Power-Ups: " << player.powerUps << endl;
//////                }
//////            }
//////
//////            for (int i = 0; i < enemyCount; i++)
//////                if (grid[static_cast<int>(enemies[i].y / ts)][static_cast<int>(enemies[i].x / ts)] == 2) Game = false;
//////        }
//////
//////        if (player.frozen && player.frozenClock.getElapsedTime().asSeconds() >= 3) {
//////            player.frozen = false;
//////        }
//////
//////        window.clear();
//////        inventory.drawBackground(window);
//////
//////        for (int i = 0; i < M; i++)
//////            for (int j = 0; j < N; j++) {
//////                if (grid[i][j] == 0) continue;
//////                if (grid[i][j] == 1) sTile.setTextureRect(IntRect(0, 0, ts, ts));
//////                if (grid[i][j] == 2) sTile.setTextureRect(IntRect(54, 0, ts, ts));
//////                sTile.setPosition(j * ts, i * ts);
//////                window.draw(sTile);
//////            }
//////
//////        sTile.setTextureRect(IntRect(36, 0, ts, ts));
//////        sTile.setPosition(player.x * ts, player.y * ts);
//////        window.draw(sTile);
//////
//////        sEnemy.rotate(10);
//////        for (int i = 0; i < enemyCount; i++) {
//////            if (enemies[i].frozen) sEnemy.setColor(Color::Cyan);
//////            else sEnemy.setColor(Color::White);
//////            sEnemy.setPosition(enemies[i].x, enemies[i].y);
//////            window.draw(sEnemy);
//////        }
//////
//////        scoreText.setString("Score: " + to_string(player.score));
//////        powerUpText.setString("Power-Ups: " + to_string(player.powerUps));
//////        window.draw(scoreText);
//////        window.draw(powerUpText);
//////
//////        if (player.showBackgroundMenu) {
//////            for (const auto& option : backgroundOptions) {
//////                window.draw(option);
//////            }
//////        }
//////
//////        if (!Game) window.draw(sGameover);
//////
//////        window.display();
//////    }
//////}
//////
////// Multiplayer mode
//////void playMultiplayerXonixGame(PlayerList& pl, RenderWindow& window, Font& font, const string& player1User, const string& player2User, Inventory& inventory) {
//////    window.setTitle("Xonix Multiplayer Game");
//////
//////    srand(time(0));
//////
//////    int grid[M][N] = { 0 };
//////    int tileOwner[M][N] = { 0 };
//////
//////    Texture t1, t2, t3;
//////    if (!t1.loadFromFile("images/tiles.png") || !t2.loadFromFile("images/gameover.png") || !t3.loadFromFile("images/enemy.png")) {
//////        cout << "Error: Could not load game textures.\n";
//////        return;
//////    }
//////
//////    Sprite sTile(t1), sGameover(t2), sEnemy(t3);
//////    sGameover.setPosition(200.0f, 150.0f);
//////    sEnemy.setOrigin(20, 20);
//////
//////    Text scoreTextP1("", font, 20);
//////    scoreTextP1.setFillColor(Color::White);
//////    scoreTextP1.setPosition(20.0f, 15.0f);
//////
//////    Text powerUpTextP1("", font, 20);
//////    powerUpTextP1.setFillColor(Color(100, 255, 100));
//////    powerUpTextP1.setPosition(20.0f, 40.0f);
//////
//////    Text scoreTextP2("", font, 20);
//////    scoreTextP2.setFillColor(Color::White);
//////    scoreTextP2.setPosition(static_cast<float>(N * ts - 190), 15.0f);
//////
//////    Text powerUpTextP2("", font, 20);
//////    powerUpTextP2.setFillColor(Color(100, 255, 100));
//////    powerUpTextP2.setPosition(static_cast<float>(N * ts - 190), 40.0f);
//////
//////    Text winnerText("", font, 30);
//////    winnerText.setFillColor(Color::Yellow);
//////    winnerText.setStyle(Text::Bold);
//////    winnerText.setPosition(300.0f, 120.0f);
//////    winnerText.setOutlineColor(Color::Black);
//////    winnerText.setOutlineThickness(1);
//////
//////    vector<Text> backgroundOptions;
//////    string namesString = inventory.getBackgroundNames();
//////    vector<string> bgNames;
//////    stringstream ss(namesString);
//////    string name;
//////    while (getline(ss, name)) {
//////        if (!name.empty()) {
//////            bgNames.push_back(name);
//////        }
//////    }
//////    for (size_t i = 0; i < bgNames.size(); ++i) {
//////        Text option;
//////        option.setFont(font);
//////        option.setString(bgNames[i]);
//////        option.setCharacterSize(18);
//////        option.setPosition(20.0f, 70.0f + i * 25.0f);
//////        option.setFillColor(i + 1 == inventory.getCurrentBackgroundID() ? getColorForBackground(bgNames[i]) : Color::White);
//////        backgroundOptions.push_back(option);
//////    }
//////
//////    int enemyCount = 2; // Default to 2 enemies
//////    Enemy enemies[10];
//////    for (int i = 0; i < enemyCount; ++i) {
//////        enemies[i] = Enemy(grid, ts);
//////    }
//////
//////    bool Game = true;
//////    PlayerState player1, player2;
//////    player1.x = 10;
//////    player1.y = 0;
//////    player1.pathColor = Color::Blue;
//////    player2.x = N - 10;
//////    player2.y = 0;
//////    player2.pathColor = Color::Red;
//////
//////    float timer = 0, delay = 0.07;
//////    Clock clock;
//////
//////    int bonusCountP1 = 0, bonusThresholdP1 = 10, bonusMultiplierP1 = 2;
//////    int bonusCountP2 = 0, bonusThresholdP2 = 10, bonusMultiplierP2 = 2;
//////
//////    for (int i = 0; i < M; i++)
//////        for (int j = 0; j < N; j++)
//////            if (i == 0 || j == 0 || i == M - 1 || j == N - 1) {
//////                grid[i][j] = 1;
//////                tileOwner[i][j] = 0;
//////            }
//////
//////     Start background music
//////    Music& music = inventory.getCurrentSound();
//////    music.setLoop(true);
//////    music.play();
//////
//////    while (window.isOpen()) {
//////        float time = clock.getElapsedTime().asSeconds();
//////        clock.restart();
//////        timer += time;
//////
//////        Event e;
//////        while (window.pollEvent(e)) {
//////            if (e.type == Event::Closed) {
//////                music.stop();
//////                Player* p1 = pl.getPlayerByUsername(player1User);
//////                if (p1) {
//////                    pl.savePlayerStats(player1User, player1.score, player1.powerUps, p1->preferredThemeID, p1->preferredSoundID);
//////                }
//////                else {
//////                    cerr << "Error: Player " << player1User << " not found for saving stats.\n";
//////                }
//////                Player* p2 = pl.getPlayerByUsername(player2User);
//////                if (p2) {
//////                    pl.savePlayerStats(player2User, player2.score, player2.powerUps, p2->preferredThemeID, p2->preferredSoundID);
//////                }
//////                else {
//////                    cerr << "Error: Player " << player2User << " not found for saving stats.\n";
//////                }
//////                window.close();
//////                return;
//////            }
//////
//////            if (e.type == Event::KeyPressed) {
//////                if (e.key.code == Keyboard::Escape) {
//////                    if (!Game || (!player1.alive && !player2.alive)) {
//////                        music.stop();
//////                        Player* p1 = pl.getPlayerByUsername(player1User);
//////                        if (p1) {
//////                            pl.savePlayerStats(player1User, player1.score, player1.powerUps, p1->preferredThemeID, p1->preferredSoundID);
//////                        }
//////                        else {
//////                            cerr << "Error: Player " << player1User << " not found for saving stats.\n";
//////                        }
//////                        Player* p2 = pl.getPlayerByUsername(player2User);
//////                        if (p2) {
//////                            pl.savePlayerStats(player2User, player2.score, player2.powerUps, p2->preferredThemeID, p2->preferredSoundID);
//////                        }
//////                        else {
//////                            cerr << "Error: Player " << player2User << " not found for saving stats.\n";
//////                        }
//////                        return;
//////                    }
//////                    else {
//////                        for (int i = 0; i < M; i++)
//////                            for (int j = 0; j < N; j++) {
//////                                if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
//////                                    grid[i][j] = 1;
//////                                else
//////                                    grid[i][j] = 0;
//////                                tileOwner[i][j] = 0;
//////                            }
//////                        player1 = PlayerState();
//////                        player2 = PlayerState();
//////                        player1.x = 10;
//////                        player1.y = 0;
//////                        player1.pathColor = Color::Blue;
//////                        player2.x = N - 10;
//////                        player2.y = 0;
//////                        player2.pathColor = Color::Red;
//////                        Game = true;
//////                        bonusCountP1 = bonusCountP2 = 0;
//////                        bonusThresholdP1 = bonusThresholdP2 = 10;
//////                        bonusMultiplierP1 = bonusMultiplierP2 = 2;
//////                        for (int i = 0; i < enemyCount; i++)
//////                            enemies[i] = Enemy(grid, ts);
//////                        music.stop();
//////                        music.play(); // Restart music after reset
//////                    }
//////                }
//////                if (e.key.code == Keyboard::Space && player1.powerUps > 0 && Game && player1.alive) {
//////                    for (int i = 0; i < enemyCount; i++)
//////                        enemies[i].activateFreeze();
//////                    player2.frozen = true;
//////                    player2.frozenClock.restart();
//////                    player1.powerUps--;
//////                }
//////                if (e.key.code == Keyboard::Return && player2.powerUps > 0 && Game && player2.alive) {
//////                    for (int i = 0; i < enemyCount; i++)
//////                        enemies[i].activateFreeze();
//////                    player1.frozen = true;
//////                    player1.frozenClock.restart();
//////                    player2.powerUps--;
//////                }
//////                if (e.key.code == Keyboard::B) {
//////                    player1.showBackgroundMenu = !player1.showBackgroundMenu;
//////                    player2.showBackgroundMenu = player1.showBackgroundMenu;
//////                }
//////            }
//////
//////            if (e.type == Event::MouseButtonPressed && player1.showBackgroundMenu) {
//////                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
//////                for (size_t i = 0; i < backgroundOptions.size(); ++i) {
//////                    if (backgroundOptions[i].getGlobalBounds().contains(mousePos)) {
//////                        inventory.setBackground(i + 1);
//////                        player1.showBackgroundMenu = false;
//////                        player2.showBackgroundMenu = false;
//////                        for (size_t j = 0; j < backgroundOptions.size(); ++j) {
//////                            backgroundOptions[j].setFillColor(j + 1 == inventory.getCurrentBackgroundID() ? getColorForBackground(bgNames[j]) : Color::White);
//////                        }
//////                        break;
//////                    }
//////                }
//////            }
//////        }
//////
//////        if (Game) {
//////            if (!player1.frozen && player1.alive) {
//////                if (Keyboard::isKeyPressed(Keyboard::A)) { player1.dx = -1; player1.dy = 0; }
//////                if (Keyboard::isKeyPressed(Keyboard::D)) { player1.dx = 1; player1.dy = 0; }
//////                if (Keyboard::isKeyPressed(Keyboard::W)) { player1.dx = 0; player1.dy = -1; }
//////                if (Keyboard::isKeyPressed(Keyboard::S)) { player1.dx = 0; player1.dy = 1; }
//////            }
//////
//////            if (!player2.frozen && player2.alive) {
//////                if (Keyboard::isKeyPressed(Keyboard::Left)) { player2.dx = -1; player2.dy = 0; }
//////                if (Keyboard::isKeyPressed(Keyboard::Right)) { player2.dx = 1; player2.dy = 0; }
//////                if (Keyboard::isKeyPressed(Keyboard::Up)) { player2.dx = 0; player2.dy = -1; }
//////                if (Keyboard::isKeyPressed(Keyboard::Down)) { player2.dx = 0; player2.dy = 1; }
//////            }
//////
//////            if (timer > delay) {
//////                if (player1.alive && !player1.frozen) {
//////                    player1.x += player1.dx;
//////                    player1.y += player1.dy;
//////
//////                    if (player1.x < 0) player1.x = 0; if (player1.x > N - 1) player1.x = N - 1;
//////                    if (player1.y < 0) player1.y = 0; if (player1.y > M - 1) player1.y = M - 1;
//////
//////                    player1.constructing = (grid[player1.y][player1.x] == 0);
//////                    if (grid[player1.y][player1.x] == 2 && tileOwner[player1.y][player1.x] == 1) {
//////                        player1.alive = false;
//////                    }
//////                    else if (grid[player1.y][player1.x] == 3 ||
//////                        (grid[player1.y][player1.x] == 1 && tileOwner[player1.y][player1.x] == 2)) {
//////                        player1.alive = false;
//////                    }
//////                    else if (grid[player1.y][player1.x] == 0) {
//////                        grid[player1.y][player1.x] = 2;
//////                        tileOwner[player1.y][player1.x] = 1;
//////                        player1.capturedTiles++;
//////                    }
//////                }
//////
//////                if (player2.alive && !player2.frozen) {
//////                    player2.x += player2.dx;
//////                    player2.y += player2.dy;
//////
//////                    if (player2.x < 0) player2.x = 0; if (player2.x > N - 1) player2.x = N - 1;
//////                    if (player2.y < 0) player2.y = 0; if (player2.y > M - 1) player2.y = M - 1;
//////
//////                    player2.constructing = (grid[player2.y][player2.x] == 0);
//////                    if (grid[player2.y][player2.x] == 3 && tileOwner[player2.y][player2.x] == 2) {
//////                        player2.alive = false;
//////                    }
//////                    else if (grid[player2.y][player2.x] == 2 ||
//////                        (grid[player2.y][player2.x] == 1 && tileOwner[player2.y][player2.x] == 1)) {
//////                        player2.alive = false;
//////                    }
//////                    else if (grid[static_cast<int>(player2.y)][static_cast<int>(player2.x)] == 0) {
//////                        grid[static_cast<int>(player2.y)][static_cast<int>(player2.x)] = 3;
//////                        tileOwner[static_cast<int>(player2.y)][static_cast<int>(player2.x)] = 2;
//////
//////                        player2.capturedTiles++;
//////                    }
//////                }
//////
//////                if (player1.alive && player2.alive && player1.x == player2.x && player1.y == player2.y) {
//////                    if (player1.constructing && player2.constructing) {
//////                        player1.alive = false;
//////                        player2.alive = false;
//////                    }
//////                    else if (player1.constructing) {
//////                        player1.alive = false;
//////                    }
//////                    else if (player2.constructing) {
//////                        player2.alive = false;
//////                    }
//////                    else {
//////                        player1.alive = false;
//////                        player2.alive = false;
//////                    }
//////                }
//////
//////                timer = 0;
//////            }
//////
//////            for (int i = 0; i < enemyCount; i++) {
//////                enemies[i].move(grid, time, ts);
//////                if (enemies[i].y / ts < M && enemies[i].x / ts < N) {
//////                    int enemyGridVal = grid[static_cast<int>(enemies[i].y / ts)][static_cast<int>(enemies[i].x / ts)];
//////                    if (enemyGridVal == 2 && player1.alive && player1.constructing) player1.alive = false;
//////                    if (enemyGridVal == 3 && player2.alive && player2.constructing) player2.alive = false;
//////                }
//////            }
//////
//////            if (player1.alive && grid[player1.y][player1.x] == 1) {
//////                player1.dx = player1.dy = 0;
//////                for (int i = 0; i < enemyCount; i++)
//////                    drop(enemies[i].y / ts, enemies[i].x / ts, grid);
//////
//////                int newlyCaptured = 0;
//////                for (int i = 0; i < M; i++)
//////                    for (int j = 0; j < N; j++) {
//////                        if (grid[i][j] == -1) {
//////                            grid[i][j] = 0;
//////                            tileOwner[i][j] = 0;
//////                        }
//////                        else if (grid[i][j] == 2 && tileOwner[i][j] == 1) {
//////                            grid[i][j] = 1;
//////                            tileOwner[i][j] = 1;
//////                            newlyCaptured++;
//////                        }
//////                    }
//////
//////                if (newlyCaptured > 0) {
//////                    if (newlyCaptured > bonusThresholdP1) {
//////                        player1.score += newlyCaptured * bonusMultiplierP1;
//////                        bonusCountP1++;
//////                        if (bonusCountP1 == 3) bonusThresholdP1 = 5;
//////                        if (bonusCountP1 >= 5) bonusMultiplierP1 = 4;
//////                    }
//////                    else {
//////                        player1.score += newlyCaptured;
//////                    }
//////                    cout << "Multiplayer P1: Captured " << newlyCaptured << " tiles, Score: " << player1.score << endl;
//////
//////                    if (player1.score >= 50 && (player1.score - newlyCaptured) < 50) player1.powerUps++;
//////                    if (player1.score >= 70 && (player1.score - newlyCaptured) < 70) player1.powerUps++;
//////                    if (player1.score >= 100 && (player1.score - newlyCaptured) < 100) player1.powerUps++;
//////                    if (player1.score >= 130 && (player1.score - newlyCaptured) < 130) player1.powerUps++;
//////                    if (player1.score >= 160 && ((player1.score - newlyCaptured) / 30 < player1.score / 30)) player1.powerUps++;
//////                    cout << "Multiplayer P1: Power-Ups: " << player1.powerUps << endl;
//////                }
//////            }
//////
//////            if (player2.alive && grid[player2.y][player2.x] == 1) {
//////                player2.dx = player2.dy = 0;
//////                for (int i = 0; i < enemyCount; i++)
//////                    drop(enemies[i].y / ts, enemies[i].x / ts, grid);
//////
//////                int newlyCaptured = 0;
//////                for (int i = 0; i < M; i++)
//////                    for (int j = 0; j < N; j++) {
//////                        if (grid[i][j] == -1) {
//////                            grid[i][j] = 0;
//////                            tileOwner[i][j] = 0;
//////                        }
//////                        else if (grid[i][j] == 3 && tileOwner[i][j] == 2) {
//////                            grid[i][j] = 1;
//////                            tileOwner[i][j] = 2;
//////                            newlyCaptured++;
//////                        }
//////                    }
//////
//////                if (newlyCaptured > 0) {
//////                    if (newlyCaptured > bonusThresholdP2) {
//////                        player2.score += newlyCaptured * bonusMultiplierP2;
//////                        bonusCountP2++;
//////                        if (bonusCountP2 == 3) bonusThresholdP2 = 5;
//////                        if (bonusCountP2 >= 5) bonusMultiplierP2 = 4;
//////                    }
//////                    else {
//////                        player2.score += newlyCaptured;
//////                    }
//////                    cout << "Multiplayer P2: Captured " << newlyCaptured << " tiles, Score: " << player2.score << endl;
//////
//////                    if (player2.score >= 50 && (player2.score - newlyCaptured) < 50) player2.powerUps++;
//////                    if (player2.score >= 70 && (player2.score - newlyCaptured) < 70) player2.powerUps++;
//////                    if (player2.score >= 100 && (player2.score - newlyCaptured) < 100) player2.powerUps++;
//////                    if (player2.score >= 130 && (player2.score - newlyCaptured) < 130) player2.powerUps++;
//////                    if (player2.score >= 160 && ((player2.score - newlyCaptured) / 30 < player2.score / 30)) player2.powerUps++;
//////                    cout << "Multiplayer P2: Power-Ups: " << player2.powerUps << endl;
//////                }
//////            }
//////        }
//////
//////        if (player1.frozen && player1.frozenClock.getElapsedTime().asSeconds() >= 3) {
//////            player1.frozen = false;
//////        }
//////        if (player2.frozen && player2.frozenClock.getElapsedTime().asSeconds() >= 3) {
//////            player2.frozen = false;
//////        }
//////
//////        if (!player1.alive && !player2.alive) {
//////            Game = false;
//////            string winner;
//////            if (player1.score > player2.score)
//////                winner = "Player 1 Wins!";
//////            else if (player2.score > player1.score)
//////                winner = "Player 2 Wins!";
//////            else
//////                winner = "It's a Tie!";
//////            winnerText.setString(winner);
//////        }
//////
//////        window.clear();
//////        inventory.drawBackground(window);
//////
//////        for (int i = 0; i < M; i++)
//////            for (int j = 0; j < N; j++) {
//////                if (grid[i][j] == 0) continue;
//////                sTile.setColor(Color::White);
//////                if (grid[i][j] == 1) {
//////                    sTile.setTextureRect(IntRect(0, 0, ts, ts));
//////                    if (tileOwner[i][j] == 1)
//////                        sTile.setColor(Color(135, 206, 250, 200));
//////                    else if (tileOwner[i][j] == 2)
//////                        sTile.setColor(Color(255, 99, 71, 200));
//////                }
//////                if (grid[i][j] == 2) {
//////                    sTile.setTextureRect(IntRect(54, 0, ts, ts));
//////                    sTile.setColor(player1.pathColor);
//////                }
//////                if (grid[i][j] == 3) {
//////                    sTile.setTextureRect(IntRect(54, 0, ts, ts));
//////                    sTile.setColor(player2.pathColor);
//////                }
//////                sTile.setPosition(j * ts, i * ts);
//////                window.draw(sTile);
//////            }
//////
//////        if (player1.alive) {
//////            sTile.setColor(Color::White);
//////            sTile.setTextureRect(IntRect(36, 0, ts, ts));
//////            sTile.setPosition(player1.x * ts, player1.y * ts);
//////            window.draw(sTile);
//////        }
//////
//////        if (player2.alive) {
//////            sTile.setColor(Color::White);
//////            sTile.setTextureRect(IntRect(72, 0, ts, ts));
//////            sTile.setPosition(player2.x * ts, player2.y * ts);
//////            window.draw(sTile);
//////        }
//////
//////        sEnemy.rotate(10);
//////        for (int i = 0; i < enemyCount; i++) {
//////            if (enemies[i].frozen) sEnemy.setColor(Color::Cyan);
//////            else sEnemy.setColor(Color::White);
//////            sEnemy.setPosition(enemies[i].x, enemies[i].y);
//////            window.draw(sEnemy);
//////        }
//////
//////        scoreTextP1.setString("P1 Score: " + to_string(player1.score));
//////        powerUpTextP1.setString("P1 Power-Ups: " + to_string(player1.powerUps));
//////        scoreTextP2.setString("P2 Score: " + to_string(player2.score));
//////        powerUpTextP2.setString("P2 Power-Ups: " + to_string(player2.powerUps));
//////
//////        window.draw(scoreTextP1);
//////        window.draw(powerUpTextP1);
//////        window.draw(scoreTextP2);
//////        window.draw(powerUpTextP2);
//////
//////        if (player1.showBackgroundMenu) {
//////            for (const auto& option : backgroundOptions) {
//////                window.draw(option);
//////            }
//////        }
//////
//////        if (!Game) {
//////            window.draw(sGameover);
//////            window.draw(winnerText);
//////        }
//////
//////        window.display();
//////    }
//////}
////
////#include "game.h"
////#include <SFML/Audio.hpp>
////#include <time.h>
////#include <string>
////#include <sstream>
////#include <iostream>
////#include <algorithm>
////#include <cctype>
////
////using namespace std;
////using namespace sf;
////
////// Helper function to map background names to colors
////Color getColorForBackground(const string& name) {
////    string lowerName = name;
////    transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
////    if (lowerName == "blue") return Color(135, 206, 250); // Light blue for readability
////    if (lowerName == "white") return Color::White;
////    if (lowerName == "red") return Color::Red;
////    return Color::White; // Default for unknown names
////}
////
////// Helper function to create a styled button
////RectangleShape createButton(float x, float y, float width, float height, Color fillColor) {
////    RectangleShape button(Vector2f(width, height));
////    button.setPosition(x, y);
////    button.setFillColor(fillColor);
////    button.setOutlineColor(Color(50, 50, 50));
////    button.setOutlineThickness(2);
////    return button;
////}
////
////// Enemy implementation
////Enemy::Enemy(int grid[M][N], int tileSize) {
////    // Random position within playable area (not on borders)
////    int maxX = N - 2, maxY = M - 2;
////    do {
////        x = (rand() % maxX + 1) * tileSize + tileSize / 2.0f;
////        y = (rand() % maxY + 1) * tileSize + tileSize / 2.0f;
////    } while (grid[static_cast<int>(y / tileSize)][static_cast<int>(x / tileSize)] != 0);
////
////    // Random initial direction with normalized speed
////    float angle = static_cast<float>(rand()) / RAND_MAX * 2 * 3.14159f;
////    dx = cos(angle);
////    dy = sin(angle);
////    frozen = false;
////    speed = 90.0f; // Default speed (pixels/second), adjustable by level
////}
////
////void Enemy::move(int grid[M][N], float deltaTime, int tileSize) {
////    if (frozen && frozenClock.getElapsedTime().asSeconds() < 3) return;
////    frozen = false;
////
////    // Move smoothly based on speed and deltaTime
////    float moveX = dx * speed * deltaTime;
////    float moveY = dy * speed * deltaTime;
////    float newX = x + moveX;
////    float newY = y + moveY;
////
////    // Check boundaries and grid collision
////    int gridX = static_cast<int>(newX / tileSize);
////    int gridY = static_cast<int>(newY / tileSize);
////    bool collide = false;
////
////    if (gridX < 0 || gridX >= N || gridY < 0 || gridY >= M) {
////        collide = true;
////    }
////    else if (grid[gridY][gridX] == 1) {
////        collide = true;
////    }
////
////    if (collide) {
////        // Bounce with slight randomization
////        float angle = atan2(dy, dx) + (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.5f;
////        dx = cos(angle);
////        dy = sin(angle);
////        // Ensure non-zero movement
////        if (abs(dx) < 0.1f && abs(dy) < 0.1f) {
////            dx = rand() % 2 ? 1.0f : -1.0f;
////            dy = rand() % 2 ? 1.0f : -1.0f;
////            float len = sqrt(dx * dx + dy * dy);
////            dx /= len;
////            dy /= len;
////        }
////    }
////    else {
////        x = newX;
////        y = newY;
////    }
////
////    // Keep within bounds
////    if (x < tileSize / 2.0f) x = tileSize / 2.0f;
////    if (x > (N - 0.5f) * tileSize) x = (N - 0.5f) * tileSize;
////    if (y < tileSize / 2.0f) y = tileSize / 2.0f;
////    if (y > (M - 0.5f) * tileSize) y = (M - 0.5f) * tileSize;
////}
////
////void Enemy::activateFreeze() {
////    frozen = true;
////    frozenClock.restart();
////}
////
////// Drop implementation (flood-fill to mark unreachable areas)
////void drop(int y, int x, int grid[M][N]) {
////    if (y < 0 || y >= M || x < 0 || x >= N) return;
////    if (grid[y][x] == 0) grid[y][x] = -1;
////    if (y > 0 && grid[y - 1][x] == 0) drop(y - 1, x, grid);
////    if (y < M - 1 && grid[y + 1][x] == 0) drop(y + 1, x, grid);
////    if (x > 0 && grid[y][x - 1] == 0) drop(y, x - 1, grid);
////    if (x < N - 1 && grid[y][x + 1] == 0) drop(y, x + 1, grid);
////}
////
////// Level selection struct
////struct Level {
////    string name;
////    int enemyCount;
////    float enemySpeed;
////};
////
////// Single-player level selection
////int selectLevel(RenderWindow& window, Font& font, Inventory& inventory) {
////    window.setTitle("Select Level");
////
////    // Level definitions (scaled for ts=18)
////    vector<Level> levels = {
////        {"Easy", 2, 45.0f},
////        {"Medium", 4, 90.0f},
////        {"Hard", 6, 135.0f}
////    };
////
////    // UI elements
////    Text title("Select Level", font, 36);
////    title.setFillColor(Color::White);
////    title.setStyle(Text::Bold);
////    title.setPosition(330, 40);
////    title.setOutlineColor(Color::Black);
////    title.setOutlineThickness(1);
////
////    vector<RectangleShape> levelButtonBgs;
////    vector<Text> levelButtons;
////    for (size_t i = 0; i < levels.size(); ++i) {
////        RectangleShape bg = createButton(320, 150 + i * 60, 160, 40, Color(50, 150, 50));
////        Text button(levels[i].name, font, 22);
////        button.setFillColor(Color::White);
////        button.setPosition(350, 155 + i * 60);
////        levelButtonBgs.push_back(bg);
////        levelButtons.push_back(button);
////    }
////
////    while (window.isOpen()) {
////        Event event;
////        while (window.pollEvent(event)) {
////            if (event.type == Event::Closed) {
////                window.close();
////                return -1; // Indicate closure
////            }
////
////            if (event.type == Event::MouseButtonPressed) {
////                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
////                for (size_t i = 0; i < levelButtons.size(); ++i) {
////                    if (levelButtons[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
////                        return static_cast<int>(i); // Return level index
////                    }
////                }
////            }
////        }
////
////        // Hover effects
////        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
////        for (size_t i = 0; i < levelButtonBgs.size(); ++i) {
////            levelButtonBgs[i].setFillColor(levelButtons[i].getGlobalBounds().contains(mousePos.x, mousePos.y) ? Color(70, 180, 70) : Color(50, 150, 50));
////        }
////
////        window.clear();
////        inventory.drawBackground(window);
////        window.draw(title);
////        for (size_t i = 0; i < levelButtons.size(); ++i) {
////            window.draw(levelButtonBgs[i]);
////            window.draw(levelButtons[i]);
////        }
////        window.display();
////    }
////    return -1; // Fallback
////}
////
////// Single-player game
////void playXonixGame(PlayerList& pl, RenderWindow& window, Font& font, const string& currentUser, Inventory& inventory) {
////    window.setTitle("Xonix Game");
////    const int windowWidth = 800;
////    const int windowHeight = 600;
////    const int ts = 18; // Tile size from game.h
////    const float offsetX = (windowWidth - N * ts) / 2.0f; // 40
////    const float offsetY = (windowHeight - M * ts) / 2.0f; // 120
////    window.setSize(Vector2u(windowWidth, windowHeight));
////    srand(time(0));
////
////    // Level selection
////    int levelIndex = selectLevel(window, font, inventory);
////    if (levelIndex == -1) return; // Window closed
////
////    vector<Level> levels = {
////        {"Easy", 2, 45.0f},
////        {"Medium", 4, 90.0f},
////        {"Hard", 6, 135.0f}
////    };
////    int enemyCount = levels[levelIndex].enemyCount;
////    float enemySpeed = levels[levelIndex].enemySpeed;
////
////    int grid[M][N] = { 0 };
////
////    Texture t1, t2, t3;
////    if (!t1.loadFromFile("images/tiles.png") || !t2.loadFromFile("images/gameover.png") || !t3.loadFromFile("images/enemy.png")) {
////        cout << "Error: Could not load game textures.\n";
////        return;
////    }
////
////    Sprite sTile(t1), sGameover(t2), sEnemy(t3);
////    sGameover.setPosition(200.0f + offsetX, 150.0f + offsetY);
////    sEnemy.setOrigin(20, 20);
////
////    Text scoreText("", font, 20);
////    scoreText.setFillColor(Color::White);
////    scoreText.setPosition(20.0f + offsetX, 15.0f + offsetY);
////
////    Text powerUpText("", font, 20);
////    powerUpText.setFillColor(Color::Green);
////    powerUpText.setPosition(20.0f + offsetX, 40.0f + offsetY);
////
////    // Background selection UI
////    vector<Text> backgroundOptions;
////    string namesString = inventory.getBackgroundNames();
////    vector<string> bgNames;
////    stringstream ss(namesString);
////    string name;
////    while (getline(ss, name)) {
////        if (!name.empty()) {
////            bgNames.push_back(name);
////        }
////    }
////    for (size_t i = 0; i < bgNames.size(); ++i) {
////        Text option;
////        option.setFont(font);
////        option.setString(bgNames[i]);
////        option.setCharacterSize(18);
////        option.setPosition(20.0f + offsetX, 70.0f + offsetY + i * 25.0f);
////        option.setFillColor(i + 1 == inventory.getCurrentBackgroundID() ? getColorForBackground(bgNames[i]) : Color::White);
////        backgroundOptions.push_back(option);
////    }
////
////    Enemy enemies[10];
////    for (int i = 0; i < enemyCount; ++i) {
////        enemies[i] = Enemy(grid, ts);
////        enemies[i].speed = enemySpeed;
////    }
////
////    bool Game = true;
////    PlayerState player;
////    player.x = 10;
////    player.y = 0; // Start on border
////    float timer = 0, delay = 0.07;
////    Clock clock;
////
////    int bonusCount = 0, bonusThreshold = 10, bonusMultiplier = 2;
////
////    for (int i = 0; i < M; i++)
////        for (int j = 0; j < N; j++)
////            if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
////                grid[i][j] = 1;
////
////    // Start background music
////    Music& music = inventory.getCurrentSound();
////    music.setLoop(true);
////    music.play();
////
////    while (window.isOpen()) {
////        float time = clock.getElapsedTime().asSeconds();
////        clock.restart();
////        timer += time;
////
////        Event e;
////        while (window.pollEvent(e)) {
////            if (e.type == Event::Closed) {
////                music.stop();
////                Player* p = pl.getPlayerByUsername(currentUser);
////                if (p) {
////                    pl.savePlayerStats(currentUser, player.score, player.powerUps, p->preferredThemeID, p->preferredSoundID);
////                }
////                else {
////                    cerr << "Error: Player " << currentUser << " not found for saving stats.\n";
////                }
////                window.close();
////                return;
////            }
////
////            if (e.type == Event::KeyPressed) {
////                if (e.key.code == Keyboard::Escape) {
////                    if (!Game) {
////                        music.stop();
////                        Player* p = pl.getPlayerByUsername(currentUser);
////                        if (p) {
////                            pl.savePlayerStats(currentUser, player.score, player.powerUps, p->preferredThemeID, p->preferredSoundID);
////                        }
////                        else {
////                            cerr << "Error: Player " << currentUser << " not found for saving stats.\n";
////                        }
////                        return;
////                    }
////                    else {
////                        for (int i = 1; i < M - 1; i++)
////                            for (int j = 1; j < N - 1; j++)
////                                grid[i][j] = 0;
////                        player = PlayerState();
////                        player.x = 10;
////                        player.y = 0; // Reset to border
////                        Game = true;
////                        bonusCount = 0;
////                        bonusThreshold = 10;
////                        bonusMultiplier = 2;
////                        for (int i = 0; i < enemyCount; i++) {
////                            enemies[i] = Enemy(grid, ts);
////                            enemies[i].speed = enemySpeed;
////                        }
////                        music.stop();
////                        music.play(); // Restart music after reset
////                    }
////                }
////                if (e.key.code == Keyboard::Space && player.powerUps > 0 && Game) {
////                    for (int i = 0; i < enemyCount; i++)
////                        enemies[i].activateFreeze();
////                    player.powerUps--;
////                }
////                if (e.key.code == Keyboard::B) {
////                    player.showBackgroundMenu = !player.showBackgroundMenu;
////                }
////            }
////
////            if (e.type == Event::MouseButtonPressed && player.showBackgroundMenu) {
////                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
////                for (size_t i = 0; i < backgroundOptions.size(); ++i) {
////                    if (backgroundOptions[i].getGlobalBounds().contains(mousePos)) {
////                        inventory.setBackground(i + 1);
////                        player.showBackgroundMenu = false;
////                        for (size_t j = 0; j < backgroundOptions.size(); ++j) {
////                            backgroundOptions[j].setFillColor(j + 1 == inventory.getCurrentBackgroundID() ? getColorForBackground(bgNames[j]) : Color::White);
////                        }
////                        break;
////                    }
////                }
////            }
////        }
////
////        if (Game && !player.frozen) {
////            if (Keyboard::isKeyPressed(Keyboard::Left)) { player.dx = -1; player.dy = 0; }
////            if (Keyboard::isKeyPressed(Keyboard::Right)) { player.dx = 1; player.dy = 0; }
////            if (Keyboard::isKeyPressed(Keyboard::Up)) { player.dx = 0; player.dy = -1; }
////            if (Keyboard::isKeyPressed(Keyboard::Down)) { player.dx = 0; player.dy = 1; }
////
////            if (timer > delay) {
////                player.x += player.dx;
////                player.y += player.dy;
////
////                if (player.x < 0) player.x = 0; if (player.x > N - 1) player.x = N - 1;
////                if (player.y < 0) player.y = 0; if (player.y > M - 1) player.y = M - 1;
////
////                player.constructing = (grid[static_cast<int>(player.y)][static_cast<int>(player.x)] == 0);
////                if (grid[static_cast<int>(player.y)][static_cast<int>(player.x)] == 2) Game = false;
////                if (grid[static_cast<int>(player.y)][static_cast<int>(player.x)] == 0) {
////                    grid[static_cast<int>(player.y)][static_cast<int>(player.x)] = 2;
////                    player.capturedTiles++;
////                }
////                timer = 0;
////            }
////
////            for (int i = 0; i < enemyCount; i++) enemies[i].move(grid, time, ts);
////
////            if (grid[static_cast<int>(player.y)][static_cast<int>(player.x)] == 1) {
////                player.dx = player.dy = 0;
////
////                for (int i = 0; i < enemyCount; i++) {
////                    int enemyY = static_cast<int>(enemies[i].y / ts);
////                    int enemyX = static_cast<int>(enemies[i].x / ts);
////                    if (enemyY >= 0 && enemyY < M && enemyX >= 0 && enemyX < N)
////                        drop(enemyY, enemyX, grid);
////                }
////
////                int newlyCaptured = 0;
////                for (int i = 0; i < M; i++)
////                    for (int j = 0; j < N; j++) {
////                        if (grid[i][j] == -1) grid[i][j] = 0;
////                        else if (grid[i][j] == 2) { grid[i][j] = 1; newlyCaptured++; }
////                    }
////
////                if (newlyCaptured > 0) {
////                    if (newlyCaptured > bonusThreshold) {
////                        player.score += newlyCaptured * bonusMultiplier;
////                        bonusCount++;
////                        if (bonusCount == 3) bonusThreshold = 5;
////                        if (bonusCount >= 5) bonusMultiplier = 4;
////                    }
////                    else {
////                        player.score += newlyCaptured;
////                    }
////                    cout << "Single-player: Captured " << newlyCaptured << " tiles, Score: " << player.score << endl;
////
////                    if (player.score >= 50 && (player.score - newlyCaptured) < 50) player.powerUps++;
////                    if (player.score >= 70 && (player.score - newlyCaptured) < 70) player.powerUps++;
////                    if (player.score >= 100 && (player.score - newlyCaptured) < 100) player.powerUps++;
////                    if (player.score >= 130 && (player.score - newlyCaptured) < 130) player.powerUps++;
////                    if (player.score >= 160 && ((player.score - newlyCaptured) / 30 < player.score / 30)) player.powerUps++;
////                    cout << "Single-player: Power-Ups: " << player.powerUps << endl;
////                }
////            }
////
////            for (int i = 0; i < enemyCount; i++) {
////                int enemyY = static_cast<int>(enemies[i].y / ts);
////                int enemyX = static_cast<int>(enemies[i].x / ts);
////                if (enemyY >= 0 && enemyY < M && enemyX >= 0 && enemyX < N) {
////                    if (grid[enemyY][enemyX] == 2) Game = false;
////                }
////            }
////        }
////
////        if (player.frozen && player.frozenClock.getElapsedTime().asSeconds() >= 3) {
////            player.frozen = false;
////        }
////
////        window.clear();
////        inventory.drawBackground(window);
////
////        for (int i = 0; i < M; i++)
////            for (int j = 0; j < N; j++) {
////                if (grid[i][j] == 0) continue;
////                if (grid[i][j] == 1) sTile.setTextureRect(IntRect(0, 0, ts, ts));
////                if (grid[i][j] == 2) sTile.setTextureRect(IntRect(54, 0, ts, ts));
////                sTile.setPosition(j * ts + offsetX, i * ts + offsetY);
////                window.draw(sTile);
////            }
////
////        sTile.setTextureRect(IntRect(36, 0, ts, ts));
////        sTile.setPosition(static_cast<int>(player.x) * ts + offsetX, static_cast<int>(player.y) * ts + offsetY);
////        window.draw(sTile);
////
////        sEnemy.rotate(10);
////        for (int i = 0; i < enemyCount; i++) {
////            if (enemies[i].frozen) sEnemy.setColor(Color::Cyan);
////            else sEnemy.setColor(Color::White);
////            sEnemy.setPosition(enemies[i].x + offsetX, enemies[i].y + offsetY);
////            window.draw(sEnemy);
////        }
////
////        scoreText.setString("Score: " + to_string(player.score));
////        powerUpText.setString("Power-Ups: " + to_string(player.powerUps));
////        window.draw(scoreText);
////        window.draw(powerUpText);
////
////        if (player.showBackgroundMenu) {
////            for (const auto& option : backgroundOptions) {
////                window.draw(option);
////            }
////        }
////
////        if (!Game) window.draw(sGameover);
////
////        window.display();
////    }
////}
////
////// Multiplayer mode
////void playMultiplayerXonixGame(PlayerList& pl, RenderWindow& window, Font& font, const string& player1User, const string& player2User, Inventory& inventory) {
////    window.setTitle("Xonix Multiplayer Game");
////
////    srand(time(0));
////
////    int grid[M][N] = { 0 };
////    int tileOwner[M][N] = { 0 };
////
////    const int ts = 18; // Tile size from game.h
////    const float offsetX = (800 - N * ts) / 2.0f; // 40
////    const float offsetY = (600 - M * ts) / 2.0f; // 120
////
////    Texture t1, t2, t3;
////    if (!t1.loadFromFile("images/tiles.png") || !t2.loadFromFile("images/gameover.png") || !t3.loadFromFile("images/enemy.png")) {
////        cout << "Error: Could not load game textures.\n";
////        return;
////    }
////
////    Sprite sTile(t1), sGameover(t2), sEnemy(t3);
////    sGameover.setPosition(200.0f + offsetX, 150.0f + offsetY);
////    sEnemy.setOrigin(20, 20);
////
////    Text scoreTextP1("", font, 20);
////    scoreTextP1.setFillColor(Color::White);
////    scoreTextP1.setPosition(20.0f + offsetX, 15.0f + offsetY);
////
////    Text powerUpTextP1("", font, 20);
////    powerUpTextP1.setFillColor(Color(100, 255, 100));
////    powerUpTextP1.setPosition(20.0f + offsetX, 40.0f + offsetY);
////
////    Text scoreTextP2("", font, 20);
////    scoreTextP2.setFillColor(Color::White);
////    scoreTextP2.setPosition(static_cast<float>(N * ts - 190) + offsetX, 15.0f + offsetY);
////
////    Text powerUpTextP2("", font, 20);
////    powerUpTextP2.setFillColor(Color(100, 255, 100));
////    powerUpTextP2.setPosition(static_cast<float>(N * ts - 190) + offsetX, 40.0f + offsetY);
////
////    Text winnerText("", font, 30);
////    winnerText.setFillColor(Color::Yellow);
////    winnerText.setStyle(Text::Bold);
////    winnerText.setPosition(300.0f + offsetX, 120.0f + offsetY);
////    winnerText.setOutlineColor(Color::Black);
////    winnerText.setOutlineThickness(1);
////
////    vector<Text> backgroundOptions;
////    string namesString = inventory.getBackgroundNames();
////    vector<string> bgNames;
////    stringstream ss(namesString);
////    string name;
////    while (getline(ss, name)) {
////        if (!name.empty()) {
////            bgNames.push_back(name);
////        }
////    }
////    for (size_t i = 0; i < bgNames.size(); ++i) {
////        Text option;
////        option.setFont(font);
////        option.setString(bgNames[i]);
////        option.setCharacterSize(18);
////        option.setPosition(20.0f + offsetX, 70.0f + offsetY + i * 25.0f);
////        option.setFillColor(i + 1 == inventory.getCurrentBackgroundID() ? getColorForBackground(bgNames[i]) : Color::White);
////        backgroundOptions.push_back(option);
////    }
////
////    int enemyCount = 2; // Default to 2 enemies
////    Enemy enemies[10];
////    for (int i = 0; i < enemyCount; ++i) {
////        enemies[i] = Enemy(grid, ts);
////    }
////
////    bool Game = true;
////    PlayerState player1, player2;
////    player1.x = 10;
////    player1.y = 0;
////    player1.pathColor = Color::Blue;
////    player2.x = N - 10;
////    player2.y = 0;
////    player2.pathColor = Color::Red;
////
////    float timer = 0, delay = 0.07;
////    Clock clock;
////
////    int bonusCountP1 = 0, bonusThresholdP1 = 10, bonusMultiplierP1 = 2;
////    int bonusCountP2 = 0, bonusThresholdP2 = 10, bonusMultiplierP2 = 2;
////
////    for (int i = 0; i < M; i++)
////        for (int j = 0; j < N; j++)
////            if (i == 0 || j == 0 || i == M - 1 || j == N - 1) {
////                grid[i][j] = 1;
////                tileOwner[i][j] = 0;
////            }
////
////    // Start background music
////    Music& music = inventory.getCurrentSound();
////    music.setLoop(true);
////    music.play();
////
////    while (window.isOpen()) {
////        float time = clock.getElapsedTime().asSeconds();
////        clock.restart();
////        timer += time;
////
////        Event e;
////        while (window.pollEvent(e)) {
////            if (e.type == Event::Closed) {
////                music.stop();
////                Player* p1 = pl.getPlayerByUsername(player1User);
////                if (p1) {
////                    pl.savePlayerStats(player1User, player1.score, player1.powerUps, p1->preferredThemeID, p1->preferredSoundID);
////                }
////                else {
////                    cerr << "Error: Player " << player1User << " not found for saving stats.\n";
////                }
////                Player* p2 = pl.getPlayerByUsername(player2User);
////                if (p2) {
////                    pl.savePlayerStats(player2User, player2.score, player2.powerUps, p2->preferredThemeID, p2->preferredSoundID);
////                }
////                else {
////                    cerr << "Error: Player " << player2User << " not found for saving stats.\n";
////                }
////                window.close();
////                return;
////            }
////
////            if (e.type == Event::KeyPressed) {
////                if (e.key.code == Keyboard::Escape) {
////                    if (!Game || (!player1.alive && !player2.alive)) {
////                        music.stop();
////                        Player* p1 = pl.getPlayerByUsername(player1User);
////                        if (p1) {
////                            pl.savePlayerStats(player1User, player1.score, player1.powerUps, p1->preferredThemeID, p1->preferredSoundID);
////                        }
////                        else {
////                            cerr << "Error: Player " << player1User << " not found for saving stats.\n";
////                        }
////                        Player* p2 = pl.getPlayerByUsername(player2User);
////                        if (p2) {
////                            pl.savePlayerStats(player2User, player2.score, player2.powerUps, p2->preferredThemeID, p2->preferredSoundID);
////                        }
////                        else {
////                            cerr << "Error: Player " << player2User << " not found for saving stats.\n";
////                        }
////                        return;
////                    }
////                    else {
////                        for (int i = 0; i < M; i++)
////                            for (int j = 0; j < N; j++) {
////                                if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
////                                    grid[i][j] = 1;
////                                else
////                                    grid[i][j] = 0;
////                                tileOwner[i][j] = 0;
////                            }
////                        player1 = PlayerState();
////                        player2 = PlayerState();
////                        player1.x = 10;
////                        player1.y = 0;
////                        player1.pathColor = Color::Blue;
////                        player2.x = N - 10;
////                        player2.y = 0;
////                        player2.pathColor = Color::Red;
////                        Game = true;
////                        bonusCountP1 = bonusCountP2 = 0;
////                        bonusThresholdP1 = bonusThresholdP2 = 10;
////                        bonusMultiplierP1 = bonusMultiplierP2 = 2;
////                        for (int i = 0; i < enemyCount; i++)
////                            enemies[i] = Enemy(grid, ts);
////                        music.stop();
////                        music.play(); // Restart music after reset
////                    }
////                }
////                if (e.key.code == Keyboard::Space && player1.powerUps > 0 && Game && player1.alive) {
////                    for (int i = 0; i < enemyCount; i++)
////                        enemies[i].activateFreeze();
////                    player2.frozen = true;
////                    player2.frozenClock.restart();
////                    player1.powerUps--;
////                }
////                if (e.key.code == Keyboard::Return && player2.powerUps > 0 && Game && player2.alive) {
////                    for (int i = 0; i < enemyCount; i++)
////                        enemies[i].activateFreeze();
////                    player1.frozen = true;
////                    player1.frozenClock.restart();
////                    player2.powerUps--;
////                }
////                if (e.key.code == Keyboard::B) {
////                    player1.showBackgroundMenu = !player1.showBackgroundMenu;
////                    player2.showBackgroundMenu = player1.showBackgroundMenu;
////                }
////            }
////
////            if (e.type == Event::MouseButtonPressed && player1.showBackgroundMenu) {
////                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
////                for (size_t i = 0; i < backgroundOptions.size(); ++i) {
////                    if (backgroundOptions[i].getGlobalBounds().contains(mousePos)) {
////                        inventory.setBackground(i + 1);
////                        player1.showBackgroundMenu = false;
////                        player2.showBackgroundMenu = false;
////                        for (size_t j = 0; j < backgroundOptions.size(); ++j) {
////                            backgroundOptions[j].setFillColor(j + 1 == inventory.getCurrentBackgroundID() ? getColorForBackground(bgNames[j]) : Color::White);
////                        }
////                        break;
////                    }
////                }
////            }
////        }
////
////        if (Game) {
////            if (!player1.frozen && player1.alive) {
////                if (Keyboard::isKeyPressed(Keyboard::A)) { player1.dx = -1; player1.dy = 0; }
////                if (Keyboard::isKeyPressed(Keyboard::D)) { player1.dx = 1; player1.dy = 0; }
////                if (Keyboard::isKeyPressed(Keyboard::W)) { player1.dx = 0; player1.dy = -1; }
////                if (Keyboard::isKeyPressed(Keyboard::S)) { player1.dx = 0; player1.dy = 1; }
////            }
////
////            if (!player2.frozen && player2.alive) {
////                if (Keyboard::isKeyPressed(Keyboard::Left)) { player2.dx = -1; player2.dy = 0; }
////                if (Keyboard::isKeyPressed(Keyboard::Right)) { player2.dx = 1; player2.dy = 0; }
////                if (Keyboard::isKeyPressed(Keyboard::Up)) { player2.dx = 0; player2.dy = -1; }
////                if (Keyboard::isKeyPressed(Keyboard::Down)) { player2.dx = 0; player2.dy = 1; }
////            }
////
////            if (timer > delay) {
////                if (player1.alive && !player1.frozen) {
////                    player1.x += player1.dx;
////                    player1.y += player1.dy;
////
////                    if (player1.x < 0) player1.x = 0; if (player1.x > N - 1) player1.x = N - 1;
////                    if (player1.y < 0) player1.y = 0; if (player1.y > M - 1) player1.y = M - 1;
////
////                    player1.constructing = (grid[static_cast<int>(player1.y)][static_cast<int>(player1.x)] == 0);
////                    if (grid[static_cast<int>(player1.y)][static_cast<int>(player1.x)] == 2 && tileOwner[static_cast<int>(player1.y)][static_cast<int>(player1.x)] == 1) {
////                        player1.alive = false;
////                    }
////                    else if (grid[static_cast<int>(player1.y)][static_cast<int>(player1.x)] == 3 ||
////                        (grid[static_cast<int>(player1.y)][static_cast<int>(player1.x)] == 1 && tileOwner[static_cast<int>(player1.y)][static_cast<int>(player1.x)] == 2)) {
////                        player1.alive = false;
////                    }
////                    else if (grid[static_cast<int>(player1.y)][static_cast<int>(player1.x)] == 0) {
////                        grid[static_cast<int>(player1.y)][static_cast<int>(player1.x)] = 2;
////                        tileOwner[static_cast<int>(player1.y)][static_cast<int>(player1.x)] = 1;
////                        player1.capturedTiles++;
////                    }
////                }
////
////                if (player2.alive && !player2.frozen) {
////                    player2.x += player2.dx;
////                    player2.y += player2.dy;
////
////                    if (player2.x < 0) player2.x = 0; if (player2.x > N - 1) player2.x = N - 1;
////                    if (player2.y < 0) player2.y = 0; if (player2.y > M - 1) player2.y = M - 1;
////
////                    player2.constructing = (grid[static_cast<int>(player2.y)][static_cast<int>(player2.x)] == 0);
////                    if (grid[static_cast<int>(player2.y)][static_cast<int>(player2.x)] == 3 && tileOwner[static_cast<int>(player2.y)][static_cast<int>(player2.x)] == 2) {
////                        player2.alive = false;
////                    }
////                    else if (grid[static_cast<int>(player2.y)][static_cast<int>(player2.x)] == 2 ||
////                        (grid[static_cast<int>(player2.y)][static_cast<int>(player2.x)] == 1 && tileOwner[static_cast<int>(player2.y)][static_cast<int>(player2.x)] == 1)) {
////                        player2.alive = false;
////                    }
////                    else if (grid[static_cast<int>(player2.y)][static_cast<int>(player2.x)] == 0) {
////                        grid[static_cast<int>(player2.y)][static_cast<int>(player2.x)] = 3;
////                        tileOwner[static_cast<int>(player2.y)][static_cast<int>(player2.x)] = 2;
////                        player2.capturedTiles++;
////                    }
////                }
////
////                if (player1.alive && player2.alive && static_cast<int>(player1.x) == static_cast<int>(player2.x) && static_cast<int>(player1.y) == static_cast<int>(player2.y)) {
////                    if (player1.constructing && player2.constructing) {
////                        player1.alive = false;
////                        player2.alive = false;
////                    }
////                    else if (player1.constructing) {
////                        player1.alive = false;
////                    }
////                    else if (player2.constructing) {
////                        player2.alive = false;
////                    }
////                    else {
////                        player1.alive = false;
////                        player2.alive = false;
////                    }
////                }
////
////                timer = 0;
////            }
////
////            for (int i = 0; i < enemyCount; i++) {
////                enemies[i].move(grid, time, ts);
////                int enemyY = static_cast<int>(enemies[i].y / ts);
////                int enemyX = static_cast<int>(enemies[i].x / ts);
////                if (enemyY >= 0 && enemyY < M && enemyX >= 0 && enemyX < N) {
////                    int enemyGridVal = grid[enemyY][enemyX];
////                    if (enemyGridVal == 2 && player1.alive && player1.constructing) player1.alive = false;
////                    if (enemyGridVal == 3 && player2.alive && player2.constructing) player2.alive = false;
////                }
////            }
////
////            if (player1.alive && grid[static_cast<int>(player1.y)][static_cast<int>(player1.x)] == 1) {
////                player1.dx = player1.dy = 0;
////                for (int i = 0; i < enemyCount; i++) {
////                    int enemyY = static_cast<int>(enemies[i].y / ts);
////                    int enemyX = static_cast<int>(enemies[i].x / ts);
////                    if (enemyY >= 0 && enemyY < M && enemyX >= 0 && enemyX < N)
////                        drop(enemyY, enemyX, grid);
////                }
////
////                int newlyCaptured = 0;
////                for (int i = 0; i < M; i++)
////                    for (int j = 0; j < N; j++) {
////                        if (grid[i][j] == -1) {
////                            grid[i][j] = 0;
////                            tileOwner[i][j] = 0;
////                        }
////                        else if (grid[i][j] == 2 && tileOwner[i][j] == 1) {
////                            grid[i][j] = 1;
////                            tileOwner[i][j] = 1;
////                            newlyCaptured++;
////                        }
////                    }
////
////                if (newlyCaptured > 0) {
////                    if (newlyCaptured > bonusThresholdP1) {
////                        player1.score += newlyCaptured * bonusMultiplierP1;
////                        bonusCountP1++;
////                        if (bonusCountP1 == 3) bonusThresholdP1 = 5;
////                        if (bonusCountP1 >= 5) bonusMultiplierP1 = 4;
////                    }
////                    else {
////                        player1.score += newlyCaptured;
////                    }
////                    cout << "Multiplayer P1: Captured " << newlyCaptured << " tiles, Score: " << player1.score << endl;
////
////                    if (player1.score >= 50 && (player1.score - newlyCaptured) < 50) player1.powerUps++;
////                    if (player1.score >= 70 && (player1.score - newlyCaptured) < 70) player1.powerUps++;
////                    if (player1.score >= 100 && (player1.score - newlyCaptured) < 100) player1.powerUps++;
////                    if (player1.score >= 130 && (player1.score - newlyCaptured) < 130) player1.powerUps++;
////                    if (player1.score >= 160 && ((player1.score - newlyCaptured) / 30 < player1.score / 30)) player1.powerUps++;
////                    cout << "Multiplayer P1: Power-Ups: " << player1.powerUps << endl;
////                }
////            }
////
////            if (player2.alive && grid[static_cast<int>(player2.y)][static_cast<int>(player2.x)] == 1) {
////                player2.dx = player2.dy = 0;
////                for (int i = 0; i < enemyCount; i++) {
////                    int enemyY = static_cast<int>(enemies[i].y / ts);
////                    int enemyX = static_cast<int>(enemies[i].x / ts);
////                    if (enemyY >= 0 && enemyY < M && enemyX >= 0 && enemyX < N)
////                        drop(enemyY, enemyX, grid);
////                }
////
////                int newlyCaptured = 0;
////                for (int i = 0; i < M; i++)
////                    for (int j = 0; j < N; j++) {
////                        if (grid[i][j] == -1) {
////                            grid[i][j] = 0;
////                            tileOwner[i][j] = 0;
////                        }
////                        else if (grid[i][j] == 3 && tileOwner[i][j] == 2) {
////                            grid[i][j] = 1;
////                            tileOwner[i][j] = 2;
////                            newlyCaptured++;
////                        }
////                    }
////
////                if (newlyCaptured > 0) {
////                    if (newlyCaptured > bonusThresholdP2) {
////                        player2.score += newlyCaptured * bonusMultiplierP2;
////                        bonusCountP2++;
////                        if (bonusCountP2 == 3) bonusThresholdP2 = 5;
////                        if (bonusCountP2 >= 5) bonusMultiplierP2 = 4;
////                    }
////                    else {
////                        player2.score += newlyCaptured;
////                    }
////                    cout << "Multiplayer P2: Captured " << newlyCaptured << " tiles, Score: " << player2.score << endl;
////
////                    if (player2.score >= 50 && (player2.score - newlyCaptured) < 50) player2.powerUps++;
////                    if (player2.score >= 70 && (player2.score - newlyCaptured) < 70) player2.powerUps++;
////                    if (player2.score >= 100 && (player2.score - newlyCaptured) < 100) player2.powerUps++;
////                    if (player2.score >= 130 && (player2.score - newlyCaptured) < 130) player2.powerUps++;
////                    if (player2.score >= 160 && ((player2.score - newlyCaptured) / 30 < player2.score / 30)) player2.powerUps++;
////                    cout << "Multiplayer P2: Power-Ups: " << player2.powerUps << endl;
////                }
////            }
////        }
////
////        if (player1.frozen && player1.frozenClock.getElapsedTime().asSeconds() >= 3) {
////            player1.frozen = false;
////        }
////        if (player2.frozen && player2.frozenClock.getElapsedTime().asSeconds() >= 3) {
////            player2.frozen = false;
////        }
////
////        if (!player1.alive && !player2.alive) {
////            Game = false;
////            string winner;
////            if (player1.score > player2.score)
////                winner = "Player 1 Wins!";
////            else if (player2.score > player1.score)
////                winner = "Player 2 Wins!";
////            else
////                winner = "It's a Tie!";
////            winnerText.setString(winner);
////        }
////
////        window.clear();
////        inventory.drawBackground(window);
////
////        for (int i = 0; i < M; i++)
////            for (int j = 0; j < N; j++) {
////                if (grid[i][j] == 0) continue;
////                sTile.setColor(Color::White);
////                if (grid[i][j] == 1) {
////                    sTile.setTextureRect(IntRect(0, 0, ts, ts));
////                    if (tileOwner[i][j] == 1)
////                        sTile.setColor(Color(135, 206, 250, 200));
////                    else if (tileOwner[i][j] == 2)
////                        sTile.setColor(Color(255, 99, 71, 200));
////                }
////                if (grid[i][j] == 2) {
////                    sTile.setTextureRect(IntRect(54, 0, ts, ts));
////                    sTile.setColor(player1.pathColor);
////                }
////                if (grid[i][j] == 3) {
////                    sTile.setTextureRect(IntRect(54, 0, ts, ts));
////                    sTile.setColor(player2.pathColor);
////                }
////                sTile.setPosition(j * ts + offsetX, i * ts + offsetY);
////                window.draw(sTile);
////            }
////
////        if (player1.alive) {
////            sTile.setColor(Color::White);
////            sTile.setTextureRect(IntRect(36, 0, ts, ts));
////            sTile.setPosition(static_cast<int>(player1.x) * ts + offsetX, static_cast<int>(player1.y) * ts + offsetY);
////            window.draw(sTile);
////        }
////
////        if (player2.alive) {
////            sTile.setColor(Color::White);
////            sTile.setTextureRect(IntRect(72, 0, ts, ts));
////            sTile.setPosition(static_cast<int>(player2.x) * ts + offsetX, static_cast<int>(player2.y) * ts + offsetY);
////            window.draw(sTile);
////        }
////
////        sEnemy.rotate(10);
////        for (int i = 0; i < enemyCount; i++) {
////            if (enemies[i].frozen) sEnemy.setColor(Color::Cyan);
////            else sEnemy.setColor(Color::White);
////            sEnemy.setPosition(enemies[i].x + offsetX, enemies[i].y + offsetY);
////            window.draw(sEnemy);
////        }
////
////        scoreTextP1.setString("P1 Score: " + to_string(player1.score));
////        powerUpTextP1.setString("P1 Power-Ups: " + to_string(player1.powerUps));
////        scoreTextP2.setString("P2 Score: " + to_string(player2.score));
////        powerUpTextP2.setString("P2 Power-Ups: " + to_string(player2.powerUps));
////
////        window.draw(scoreTextP1);
////        window.draw(powerUpTextP1);
////        window.draw(scoreTextP2);
////        window.draw(powerUpTextP2);
////
////        if (player1.showBackgroundMenu) {
////            for (const auto& option : backgroundOptions) {
////                window.draw(option);
////            }
////        }
////
////        if (!Game) {
////            window.draw(sGameover);
////            window.draw(winnerText);
////        }
////
////        window.display();
////    }
////}
//
//#include "game.h"
//#include <SFML/Audio.hpp>
//#include <time.h>
//#include <string>
//#include <sstream>
//#include <iostream>
//#include <algorithm>
//#include<vector>
//#include <cctype>
//
//using namespace std;
//using namespace sf;
//
//// Helper function to map background names to colors
//Color getColorForBackground(const string& name) {
//    string lowerName = name;
//    transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
//    if (lowerName == "blue") return Color(135, 206, 250); // Light blue for readability
//    if (lowerName == "white") return Color::White;
//    if (lowerName == "red") return Color::Red;
//    return Color::White; // Default for unknown names
//}
//
//// Helper function to create a styled button
//RectangleShape createButton(float x, float y, float width, float height, Color fillColor) {
//    RectangleShape button(Vector2f(width, height));
//    button.setPosition(x, y);
//    button.setFillColor(fillColor);
//    button.setOutlineColor(Color(50, 50, 50));
//    button.setOutlineThickness(2);
//    return button;
//}
//
//// Enemy implementation
//Enemy::Enemy(int grid[M][N], int tileSize) {
//    // Random position within playable area (not on borders)
//    int maxX = N - 2, maxY = M - 2;
//    do {
//        x = (rand() % maxX + 1) * tileSize + tileSize / 2.0f;
//        y = (rand() % maxY + 1) * tileSize + tileSize / 2.0f;
//    } while (grid[static_cast<int>(y / tileSize)][static_cast<int>(x / tileSize)] != 0);
//
//    // Random initial direction with normalized speed
//    float angle = static_cast<float>(rand()) / RAND_MAX * 2 * 3.14159f;
//    dx = cos(angle);
//    dy = sin(angle);
//    frozen = false;
//    speed = 90.0f; // Default speed (pixels/second), adjustable by level
//}
//
//void Enemy::move(int grid[M][N], float deltaTime, int tileSize) {
//    if (frozen && frozenClock.getElapsedTime().asSeconds() < 3) return;
//    frozen = false;
//
//    // Move smoothly based on speed and deltaTime
//    float moveX = dx * speed * deltaTime;
//    float moveY = dy * speed * deltaTime;
//    float newX = x + moveX;
//    float newY = y + moveY;
//
//    // Check boundaries and grid collision
//    int gridX = static_cast<int>(newX / tileSize);
//    int gridY = static_cast<int>(newY / tileSize);
//    bool collideHorizontal = false, collideVertical = false;
//
//    // Check grid boundaries
//    if (gridX < 0 || gridX >= N) {
//        collideHorizontal = true;
//    }
//    if (gridY < 0 || gridY >= M) {
//        collideVertical = true;
//    }
//
//    // Check grid tiles
//    if (!collideHorizontal && !collideVertical && gridY >= 0 && gridY < M && gridX >= 0 && gridX < N) {
//        if (grid[gridY][gridX] == 1) {
//            // Determine collision side by checking position relative to tile center
//            float tileCenterX = gridX * tileSize + tileSize / 2.0f;
//            float tileCenterY = gridY * tileSize + tileSize / 2.0f;
//            float dxToTile = newX - tileCenterX;
//            float dyToTile = newY - tileCenterY;
//
//            // Approximate collision based on dominant direction
//            if (abs(dxToTile) > abs(dyToTile)) {
//                collideHorizontal = true; // Hit left or right side
//            }
//            else {
//                collideVertical = true; // Hit top or bottom side
//            }
//        }
//    }
//
//    if (collideHorizontal || collideVertical) {
//        // Bounce like a ball
//        if (collideHorizontal) {
//            dx = -dx; // Reverse horizontal direction
//        }
//        if (collideVertical) {
//            dy = -dy; // Reverse vertical direction
//        }
//
//        // Add slight randomization to avoid repetitive paths
//        float angle = atan2(dy, dx) + (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.5f;
//        dx = cos(angle);
//        dy = sin(angle);
//
//        // Ensure non-zero movement
//        if (abs(dx) < 0.1f && abs(dy) < 0.1f) {
//            dx = rand() % 2 ? 1.0f : -1.0f;
//            dy = rand() % 2 ? 1.0f : -1.0f;
//            float len = sqrt(dx * dx + dy * dy);
//            dx /= len;
//            dy /= len;
//        }
//
//        // Recalculate position with new direction (partial move to avoid passing through)
//        newX = x + dx * speed * deltaTime;
//        newY = y + dy * speed * deltaTime;
//    }
//
//    // Update position
//    x = newX;
//    y = newY;
//
//    // Keep within bounds
//    if (x < tileSize / 2.0f) { x = tileSize / 2.0f; dx = abs(dx); }
//    if (x > (N - 0.5f) * tileSize) { x = (N - 0.5f) * tileSize; dx = -abs(dx); }
//    if (y < tileSize / 2.0f) { y = tileSize / 2.0f; dy = abs(dy); }
//    if (y > (M - 0.5f) * tileSize) { y = (M - 0.5f) * tileSize; dy = -abs(dy); }
//}
//
//void Enemy::activateFreeze() {
//    frozen = true;
//    frozenClock.restart();
//}
//
//// Drop implementation (flood-fill to mark unreachable areas)
//void drop(int y, int x, int grid[M][N]) {
//    if (y < 0 || y >= M || x < 0 || x >= N) return;
//    if (grid[y][x] == 0) grid[y][x] = -1;
//    if (y > 0 && grid[y - 1][x] == 0) drop(y - 1, x, grid);
//    if (y < M - 1 && grid[y + 1][x] == 0) drop(y + 1, x, grid);
//    if (x > 0 && grid[y][x - 1] == 0) drop(y, x - 1, grid);
//    if (x < N - 1 && grid[y][x + 1] == 0) drop(y, x + 1, grid);
//}
//
//// Level selection struct
//struct Level {
//    string name;
//    int enemyCount;
//    float enemySpeed;
//};
//
//// Single-player level selection
//int selectLevel(RenderWindow& window, Font& font, Inventory& inventory) {
//    window.setTitle("Select Level");
//
//    // Level definitions (scaled for ts=18)
//    vector<Level> levels = {
//        {"Easy", 2, 45.0f},
//        {"Medium", 4, 90.0f},
//        {"Hard", 6, 135.0f}
//    };
//
//    // UI elements
//    Text title("Select Level", font, 36);
//    title.setFillColor(Color::White);
//    title.setStyle(Text::Bold);
//    title.setPosition(330, 40);
//    title.setOutlineColor(Color::Black);
//    title.setOutlineThickness(1);
//
//    vector<RectangleShape> levelButtonBgs;
//    vector<Text> levelButtons;
//    for (size_t i = 0; i < levels.size(); ++i) {
//        RectangleShape bg = createButton(320, 150 + i * 60, 160, 40, Color(50, 150, 50));
//        Text button(levels[i].name, font, 22);
//        button.setFillColor(Color::White);
//        button.setPosition(350, 155 + i * 60);
//        levelButtonBgs.push_back(bg);
//        levelButtons.push_back(button);
//    }
//
//    while (window.isOpen()) {
//        Event event;
//        while (window.pollEvent(event)) {
//            if (event.type == Event::Closed) {
//                window.close();
//                return -1; // Indicate closure
//            }
//
//            if (event.type == Event::MouseButtonPressed) {
//                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
//                for (size_t i = 0; i < levelButtons.size(); ++i) {
//                    if (levelButtons[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
//                        return static_cast<int>(i); // Return level index
//                    }
//                }
//            }
//        }
//
//        // Hover effects
//        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
//        for (size_t i = 0; i < levelButtonBgs.size(); ++i) {
//            levelButtonBgs[i].setFillColor(levelButtons[i].getGlobalBounds().contains(mousePos.x, mousePos.y) ? Color(70, 180, 70) : Color(50, 150, 50));
//        }
//
//        window.clear();
//        inventory.drawBackground(window);
//        window.draw(title);
//        for (size_t i = 0; i < levelButtons.size(); ++i) {
//            window.draw(levelButtonBgs[i]);
//            window.draw(levelButtons[i]);
//        }
//        window.display();
//    }
//    return -1; // Fallback
//}
//
//// Single-player game
//void playXonixGame(PlayerList& pl, RenderWindow& window, Font& font, const string& currentUser, Inventory& inventory) {
//    window.setTitle("Xonix Game");
//    const int windowWidth = 800;
//    const int windowHeight = 600;
//    const int ts = 18; // Tile size
//    const float offsetX = (windowWidth - N * ts) / 2.0f; // 40
//    const float offsetY = (windowHeight - M * ts) / 2.0f; // 120
//    window.setSize(Vector2u(windowWidth, windowHeight));
//    srand(time(0));
//
//    // Level selection
//    int levelIndex = selectLevel(window, font, inventory);
//    if (levelIndex == -1) return; // Window closed
//
//    vector<Level> levels = {
//        {"Easy", 2, 45.0f},
//        {"Medium", 4, 90.0f},
//        {"Hard", 6, 135.0f}
//    };
//    int enemyCount = levels[levelIndex].enemyCount;
//    float enemySpeed = levels[levelIndex].enemySpeed;
//
//    int grid[M][N] = { 0 };
//
//    Texture t1, t2, t3;
//    if (!t1.loadFromFile("images/tiles.png") || !t2.loadFromFile("images/gameover.png") || !t3.loadFromFile("images/enemy.png")) {
//        cout << "Error: Could not load game textures.\n";
//        return;
//    }
//
//    Sprite sTile(t1), sGameover(t2), sEnemy(t3);
//    sGameover.setPosition(200.0f + offsetX, 150.0f + offsetY);
//    sEnemy.setOrigin(20, 20);
//
//    Text scoreText("", font, 20);
//    scoreText.setFillColor(Color::White);
//    scoreText.setPosition(20.0f + offsetX, 15.0f + offsetY);
//
//    Text powerUpText("", font, 20);
//    powerUpText.setFillColor(Color::Green);
//    powerUpText.setPosition(20.0f + offsetX, 40.0f + offsetY);
//
//    // Background selection UI
//    vector<Text> backgroundOptions;
//    string namesString = inventory.getBackgroundNames();
//    vector<string> bgNames;
//    stringstream ss(namesString);
//    string name;
//    while (getline(ss, name)) {
//        if (!name.empty()) {
//            bgNames.push_back(name);
//        }
//    }
//    for (size_t i = 0; i < bgNames.size(); ++i) {
//        Text option;
//        option.setFont(font);
//        option.setString(bgNames[i]);
//        option.setCharacterSize(18);
//        option.setPosition(20.0f + offsetX, 70.0f + offsetY + i * 25.0f);
//        option.setFillColor(i + 1 == inventory.getCurrentBackgroundID() ? getColorForBackground(bgNames[i]) : Color::White);
//        backgroundOptions.push_back(option);
//    }
//
//    Enemy enemies[10];
//    for (int i = 0; i < enemyCount; ++i) {
//        enemies[i] = Enemy(grid, ts);
//        enemies[i].speed = enemySpeed;
//    }
//
//    bool Game = true;
//    PlayerState player;
//    player.x = 10;
//    player.y = 0; // Start on border
//    float timer = 0, delay = 0.07;
//    Clock clock;
//
//    int bonusCount = 0, bonusThreshold = 10, bonusMultiplier = 2;
//
//    for (int i = 0; i < M; i++)
//        for (int j = 0; j < N; j++)
//            if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
//                grid[i][j] = 1;
//
//    // Start background music
//    Music& music = inventory.getCurrentSound();
//    music.setLoop(true);
//    music.play();
//
//    while (window.isOpen()) {
//        float time = clock.getElapsedTime().asSeconds();
//        clock.restart();
//        timer += time;
//
//        Event e;
//        while (window.pollEvent(e)) {
//            if (e.type == Event::Closed) {
//                music.stop();
//                Player* p = pl.getPlayerByUsername(currentUser);
//                if (p) {
//                    pl.savePlayerStats(currentUser, player.score, player.powerUps, p->preferredThemeID, p->preferredSoundID);
//                }
//                else {
//                    cerr << "Error: Player " << currentUser << " not found for saving stats.\n";
//                }
//                window.close();
//                return;
//            }
//
//            if (e.type == Event::KeyPressed) {
//                if (e.key.code == Keyboard::Escape) {
//                    if (!Game) {
//                        music.stop();
//                        Player* p = pl.getPlayerByUsername(currentUser);
//                        if (p) {
//                            pl.savePlayerStats(currentUser, player.score, player.powerUps, p->preferredThemeID, p->preferredSoundID);
//                        }
//                        else {
//                            cerr << "Error: Player " << currentUser << " not found for saving stats.\n";
//                        }
//                        return;
//                    }
//                    else {
//                        for (int i = 1; i < M - 1; i++)
//                            for (int j = 1; j < N - 1; j++)
//                                grid[i][j] = 0;
//                        player = PlayerState();
//                        player.x = 10;
//                        player.y = 0; // Reset to border
//                        Game = true;
//                        bonusCount = 0;
//                        bonusThreshold = 10;
//                        bonusMultiplier = 2;
//                        for (int i = 0; i < enemyCount; i++) {
//                            enemies[i] = Enemy(grid, ts);
//                            enemies[i].speed = enemySpeed;
//                        }
//                        music.stop();
//                        music.play(); // Restart music after reset
//                    }
//                }
//                if (e.key.code == Keyboard::Space && player.powerUps > 0 && Game) {
//                    for (int i = 0; i < enemyCount; i++)
//                        enemies[i].activateFreeze();
//                    player.powerUps--;
//                }
//                if (e.key.code == Keyboard::B) {
//                    player.showBackgroundMenu = !player.showBackgroundMenu;
//                }
//            }
//
//            if (e.type == Event::MouseButtonPressed && player.showBackgroundMenu) {
//                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
//                for (size_t i = 0; i < backgroundOptions.size(); ++i) {
//                    if (backgroundOptions[i].getGlobalBounds().contains(mousePos)) {
//                        inventory.setBackground(i + 1);
//                        player.showBackgroundMenu = false;
//                        for (size_t j = 0; j < backgroundOptions.size(); ++j) {
//                            backgroundOptions[j].setFillColor(j + 1 == inventory.getCurrentBackgroundID() ? getColorForBackground(bgNames[j]) : Color::White);
//                        }
//                        break;
//                    }
//                }
//            }
//        }
//
//        if (Game && !player.frozen) {
//            if (Keyboard::isKeyPressed(Keyboard::Left)) { player.dx = -1; player.dy = 0; }
//            if (Keyboard::isKeyPressed(Keyboard::Right)) { player.dx = 1; player.dy = 0; }
//            if (Keyboard::isKeyPressed(Keyboard::Up)) { player.dx = 0; player.dy = -1; }
//            if (Keyboard::isKeyPressed(Keyboard::Down)) { player.dx = 0; player.dy = 1; }
//
//            if (timer > delay) {
//                player.x += player.dx;
//                player.y += player.dy;
//
//                if (player.x < 0) player.x = 0; if (player.x > N - 1) player.x = N - 1;
//                if (player.y < 0) player.y = 0; if (player.y > M - 1) player.y = M - 1;
//
//                player.constructing = (grid[static_cast<int>(player.y)][static_cast<int>(player.x)] == 0);
//                if (grid[static_cast<int>(player.y)][static_cast<int>(player.x)] == 2) Game = false;
//                if (grid[static_cast<int>(player.y)][static_cast<int>(player.x)] == 0) {
//                    grid[static_cast<int>(player.y)][static_cast<int>(player.x)] = 2;
//                    player.capturedTiles++;
//                }
//                timer = 0;
//            }
//
//            for (int i = 0; i < enemyCount; i++) enemies[i].move(grid, time, ts);
//
//            if (grid[static_cast<int>(player.y)][static_cast<int>(player.x)] == 1) {
//                player.dx = player.dy = 0;
//
//                for (int i = 0; i < enemyCount; i++) {
//                    int enemyY = static_cast<int>(enemies[i].y / ts);
//                    int enemyX = static_cast<int>(enemies[i].x / ts);
//                    if (enemyY >= 0 && enemyY < M && enemyX >= 0 && enemyX < N)
//                        drop(enemyY, enemyX, grid);
//                }
//
//                int newlyCaptured = 0;
//                for (int i = 0; i < M; i++)
//                    for (int j = 0; j < N; j++) {
//                        if (grid[i][j] == -1) grid[i][j] = 0;
//                        else if (grid[i][j] == 2) { grid[i][j] = 1; newlyCaptured++; }
//                    }
//
//                if (newlyCaptured > 0) {
//                    if (newlyCaptured > bonusThreshold) {
//                        player.score += newlyCaptured * bonusMultiplier;
//                        bonusCount++;
//                        if (bonusCount == 3) bonusThreshold = 5;
//                        if (bonusCount >= 5) bonusMultiplier = 4;
//                    }
//                    else {
//                        player.score += newlyCaptured;
//                    }
//                    cout << "Single-player: Captured " << newlyCaptured << " tiles, Score: " << player.score << endl;
//
//                    if (player.score >= 50 && (player.score - newlyCaptured) < 50) player.powerUps++;
//                    if (player.score >= 70 && (player.score - newlyCaptured) < 70) player.powerUps++;
//                    if (player.score >= 100 && (player.score - newlyCaptured) < 100) player.powerUps++;
//                    if (player.score >= 130 && (player.score - newlyCaptured) < 130) player.powerUps++;
//                    if (player.score >= 160 && ((player.score - newlyCaptured) / 30 < player.score / 30)) player.powerUps++;
//                    cout << "Single-player: Power-Ups: " << player.powerUps << endl;
//                }
//            }
//
//            for (int i = 0; i < enemyCount; i++) {
//                int enemyY = static_cast<int>(enemies[i].y / ts);
//                int enemyX = static_cast<int>(enemies[i].x / ts);
//                if (enemyY >= 0 && enemyY < M && enemyX >= 0 && enemyX < N) {
//                    if (grid[enemyY][enemyX] == 2) Game = false;
//                }
//            }
//        }
//
//        if (player.frozen && player.frozenClock.getElapsedTime().asSeconds() >= 3) {
//            player.frozen = false;
//        }
//
//        window.clear();
//        inventory.drawBackground(window);
//
//        for (int i = 0; i < M; i++)
//            for (int j = 0; j < N; j++) {
//                if (grid[i][j] == 0) continue;
//                if (grid[i][j] == 1) sTile.setTextureRect(IntRect(0, 0, ts, ts));
//                if (grid[i][j] == 2) sTile.setTextureRect(IntRect(54, 0, ts, ts));
//                sTile.setPosition(j * ts + offsetX, i * ts + offsetY);
//                window.draw(sTile);
//            }
//
//        sTile.setTextureRect(IntRect(36, 0, ts, ts));
//        sTile.setPosition(static_cast<int>(player.x) * ts + offsetX, static_cast<int>(player.y) * ts + offsetY);
//        window.draw(sTile);
//
//        sEnemy.rotate(10);
//        for (int i = 0; i < enemyCount; i++) {
//            if (enemies[i].frozen) sEnemy.setColor(Color::Cyan);
//            else sEnemy.setColor(Color::White);
//            sEnemy.setPosition(enemies[i].x + offsetX, enemies[i].y + offsetY);
//            window.draw(sEnemy);
//        }
//
//        scoreText.setString("Score: " + to_string(player.score));
//        powerUpText.setString("Power-Ups: " + to_string(player.powerUps));
//        window.draw(scoreText);
//        window.draw(powerUpText);
//
//        if (player.showBackgroundMenu) {
//            for (const auto& option : backgroundOptions) {
//                window.draw(option);
//            }
//        }
//
//        if (!Game) window.draw(sGameover);
//
//        window.display();
//    }
//}
//
//// Multiplayer mode
//void playMultiplayerXonixGame(PlayerList& pl, RenderWindow& window, Font& font, const string& player1User, const string& player2User, Inventory& inventory) {
//    window.setTitle("Xonix Multiplayer Game");
//
//    srand(time(0));
//
//    int grid[M][N] = { 0 };
//    int tileOwner[M][N] = { 0 };
//
//    const int ts = 18; // Tile size
//    const float offsetX = (800 - N * ts) / 2.0f; // 40
//    const float offsetY = (600 - M * ts) / 2.0f; // 120
//
//    Texture t1, t2, t3;
//    if (!t1.loadFromFile("images/tiles.png") || !t2.loadFromFile("images/gameover.png") || !t3.loadFromFile("images/enemy.png")) {
//        cout << "Error: Could not load game textures.\n";
//        return;
//    }
//
//    Sprite sTile(t1), sGameover(t2), sEnemy(t3);
//    sGameover.setPosition(200.0f + offsetX, 150.0f + offsetY);
//    sEnemy.setOrigin(20, 20);
//
//    Text scoreTextP1("", font, 20);
//    scoreTextP1.setFillColor(Color::White);
//    scoreTextP1.setPosition(20.0f + offsetX, 15.0f + offsetY);
//
//    Text powerUpTextP1("", font, 20);
//    powerUpTextP1.setFillColor(Color(100, 255, 100));
//    powerUpTextP1.setPosition(20.0f + offsetX, 40.0f + offsetY);
//
//    Text scoreTextP2("", font, 20);
//    scoreTextP2.setFillColor(Color::White);
//    scoreTextP2.setPosition(static_cast<float>(N * ts - 190) + offsetX, 15.0f + offsetY);
//
//    Text powerUpTextP2("", font, 20);
//    powerUpTextP2.setFillColor(Color(100, 255, 100));
//    powerUpTextP2.setPosition(static_cast<float>(N * ts - 190) + offsetX, 40.0f + offsetY);
//
//    Text winnerText("", font, 30);
//    winnerText.setFillColor(Color::Yellow);
//    winnerText.setStyle(Text::Bold);
//    winnerText.setPosition(300.0f + offsetX, 120.0f + offsetY);
//    winnerText.setOutlineColor(Color::Black);
//    winnerText.setOutlineThickness(1);
//
//    vector<Text> backgroundOptions;
//    string namesString = inventory.getBackgroundNames();
//    vector<string> bgNames;
//    stringstream ss(namesString);
//    string name;
//    while (getline(ss, name)) {
//        if (!name.empty()) {
//            bgNames.push_back(name);
//        }
//    }
//    for (size_t i = 0; i < bgNames.size(); ++i) {
//        Text option;
//        option.setFont(font);
//        option.setString(bgNames[i]);
//        option.setCharacterSize(18);
//        option.setPosition(20.0f + offsetX, 70.0f + offsetY + i * 25.0f);
//        option.setFillColor(i + 1 == inventory.getCurrentBackgroundID() ? getColorForBackground(bgNames[i]) : Color::White);
//        backgroundOptions.push_back(option);
//    }
//
//    int enemyCount = 3 + (rand() % 3); // Random 3 to 5 enemies
//    Enemy enemies[10];
//    for (int i = 0; i < enemyCount; ++i) {
//        enemies[i] = Enemy(grid, ts);
//    }
//
//    bool Game = true;
//    PlayerState player1, player2;
//    player1.x = 10;
//    player1.y = 0;
//    player1.pathColor = Color::Blue;
//    player2.x = N - 10;
//    player2.y = 0;
//    player2.pathColor = Color::Red;
//
//    float timer = 0, delay = 0.07;
//    Clock clock;
//
//    int bonusCountP1 = 0, bonusThresholdP1 = 10, bonusMultiplierP1 = 2;
//    int bonusCountP2 = 0, bonusThresholdP2 = 10, bonusMultiplierP2 = 2;
//
//    for (int i = 0; i < M; i++)
//        for (int j = 0; j < N; j++)
//            if (i == 0 || j == 0 || i == M - 1 || j == N - 1) {
//                grid[i][j] = 1;
//                tileOwner[i][j] = 0;
//            }
//
//    // Start background music
//    Music& music = inventory.getCurrentSound();
//    music.setLoop(true);
//    music.play();
//
//    while (window.isOpen()) {
//        float time = clock.getElapsedTime().asSeconds();
//        clock.restart();
//        timer += time;
//
//        Event e;
//        while (window.pollEvent(e)) {
//            if (e.type == Event::Closed) {
//                music.stop();
//                Player* p1 = pl.getPlayerByUsername(player1User);
//                if (p1) {
//                    pl.savePlayerStats(player1User, player1.score, player1.powerUps, p1->preferredThemeID, p1->preferredSoundID);
//                }
//                else {
//                    cerr << "Error: Player " << player1User << " not found for saving stats.\n";
//                }
//                Player* p2 = pl.getPlayerByUsername(player2User);
//                if (p2) {
//                    pl.savePlayerStats(player2User, player2.score, player2.powerUps, p2->preferredThemeID, p2->preferredSoundID);
//                }
//                else {
//                    cerr << "Error: Player " << player2User << " not found for saving stats.\n";
//                }
//                window.close();
//                return;
//            }
//
//            if (e.type == Event::KeyPressed) {
//                if (e.key.code == Keyboard::Escape) {
//                    if (!Game || (!player1.alive && !player2.alive)) {
//                        music.stop();
//                        Player* p1 = pl.getPlayerByUsername(player1User);
//                        if (p1) {
//                            pl.savePlayerStats(player1User, player1.score, player1.powerUps, p1->preferredThemeID, p1->preferredSoundID);
//                        }
//                        else {
//                            cerr << "Error: Player " << player1User << " not found for saving stats.\n";
//                        }
//                        Player* p2 = pl.getPlayerByUsername(player2User);
//                        if (p2) {
//                            pl.savePlayerStats(player2User, player2.score, player2.powerUps, p2->preferredThemeID, p2->preferredSoundID);
//                        }
//                        else {
//                            cerr << "Error: Player " << player2User << " not found for saving stats.\n";
//                        }
//                        return;
//                    }
//                    else {
//                        for (int i = 0; i < M; i++)
//                            for (int j = 0; j < N; j++) {
//                                if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
//                                    grid[i][j] = 1;
//                                else
//                                    grid[i][j] = 0;
//                                tileOwner[i][j] = 0;
//                            }
//                        player1 = PlayerState();
//                        player2 = PlayerState();
//                        player1.x = 10;
//                        player1.y = 0;
//                        player1.pathColor = Color::Blue;
//                        player2.x = N - 10;
//                        player2.y = 0;
//                        player2.pathColor = Color::Red;
//                        Game = true;
//                        bonusCountP1 = bonusCountP2 = 0;
//                        bonusThresholdP1 = bonusThresholdP2 = 10;
//                        bonusMultiplierP1 = bonusMultiplierP2 = 2;
//                        enemyCount = 3 + (rand() % 3); // Random 3 to 5 enemies on reset
//                        for (int i = 0; i < enemyCount; i++)
//                            enemies[i] = Enemy(grid, ts);
//                        music.stop();
//                        music.play(); // Restart music after reset
//                    }
//                }
//                if (e.key.code == Keyboard::Space && player1.powerUps > 0 && Game && player1.alive) {
//                    for (int i = 0; i < enemyCount; i++)
//                        enemies[i].activateFreeze();
//                    player2.frozen = true;
//                    player2.frozenClock.restart();
//                    player1.powerUps--;
//                }
//                if (e.key.code == Keyboard::Return && player2.powerUps > 0 && Game && player2.alive) {
//                    for (int i = 0; i < enemyCount; i++)
//                        enemies[i].activateFreeze();
//                    player1.frozen = true;
//                    player1.frozenClock.restart();
//                    player2.powerUps--;
//                }
//                if (e.key.code == Keyboard::B) {
//                    player1.showBackgroundMenu = !player1.showBackgroundMenu;
//                    player2.showBackgroundMenu = player1.showBackgroundMenu;
//                }
//            }
//
//            if (e.type == Event::MouseButtonPressed && player1.showBackgroundMenu) {
//                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
//                for (size_t i = 0; i < backgroundOptions.size(); ++i) {
//                    if (backgroundOptions[i].getGlobalBounds().contains(mousePos)) {
//                        inventory.setBackground(i + 1);
//                        player1.showBackgroundMenu = false;
//                        player2.showBackgroundMenu = false;
//                        for (size_t j = 0; j < backgroundOptions.size(); ++j) {
//                            backgroundOptions[j].setFillColor(j + 1 == inventory.getCurrentBackgroundID() ? getColorForBackground(bgNames[j]) : Color::White);
//                        }
//                        break;
//                    }
//                }
//            }
//        }
//
//        if (Game) {
//            if (!player1.frozen && player1.alive) {
//                if (Keyboard::isKeyPressed(Keyboard::A)) { player1.dx = -1; player1.dy = 0; }
//                if (Keyboard::isKeyPressed(Keyboard::D)) { player1.dx = 1; player1.dy = 0; }
//                if (Keyboard::isKeyPressed(Keyboard::W)) { player1.dx = 0; player1.dy = -1; }
//                if (Keyboard::isKeyPressed(Keyboard::S)) { player1.dx = 0; player1.dy = 1; }
//            }
//
//            if (!player2.frozen && player2.alive) {
//                if (Keyboard::isKeyPressed(Keyboard::Left)) { player2.dx = -1; player2.dy = 0; }
//                if (Keyboard::isKeyPressed(Keyboard::Right)) { player2.dx = 1; player2.dy = 0; }
//                if (Keyboard::isKeyPressed(Keyboard::Up)) { player2.dx = 0; player2.dy = -1; }
//                if (Keyboard::isKeyPressed(Keyboard::Down)) { player2.dx = 0; player2.dy = 1; }
//            }
//
//            if (timer > delay) {
//                if (player1.alive && !player1.frozen) {
//                    player1.x += player1.dx;
//                    player1.y += player1.dy;
//
//                    if (player1.x < 0) player1.x = 0; if (player1.x > N - 1) player1.x = N - 1;
//                    if (player1.y < 0) player1.y = 0; if (player1.y > M - 1) player1.y = M - 1;
//
//                    player1.constructing = (grid[static_cast<int>(player1.y)][static_cast<int>(player1.x)] == 0);
//                    if (grid[static_cast<int>(player1.y)][static_cast<int>(player1.x)] == 2 && tileOwner[static_cast<int>(player1.y)][static_cast<int>(player1.x)] == 1) {
//                        player1.alive = false;
//                    }
//                    else if (grid[static_cast<int>(player1.y)][static_cast<int>(player1.x)] == 3 ||
//                        (grid[static_cast<int>(player1.y)][static_cast<int>(player1.x)] == 1 && tileOwner[static_cast<int>(player1.y)][static_cast<int>(player1.x)] == 2)) {
//                        player1.alive = false;
//                    }
//                    else if (grid[static_cast<int>(player1.y)][static_cast<int>(player1.x)] == 0) {
//                        grid[static_cast<int>(player1.y)][static_cast<int>(player1.x)] = 2;
//                        tileOwner[static_cast<int>(player1.y)][static_cast<int>(player1.x)] = 1;
//                        player1.capturedTiles++;
//                    }
//                }
//
//                if (player2.alive && !player2.frozen) {
//                    player2.x += player2.dx;
//                    player2.y += player2.dy;
//
//                    if (player2.x < 0) player2.x = 0; if (player2.x > N - 1) player2.x = N - 1;
//                    if (player2.y < 0) player2.y = 0; if (player2.y > M - 1) player2.y = M - 1;
//
//                    player2.constructing = (grid[static_cast<int>(player2.y)][static_cast<int>(player2.x)] == 0);
//                    if (grid[static_cast<int>(player2.y)][static_cast<int>(player2.x)] == 3 && tileOwner[static_cast<int>(player2.y)][static_cast<int>(player2.x)] == 2) {
//                        player2.alive = false;
//                    }
//                    else if (grid[static_cast<int>(player2.y)][static_cast<int>(player2.x)] == 2 ||
//                        (grid[static_cast<int>(player2.y)][static_cast<int>(player2.x)] == 1 && tileOwner[static_cast<int>(player2.y)][static_cast<int>(player2.x)] == 1)) {
//                        player2.alive = false;
//                    }
//                    else if (grid[static_cast<int>(player2.y)][static_cast<int>(player2.x)] == 0) {
//                        grid[static_cast<int>(player2.y)][static_cast<int>(player2.x)] = 3;
//                        tileOwner[static_cast<int>(player2.y)][static_cast<int>(player2.x)] = 2;
//                        player2.capturedTiles++;
//                    }
//                }
//
//                if (player1.alive && player2.alive && static_cast<int>(player1.x) == static_cast<int>(player2.x) && static_cast<int>(player1.y) == static_cast<int>(player2.y)) {
//                    if (player1.constructing && player2.constructing) {
//                        player1.alive = false;
//                        player2.alive = false;
//                    }
//                    else if (player1.constructing) {
//                        player1.alive = false;
//                    }
//                    else if (player2.constructing) {
//                        player2.alive = false;
//                    }
//                    else {
//                        player1.alive = false;
//                        player2.alive = false;
//                    }
//                }
//
//                timer = 0;
//            }
//
//            for (int i = 0; i < enemyCount; i++) {
//                enemies[i].move(grid, time, ts);
//                int enemyY = static_cast<int>(enemies[i].y / ts);
//                int enemyX = static_cast<int>(enemies[i].x / ts);
//                if (enemyY >= 0 && enemyY < M && enemyX >= 0 && enemyX < N) {
//                    int enemyGridVal = grid[enemyY][enemyX];
//                    if (enemyGridVal == 2 && player1.alive && player1.constructing) player1.alive = false;
//                    if (enemyGridVal == 3 && player2.alive && player2.constructing) player2.alive = false;
//                }
//            }
//
//            if (player1.alive && grid[static_cast<int>(player1.y)][static_cast<int>(player1.x)] == 1) {
//                player1.dx = player1.dy = 0;
//                for (int i = 0; i < enemyCount; i++) {
//                    int enemyY = static_cast<int>(enemies[i].y / ts);
//                    int enemyX = static_cast<int>(enemies[i].x / ts);
//                    if (enemyY >= 0 && enemyY < M && enemyX >= 0 && enemyX < N)
//                        drop(enemyY, enemyX, grid);
//                }
//
//                int newlyCaptured = 0;
//                for (int i = 0; i < M; i++)
//                    for (int j = 0; j < N; j++) {
//                        if (grid[i][j] == -1) {
//                            grid[i][j] = 0;
//                            tileOwner[i][j] = 0;
//                        }
//                        else if (grid[i][j] == 2 && tileOwner[i][j] == 1) {
//                            grid[i][j] = 1;
//                            tileOwner[i][j] = 1;
//                            newlyCaptured++;
//                        }
//                    }
//
//                if (newlyCaptured > 0) {
//                    if (newlyCaptured > bonusThresholdP1) {
//                        player1.score += newlyCaptured * bonusMultiplierP1;
//                        bonusCountP1++;
//                        if (bonusCountP1 == 3) bonusThresholdP1 = 5;
//                        if (bonusCountP1 >= 5) bonusMultiplierP1 = 4;
//                    }
//                    else {
//                        player1.score += newlyCaptured;
//                    }
//                    cout << "Multiplayer P1: Captured " << newlyCaptured << " tiles, Score: " << player1.score << endl;
//
//                    if (player1.score >= 50 && (player1.score - newlyCaptured) < 50) player1.powerUps++;
//                    if (player1.score >= 70 && (player1.score - newlyCaptured) < 70) player1.powerUps++;
//                    if (player1.score >= 100 && (player1.score - newlyCaptured) < 100) player1.powerUps++;
//                    if (player1.score >= 130 && (player1.score - newlyCaptured) < 130) player1.powerUps++;
//                    if (player1.score >= 160 && ((player1.score - newlyCaptured) / 30 < player1.score / 30)) player1.powerUps++;
//                    cout << "Multiplayer P1: Power-Ups: " << player1.powerUps << endl;
//                }
//            }
//
//            if (player2.alive && grid[static_cast<int>(player2.y)][static_cast<int>(player2.x)] == 1) {
//                player2.dx = player2.dy = 0;
//                for (int i = 0; i < enemyCount; i++) {
//                    int enemyY = static_cast<int>(enemies[i].y / ts);
//                    int enemyX = static_cast<int>(enemies[i].x / ts);
//                    if (enemyY >= 0 && enemyY < M && enemyX >= 0 && enemyX < N)
//                        drop(enemyY, enemyX, grid);
//                }
//
//                int newlyCaptured = 0;
//                for (int i = 0; i < M; i++)
//                    for (int j = 0; j < N; j++) {
//                        if (grid[i][j] == -1) {
//                            grid[i][j] = 0;
//                            tileOwner[i][j] = 0;
//                        }
//                        else if (grid[i][j] == 3 && tileOwner[i][j] == 2) {
//                            grid[i][j] = 1;
//                            tileOwner[i][j] = 2;
//                            newlyCaptured++;
//                        }
//                    }
//
//                if (newlyCaptured > 0) {
//                    if (newlyCaptured > bonusThresholdP2) {
//                        player2.score += newlyCaptured * bonusMultiplierP2;
//                        bonusCountP2++;
//                        if (bonusCountP2 == 3) bonusThresholdP2 = 5;
//                        if (bonusCountP2 >= 5) bonusMultiplierP2 = 4;
//                    }
//                    else {
//                        player2.score += newlyCaptured;
//                    }
//                    cout << "Multiplayer P2: Captured " << newlyCaptured << " tiles, Score: " << player2.score << endl;
//
//                    if (player2.score >= 50 && (player2.score - newlyCaptured) < 50) player2.powerUps++;
//                    if (player2.score >= 70 && (player2.score - newlyCaptured) < 70) player2.powerUps++;
//                    if (player2.score >= 100 && (player2.score - newlyCaptured) < 100) player2.powerUps++;
//                    if (player2.score >= 130 && (player2.score - newlyCaptured) < 130) player2.powerUps++;
//                    if (player2.score >= 160 && ((player2.score - newlyCaptured) / 30 < player2.score / 30)) player2.powerUps++;
//                    cout << "Multiplayer P2: Power-Ups: " << player2.powerUps << endl;
//                }
//            }
//        }
//
//        if (player1.frozen && player1.frozenClock.getElapsedTime().asSeconds() >= 3) {
//            player1.frozen = false;
//        }
//        if (player2.frozen && player2.frozenClock.getElapsedTime().asSeconds() >= 3) {
//            player2.frozen = false;
//        }
//
//        if (!player1.alive && !player2.alive) {
//            Game = false;
//            string winner;
//            if (player1.score > player2.score)
//                winner = "Player 1 Wins!";
//            else if (player2.score > player1.score)
//                winner = "Player 2 Wins!";
//            else
//                winner = "It's a Tie!";
//            winnerText.setString(winner);
//        }
//
//        window.clear();
//        inventory.drawBackground(window);
//
//        for (int i = 0; i < M; i++)
//            for (int j = 0; j < N; j++) {
//                if (grid[i][j] == 0) continue;
//                sTile.setColor(Color::White);
//                if (grid[i][j] == 1) {
//                    sTile.setTextureRect(IntRect(0, 0, ts, ts));
//                    if (tileOwner[i][j] == 1)
//                        sTile.setColor(Color(135, 206, 250, 200));
//                    else if (tileOwner[i][j] == 2)
//                        sTile.setColor(Color(255, 99, 71, 200));
//                }
//                if (grid[i][j] == 2) {
//                    sTile.setTextureRect(IntRect(54, 0, ts, ts));
//                    sTile.setColor(player1.pathColor);
//                }
//                if (grid[i][j] == 3) {
//                    sTile.setTextureRect(IntRect(54, 0, ts, ts));
//                    sTile.setColor(player2.pathColor);
//                }
//                sTile.setPosition(j * ts + offsetX, i * ts + offsetY);
//                window.draw(sTile);
//            }
//
//        if (player1.alive) {
//            sTile.setColor(Color::White);
//            sTile.setTextureRect(IntRect(36, 0, ts, ts));
//            sTile.setPosition(static_cast<int>(player1.x) * ts + offsetX, static_cast<int>(player1.y) * ts + offsetY);
//            window.draw(sTile);
//        }
//
//        if (player2.alive) {
//            sTile.setColor(Color::White);
//            sTile.setTextureRect(IntRect(72, 0, ts, ts));
//            sTile.setPosition(static_cast<int>(player2.x) * ts + offsetX, static_cast<int>(player2.y) * ts + offsetY);
//            window.draw(sTile);
//        }
//
//        sEnemy.rotate(10);
//        for (int i = 0; i < enemyCount; i++) {
//            if (enemies[i].frozen) sEnemy.setColor(Color::Cyan);
//            else sEnemy.setColor(Color::White);
//            sEnemy.setPosition(enemies[i].x + offsetX, enemies[i].y + offsetY);
//            window.draw(sEnemy);
//        }
//
//        scoreTextP1.setString("P1 Score: " + to_string(player1.score));
//        powerUpTextP1.setString("P1 Power-Ups: " + to_string(player1.powerUps));
//        scoreTextP2.setString("P2 Score: " + to_string(player2.score));
//        powerUpTextP2.setString("P2 Power-Ups: " + to_string(player2.powerUps));
//
//        window.draw(scoreTextP1);
//        window.draw(powerUpTextP1);
//        window.draw(scoreTextP2);
//        window.draw(powerUpTextP2);
//
//        if (player1.showBackgroundMenu) {
//            for (const auto& option : backgroundOptions) {
//                window.draw(option);
//            }
//        }
//
//        if (!Game) {
//            window.draw(sGameover);
//            window.draw(winnerText);
//        }
//
//        window.display();
//    }
//}

#include "game.h"
#include <SFML/Audio.hpp>
#include <time.h>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

using namespace std;
using namespace sf;

// Helper function to map background names to colors
Color getColorForBackground(const string& name) {
    string lowerName = name;
    transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    if (lowerName == "blue") return Color(135, 206, 250); // Light blue for readability
    if (lowerName == "white") return Color::White;
    if (lowerName == "red") return Color::Red;
    return Color::White; // Default for unknown names
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
    // Random position within playable area (not on borders)
    int maxX = N - 2, maxY = M - 2;
    do {
        x = (rand() % maxX + 1) * tileSize + tileSize / 2.0f;
        y = (rand() % maxY + 1) * tileSize + tileSize / 2.0f;
    } while (grid[static_cast<int>(y / tileSize)][static_cast<int>(x / tileSize)] != 0);

    // Random initial direction with normalized speed
    float angle = static_cast<float>(rand()) / RAND_MAX * 2 * 3.14159f;
    dx = cos(angle);
    dy = sin(angle);
    frozen = false;
    speed = 90.0f; // Default speed (pixels/second), adjustable by level
}

void Enemy::move(int grid[M][N], float deltaTime, int tileSize) {
    if (frozen && frozenClock.getElapsedTime().asSeconds() < 3) return;
    frozen = false;

    // Move smoothly based on speed and deltaTime
    float moveX = dx * speed * deltaTime;
    float moveY = dy * speed * deltaTime;
    float newX = x + moveX;
    float newY = y + moveY;

    // Check boundaries and grid collision
    int gridX = static_cast<int>(newX / tileSize);
    int gridY = static_cast<int>(newY / tileSize);
    bool collideHorizontal = false, collideVertical = false;
    static int lastGridX = -1, lastGridY = -1, collisionCount = 0;

    // Check grid boundaries
    if (gridX < 0 || gridX >= N) {
        collideHorizontal = true;
        // Nudge position further to boundary
        if (gridX < 0) newX = tileSize / 2.0f + 2.0f;
        else newX = (N - 0.5f) * tileSize - 2.0f;
    }
    if (gridY < 0 || gridY >= M) {
        collideVertical = true;
        // Nudge position further to boundary
        if (gridY < 0) newY = tileSize / 2.0f + 2.0f;
        else newY = (M - 0.5f) * tileSize - 2.0f;
    }

    // Check grid tiles
    if (!collideHorizontal && !collideVertical && gridY >= 0 && gridY < M && gridX >= 0 && gridX < N) {
        if (grid[gridY][gridX] == 1) {
            // Determine collision side by checking position relative to tile center
            float tileCenterX = gridX * tileSize + tileSize / 2.0f;
            float tileCenterY = gridY * tileSize + tileSize / 2.0f;
            float dxToTile = newX - tileCenterX;
            float dyToTile = newY - tileCenterY;

            // Approximate collision based on dominant direction
            if (abs(dxToTile) > abs(dyToTile)) {
                collideHorizontal = true;
                // Nudge position further from tile edge
                if (dxToTile > 0) newX = tileCenterX + tileSize / 2.0f + 2.0f; // Right edge
                else newX = tileCenterX - tileSize / 2.0f - 2.0f; // Left edge
            }
            else {
                collideVertical = true;
                // Nudge position further from tile edge
                if (dyToTile > 0) newY = tileCenterY + tileSize / 2.0f + 2.0f; // Bottom edge
                else newY = tileCenterY - tileSize / 2.0f - 2.0f; // Top edge
            }
        }
    }

    if (collideHorizontal || collideVertical) {
        // Track consecutive collisions
        if (gridX == lastGridX && gridY == lastGridY) {
            collisionCount++;
        }
        else {
            collisionCount = 1;
            lastGridX = gridX;
            lastGridY = gridY;
        }

        // Reposition if stuck (3 consecutive collisions)
        if (collisionCount >= 3) {
            // Find a nearby empty tile
            for (int dy = -1; dy <= 1 && collisionCount >= 3; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int testX = gridX + dx;
                    int testY = gridY + dy;
                    if (testX >= 0 && testX < N && testY >= 0 && testY < M && grid[testY][testX] == 0) {
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

        // Bounce like a ball
        if (collideHorizontal) dx = -dx;
        if (collideVertical) dy = -dy;

        // Choose a new random angle, avoiding near-horizontal/vertical
        float angle;
        do {
            angle = static_cast<float>(rand()) / RAND_MAX * 2 * 3.14159f;
        } while (
            abs(fmod(angle, 3.14159f / 2.0f)) < 0.2f || // Avoid 0°, 90°, 180°, 270° ±0.2 radians
            abs(fmod(angle, 3.14159f / 2.0f) - 3.14159f / 2.0f) < 0.2f
            );
        dx = cos(angle);
        dy = sin(angle);

        // Normalize direction
        float len = sqrt(dx * dx + dy * dy);
        dx /= len;
        dy /= len;

        // Update position with new direction
        newX = x + dx * speed * deltaTime;
        newY = y + dy * speed * deltaTime;
    }
    else {
        // Reset collision counter if no collision
        collisionCount = 0;
        lastGridX = -1;
        lastGridY = -1;
    }

    // Update position
    x = newX;
    y = newY;

    // Keep within bounds with direction adjustment
    if (x < tileSize / 2.0f) { x = tileSize / 2.0f + 2.0f; dx = abs(dx); }
    if (x > (N - 0.5f) * tileSize) { x = (N - 0.5f) * tileSize - 2.0f; dx = -abs(dx); }
    if (y < tileSize / 2.0f) { y = tileSize / 2.0f + 2.0f; dy = abs(dy); }
    if (y > (M - 0.5f) * tileSize) { y = (M - 0.5f) * tileSize - 2.0f; dy = -abs(dy); }
}

void Enemy::activateFreeze() {
    frozen = true;
    frozenClock.restart();
}

// Drop implementation (flood-fill to mark unreachable areas)
void drop(int y, int x, int grid[M][N]) {
    if (y < 0 || y >= M || x < 0 || x >= N) return;
    if (grid[y][x] == 0) grid[y][x] = -1;
    if (y > 0 && grid[y - 1][x] == 0) drop(y - 1, x, grid);
    if (y < M - 1 && grid[y + 1][x] == 0) drop(y + 1, x, grid);
    if (x > 0 && grid[y][x - 1] == 0) drop(y, x - 1, grid);
    if (x < N - 1 && grid[y][x + 1] == 0) drop(y, x + 1, grid);
}

// Level selection struct
struct Level {
    string name;
    int enemyCount;
    float enemySpeed;
};

// Single-player level selection
int selectLevel(RenderWindow& window, Font& font, Inventory& inventory) {
    window.setTitle("Select Level");

    // Level definitions (scaled for ts=18)
    vector<Level> levels = {
        {"Easy", 2, 45.0f},
        {"Medium", 4, 90.0f},
        {"Hard", 6, 135.0f}
    };

    // UI elements
    Text title("Select Level", font, 36);
    title.setFillColor(Color::White);
    title.setStyle(Text::Bold);
    title.setPosition(330, 40);
    title.setOutlineColor(Color::Black);
    title.setOutlineThickness(1);

    vector<RectangleShape> levelButtonBgs;
    vector<Text> levelButtons;
    for (size_t i = 0; i < levels.size(); ++i) {
        RectangleShape bg = createButton(320, 150 + i * 60, 160, 40, Color(50, 150, 50));
        Text button(levels[i].name, font, 22);
        button.setFillColor(Color::White);
        button.setPosition(350, 155 + i * 60);
        levelButtonBgs.push_back(bg);
        levelButtons.push_back(button);
    }

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
                return -1; // Indicate closure
            }

            if (event.type == Event::MouseButtonPressed) {
                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
                for (size_t i = 0; i < levelButtons.size(); ++i) {
                    if (levelButtons[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        return static_cast<int>(i); // Return level index
                    }
                }
            }
        }

        // Hover effects
        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
        for (size_t i = 0; i < levelButtonBgs.size(); ++i) {
            levelButtonBgs[i].setFillColor(levelButtons[i].getGlobalBounds().contains(mousePos.x, mousePos.y) ? Color(70, 180, 70) : Color(50, 150, 50));
        }

        window.clear();
        inventory.drawBackground(window);
        window.draw(title);
        for (size_t i = 0; i < levelButtons.size(); ++i) {
            window.draw(levelButtonBgs[i]);
            window.draw(levelButtons[i]);
        }
        window.display();
    }
    return -1; // Fallback
}

// Single-player game
void playXonixGame(PlayerList& pl, RenderWindow& window, Font& font, const string& currentUser, Inventory& inventory) {
    window.setTitle("Xonix Game");
    const int windowWidth = 800;
    const int windowHeight = 600;
    const int ts = 18; // Tile size
    const float offsetX = (windowWidth - N * ts) / 2.0f; // 40
    const float offsetY = (windowHeight - M * ts) / 2.0f; // 120
    window.setSize(Vector2u(windowWidth, windowHeight));
    srand(time(0));

    // Level selection
    int levelIndex = selectLevel(window, font, inventory);
    if (levelIndex == -1) return; // Window closed

    vector<Level> levels = {
        {"Easy", 2, 45.0f},
        {"Medium", 4, 90.0f},
        {"Hard", 6, 135.0f}
    };
    int enemyCount = levels[levelIndex].enemyCount;
    float enemySpeed = levels[levelIndex].enemySpeed;

    int grid[M][N] = { 0 };

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
    powerUpText.setFillColor(Color::Green);
    powerUpText.setPosition(20.0f + offsetX, 40.0f + offsetY);

    // Background selection UI
    vector<Text> backgroundOptions;
    string namesString = inventory.getBackgroundNames();
    vector<string> bgNames;
    stringstream ss(namesString);
    string name;
    while (getline(ss, name)) {
        if (!name.empty()) {
            bgNames.push_back(name);
        }
    }
    for (size_t i = 0; i < bgNames.size(); ++i) {
        Text option;
        option.setFont(font);
        option.setString(bgNames[i]);
        option.setCharacterSize(18);
        option.setPosition(20.0f + offsetX, 70.0f + offsetY + i * 25.0f);
        option.setFillColor(i + 1 == inventory.getCurrentBackgroundID() ? getColorForBackground(bgNames[i]) : Color::White);
        backgroundOptions.push_back(option);
    }

    Enemy enemies[10];
    for (int i = 0; i < enemyCount; ++i) {
        enemies[i] = Enemy(grid, ts);
        enemies[i].speed = enemySpeed;
    }

    bool Game = true;
    PlayerState player;
    player.x = 10;
    player.y = 0; // Start on border
    float timer = 0, delay = 0.07;
    Clock clock;

    int bonusCount = 0, bonusThreshold = 10, bonusMultiplier = 2;

    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
                grid[i][j] = 1;

    // Start background music
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
                Player* p = pl.getPlayerByUsername(currentUser);
                if (p) {
                    pl.savePlayerStats(currentUser, player.score, player.powerUps, p->preferredThemeID, p->preferredSoundID);
                }
                else {
                    cerr << "Error: Player " << currentUser << " not found for saving stats.\n";
                }
                window.close();
                return;
            }

            if (e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::Escape) {
                    if (!Game) {
                        music.stop();
                        Player* p = pl.getPlayerByUsername(currentUser);
                        if (p) {
                            pl.savePlayerStats(currentUser, player.score, player.powerUps, p->preferredThemeID, p->preferredSoundID);
                        }
                        else {
                            cerr << "Error: Player " << currentUser << " not found for saving stats.\n";
                        }
                        return;
                    }
                    else {
                        for (int i = 1; i < M - 1; i++)
                            for (int j = 1; j < N - 1; j++)
                                grid[i][j] = 0;
                        player = PlayerState();
                        player.x = 10;
                        player.y = 0; // Reset to border
                        Game = true;
                        bonusCount = 0;
                        bonusThreshold = 10;
                        bonusMultiplier = 2;
                        for (int i = 0; i < enemyCount; i++) {
                            enemies[i] = Enemy(grid, ts);
                            enemies[i].speed = enemySpeed;
                        }
                        music.stop();
                        music.play(); // Restart music after reset
                    }
                }
                if (e.key.code == Keyboard::Space && player.powerUps > 0 && Game) {
                    for (int i = 0; i < enemyCount; i++)
                        enemies[i].activateFreeze();
                    player.powerUps--;
                }
                if (e.key.code == Keyboard::B) {
                    player.showBackgroundMenu = !player.showBackgroundMenu;
                }
            }

            if (e.type == Event::MouseButtonPressed && player.showBackgroundMenu) {
                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
                for (size_t i = 0; i < backgroundOptions.size(); ++i) {
                    if (backgroundOptions[i].getGlobalBounds().contains(mousePos)) {
                        inventory.setBackground(i + 1);
                        player.showBackgroundMenu = false;
                        for (size_t j = 0; j < backgroundOptions.size(); ++j) {
                            backgroundOptions[j].setFillColor(j + 1 == inventory.getCurrentBackgroundID() ? getColorForBackground(bgNames[j]) : Color::White);
                        }
                        break;
                    }
                }
            }
        }

        if (Game && !player.frozen) {
            if (Keyboard::isKeyPressed(Keyboard::Left)) { player.dx = -1; player.dy = 0; }
            if (Keyboard::isKeyPressed(Keyboard::Right)) { player.dx = 1; player.dy = 0; }
            if (Keyboard::isKeyPressed(Keyboard::Up)) { player.dx = 0; player.dy = -1; }
            if (Keyboard::isKeyPressed(Keyboard::Down)) { player.dx = 0; player.dy = 1; }

            if (timer > delay) {
                player.x += player.dx;
                player.y += player.dy;

                if (player.x < 0) player.x = 0; if (player.x > N - 1) player.x = N - 1;
                if (player.y < 0) player.y = 0; if (player.y > M - 1) player.y = M - 1;

                player.constructing = (grid[static_cast<int>(player.y)][static_cast<int>(player.x)] == 0);
                if (grid[static_cast<int>(player.y)][static_cast<int>(player.x)] == 2) Game = false;
                if (grid[static_cast<int>(player.y)][static_cast<int>(player.x)] == 0) {
                    grid[static_cast<int>(player.y)][static_cast<int>(player.x)] = 2;
                    player.capturedTiles++;
                }
                timer = 0;
            }

            for (int i = 0; i < enemyCount; i++) enemies[i].move(grid, time, ts);

            if (grid[static_cast<int>(player.y)][static_cast<int>(player.x)] == 1) {
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
                        if (grid[i][j] == -1) grid[i][j] = 0;
                        else if (grid[i][j] == 2) { grid[i][j] = 1; newlyCaptured++; }
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
                int enemyY = static_cast<int>(enemies[i].y / ts);
                int enemyX = static_cast<int>(enemies[i].x / ts);
                if (enemyY >= 0 && enemyY < M && enemyX >= 0 && enemyX < N) {
                    if (grid[enemyY][enemyX] == 2) Game = false;
                }
            }
        }

        if (player.frozen && player.frozenClock.getElapsedTime().asSeconds() >= 3) {
            player.frozen = false;
        }

        window.clear();
        inventory.drawBackground(window);

        for (int i = 0; i < M; i++)
            for (int j = 0; j < N; j++) {
                if (grid[i][j] == 0) continue;
                if (grid[i][j] == 1) sTile.setTextureRect(IntRect(0, 0, ts, ts));
                if (grid[i][j] == 2) sTile.setTextureRect(IntRect(54, 0, ts, ts));
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
            for (const auto& option : backgroundOptions) {
                window.draw(option);
            }
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

    const int ts = 18; // Tile size
    const float offsetX = (800 - N * ts) / 2.0f; // 40
    const float offsetY = (600 - M * ts) / 2.0f; // 120

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

    vector<Text> backgroundOptions;
    string namesString = inventory.getBackgroundNames();
    vector<string> bgNames;
    stringstream ss(namesString);
    string name;
    while (getline(ss, name)) {
        if (!name.empty()) {
            bgNames.push_back(name);
        }
    }
    for (size_t i = 0; i < bgNames.size(); ++i) {
        Text option;
        option.setFont(font);
        option.setString(bgNames[i]);
        option.setCharacterSize(18);
        option.setPosition(20.0f + offsetX, 70.0f + offsetY + i * 25.0f);
        option.setFillColor(i + 1 == inventory.getCurrentBackgroundID() ? getColorForBackground(bgNames[i]) : Color::White);
        backgroundOptions.push_back(option);
    }

    int enemyCount = 3 + (rand() % 3); // Random 3 to 5 enemies
    Enemy enemies[10];
    for (int i = 0; i < enemyCount; ++i) {
        enemies[i] = Enemy(grid, ts);
    }

    bool Game = true;
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

    // Start background music
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
                if (p1) {
                    pl.savePlayerStats(player1User, player1.score, player1.powerUps, p1->preferredThemeID, p1->preferredSoundID);
                }
                else {
                    cerr << "Error: Player " << player1User << " not found for saving stats.\n";
                }
                Player* p2 = pl.getPlayerByUsername(player2User);
                if (p2) {
                    pl.savePlayerStats(player2User, player2.score, player2.powerUps, p2->preferredThemeID, p2->preferredSoundID);
                }
                else {
                    cerr << "Error: Player " << player2User << " not found for saving stats.\n";
                }
                window.close();
                return;
            }

            if (e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::Escape) {
                    if (!Game || (!player1.alive && !player2.alive)) {
                        music.stop();
                        Player* p1 = pl.getPlayerByUsername(player1User);
                        if (p1) {
                            pl.savePlayerStats(player1User, player1.score, player1.powerUps, p1->preferredThemeID, p1->preferredSoundID);
                        }
                        else {
                            cerr << "Error: Player " << player1User << " not found for saving stats.\n";
                        }
                        Player* p2 = pl.getPlayerByUsername(player2User);
                        if (p2) {
                            pl.savePlayerStats(player2User, player2.score, player2.powerUps, p2->preferredThemeID, p2->preferredSoundID);
                        }
                        else {
                            cerr << "Error: Player " << player2User << " not found for saving stats.\n";
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
                        bonusCountP1 = bonusCountP2 = 0;
                        bonusThresholdP1 = bonusThresholdP2 = 10;
                        bonusMultiplierP1 = bonusMultiplierP2 = 2;
                        enemyCount = 3 + (rand() % 3); // Random 3 to 5 enemies on reset
                        for (int i = 0; i < enemyCount; i++)
                            enemies[i] = Enemy(grid, ts);
                        music.stop();
                        music.play(); // Restart music after reset
                    }
                }
                if (e.key.code == Keyboard::Space && player1.powerUps > 0 && Game && player1.alive) {
                    for (int i = 0; i < enemyCount; i++)
                        enemies[i].activateFreeze();
                    player2.frozen = true;
                    player2.frozenClock.restart();
                    player1.powerUps--;
                }
                if (e.key.code == Keyboard::Return && player2.powerUps > 0 && Game && player2.alive) {
                    for (int i = 0; i < enemyCount; i++)
                        enemies[i].activateFreeze();
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
                for (size_t i = 0; i < backgroundOptions.size(); ++i) {
                    if (backgroundOptions[i].getGlobalBounds().contains(mousePos)) {
                        inventory.setBackground(i + 1);
                        player1.showBackgroundMenu = false;
                        player2.showBackgroundMenu = false;
                        for (size_t j = 0; j < backgroundOptions.size(); ++j) {
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
                    if (player1.y < 0) player1.y = 0; if (player1.y > M - 1)  player1.y = M - 1;

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
                int enemyY = static_cast<int>(enemies[i].y / ts);
                int enemyX = static_cast<int>(enemies[i].x / ts);
                if (enemyY >= 0 && enemyY < M && enemyX >= 0 && enemyX < N) {
                    int enemyGridVal = grid[enemyY][enemyX];
                    if (enemyGridVal == 2 && player1.alive && player1.constructing) player1.alive = false;
                    if (enemyGridVal == 3 && player2.alive && player2.constructing) player2.alive = false;
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

        if (player1.frozen && player1.frozenClock.getElapsedTime().asSeconds() >= 3) {
            player1.frozen = false;
        }
        if (player2.frozen && player2.frozenClock.getElapsedTime().asSeconds() >= 3) {
            player2.frozen = false;
        }

        if (!player1.alive && !player2.alive) {
            Game = false;
            string winner;
            if (player1.score > player2.score)
                winner = "Player 1 Wins!";
            else if (player2.score > player2.score)
                winner = "Player 2 Wins!";
            else
                winner = "It's a Tie!";
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
            for (const auto& option : backgroundOptions) {
                window.draw(option);
            }
        }

        if (!Game) {
            window.draw(sGameover);
            window.draw(winnerText);
        }

        window.display();
    }
}