#include "game.h"
#include <SFML/Audio.hpp>
#include <time.h>
#include <string>
#include <iostream>

using namespace std;
using namespace sf;


// Enemy implementation
Enemy::Enemy() {
    x = y = 300; // Start near center
    dx = rand() % 3 - 1; // {-1, 0, 1}
    dy = rand() % 3 - 1;
    frozen = false;
    moveTimer = 0;
    // Ensure non-zero movement
    if (dx == 0 && dy == 0) {
        dx = rand() % 2 ? 1 : -1;
    }
}

void Enemy::move(int grid[M][N], float deltaTime) {
    if (frozen && freezeClock.getElapsedTime().asSeconds() < 3) return;
    frozen = false;

    moveTimer += deltaTime;
    if (moveTimer < 0.1f) return; // Move every 0.1 seconds
    moveTimer = 0;

    int newX = x + dx;
    int newY = y + dy;

    // Check boundaries and grid collision
    if (newX / ts >= 0 && newX / ts < N && newY / ts >= 0 && newY / ts < M) {
        if (grid[newY / ts][newX / ts] == 1) {
            dx = -dx;
            dy = -dy;
        }
        else {
            x = newX;
            y = newY;
        }
    }
    else {
        dx = -dx;
        dy = -dy;
    }

    // Ensure non-zero movement
    if (dx == 0 && dy == 0) {
        dx = rand() % 2 ? 1 : -1;
    }
}

void Enemy::activateFreeze() {
    frozen = true;
    freezeClock.restart();
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

// Single-player game
void playXonixGame(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const string& currentUser) {
    window.setTitle("Xonix Game");

    srand(time(0));

    int grid[M][N] = { 0 };

    Texture t1, t2, t3;
    if (!t1.loadFromFile("images/tiles.png") || !t2.loadFromFile("images/gameover.png") || !t3.loadFromFile("images/enemy.png")) {
        cout << "Error: Could not load game textures.\n";
        return;
    }

    Sprite sTile(t1), sGameover(t2), sEnemy(t3);
    sGameover.setPosition(200.0f, 150.0f);
    sEnemy.setOrigin(20, 20);

    Text scoreText("", font, 20);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(20.0f, 15.0f);

    Text powerUpText("", font, 20);
    powerUpText.setFillColor(Color::Green);
    powerUpText.setPosition(20.0f, 40.0f);

    int enemyCount = 4;
    Enemy enemies[10];

    bool Game = true;
    PlayerState player;
    player.x = 10;
    float timer = 0, delay = 0.07;
    Clock clock;

    int bonusCount = 0, bonusThreshold = 10, bonusMultiplier = 2;

    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
                grid[i][j] = 1;

    while (window.isOpen()) {
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;

        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) {
                pl.savePlayerStats(currentUser, player.score, player.powerUps);
                window.close();
                return;
            }

            if (e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::Escape) {
                    if (!Game) {
                        pl.savePlayerStats(currentUser, player.score, player.powerUps);
                        return;
                    }
                    else {
                        for (int i = 1; i < M - 1; i++)
                            for (int j = 1; j < N - 1; j++)
                                grid[i][j] = 0;
                        player = PlayerState();
                        player.x = 10;
                        Game = true;
                        bonusCount = 0;
                        bonusThreshold = 10;
                        bonusMultiplier = 2;
                        for (int i = 0; i < enemyCount; i++)
                            enemies[i] = Enemy();
                    }
                }
                if (e.key.code == Keyboard::Space && player.powerUps > 0 && Game) {
                    for (int i = 0; i < enemyCount; i++)
                        enemies[i].activateFreeze();
                    player.powerUps--;
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

                player.constructing = (grid[player.y][player.x] == 0);
                if (grid[player.y][player.x] == 2) Game = false;
                if (grid[player.y][player.x] == 0) {
                    grid[player.y][player.x] = 2;
                    player.capturedTiles++;
                }
                timer = 0;
            }

            for (int i = 0; i < enemyCount; i++) enemies[i].move(grid, time);

            if (grid[player.y][player.x] == 1) {
                player.dx = player.dy = 0;

                for (int i = 0; i < enemyCount; i++)
                    drop(enemies[i].y / ts, enemies[i].x / ts, grid);

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

                    if (player.score >= 50 && (player.score - newlyCaptured) < 50) player.powerUps++;
                    if (player.score >= 70 && (player.score - newlyCaptured) < 70) player.powerUps++;
                    if (player.score >= 100 && (player.score - newlyCaptured) < 100) player.powerUps++;
                    if (player.score >= 130 && (player.score - newlyCaptured) < 130) player.powerUps++;
                    if (player.score >= 160 && ((player.score - newlyCaptured) / 30 < (player.score / 30))) player.powerUps++;
                }
            }

            for (int i = 0; i < enemyCount; i++)
                if (grid[enemies[i].y / ts][enemies[i].x / ts] == 2) Game = false;
        }

        if (player.frozen && player.freezeClock.getElapsedTime().asSeconds() >= 3) {
            player.frozen = false;
        }

        window.clear();

        for (int i = 0; i < M; i++)
            for (int j = 0; j < N; j++) {
                if (grid[i][j] == 0) continue;
                if (grid[i][j] == 1) sTile.setTextureRect(IntRect(0, 0, ts, ts));
                if (grid[i][j] == 2) sTile.setTextureRect(IntRect(54, 0, ts, ts));
                sTile.setPosition(j * ts, i * ts);
                window.draw(sTile);
            }

        sTile.setTextureRect(IntRect(36, 0, ts, ts));
        sTile.setPosition(player.x * ts, player.y * ts);
        window.draw(sTile);

        sEnemy.rotate(10);
        for (int i = 0; i < enemyCount; i++) {
            if (enemies[i].frozen) sEnemy.setColor(Color::Cyan);
            else sEnemy.setColor(Color::White);
            sEnemy.setPosition(enemies[i].x, enemies[i].y);
            window.draw(sEnemy);
        }

        scoreText.setString("Score: " + to_string(player.score));
        powerUpText.setString("Power-Ups: " + to_string(player.powerUps));
        window.draw(scoreText);
        window.draw(powerUpText);

        if (!Game) window.draw(sGameover);

        window.display();
    }
}



//Multiplayer mode
void playMultiplayerXonixGame(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const string& player1User, const string& player2User) {
    window.setTitle("Xonix Multiplayer Game");

    srand(time(0));

    int grid[M][N] = { 0 };
    int tileOwner[M][N] = { 0 }; // Tracks tile ownership (0: none, 1: Player 1, 2: Player 2)

    // Load textures
    Texture t1, t2, t3;
    if (!t1.loadFromFile("images/tiles.png") || !t2.loadFromFile("images/gameover.png") || !t3.loadFromFile("images/enemy.png")) {
        std::cout << "Error: Could not load game textures.\n";
        return;
    }

    Sprite sTile(t1), sGameover(t2), sEnemy(t3);
    sGameover.setPosition(200.0f, 150.0f);
    sEnemy.setOrigin(20, 20);

    // Player 1 UI (left side)
    Text scoreTextP1("", font, 20);
    scoreTextP1.setFillColor(sf::Color::White);
    scoreTextP1.setPosition(20.0f, 15.0f);

    Text powerUpTextP1("", font, 20);
    powerUpTextP1.setFillColor(sf::Color::Green);
    powerUpTextP1.setPosition(20.0f, 40.0f);

    // Player 2 UI (right side)
    Text scoreTextP2("", font, 20);
    scoreTextP2.setFillColor(sf::Color::White);
    scoreTextP2.setPosition(static_cast<float>(N * ts - 190), 15.0f);

    Text powerUpTextP2("", font, 20);
    powerUpTextP2.setFillColor(sf::Color::Green);
    powerUpTextP2.setPosition(static_cast<float>(N * ts - 190), 40.0f);

    // Winner text
    Text winnerText("", font, 30);
    winnerText.setFillColor(sf::Color::Yellow);
    winnerText.setPosition(350.0f, 150.0f);

    int enemyCount = 4;
    Enemy enemies[10];

    bool Game = true;
    PlayerState player1, player2;
    player1.x = 10; // Start Player 1 at left edge
    player1.y = 0;
    player1.pathColor = sf::Color::Blue; // Player 1 path color
    player2.x = N - 10; // Start Player 2 at right edge
    player2.y = 0;
    player2.pathColor = sf::Color::Red; // Player 2 path color

    float timer = 0, delay = 0.07;
    sf::Clock clock;

    int bonusCountP1 = 0, bonusThresholdP1 = 10, bonusMultiplierP1 = 2;
    int bonusCountP2 = 0, bonusThresholdP2 = 10, bonusMultiplierP2 = 2;

    // Initialize grid borders
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            if (i == 0 || j == 0 || i == M - 1 || j == N - 1) {
                grid[i][j] = 1;
                tileOwner[i][j] = 0; // Borders have no owner
            }

    while (window.isOpen()) {
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;

        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                pl.savePlayerStats(player1User, player1.score, player1.powerUps);
                pl.savePlayerStats(player2User, player2.score, player2.powerUps);
                window.close();
                return;
            }

            if (e.type == sf::Event::KeyPressed) {
                if (e.key.code == sf::Keyboard::Escape) {
                    if (!Game || (!player1.alive && !player2.alive)) {
                        pl.savePlayerStats(player1User, player1.score, player1.powerUps);
                        pl.savePlayerStats(player2User, player2.score, player2.powerUps);
                        return;
                    }
                    else {
                        // Reset game
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
                        player1.pathColor = sf::Color::Blue;
                        player2.x = N - 10;
                        player2.y = 0;
                        player2.pathColor = sf::Color::Red;
                        Game = true;
                        bonusCountP1 = bonusCountP2 = 0;
                        bonusThresholdP1 = bonusThresholdP2 = 10;
                        bonusMultiplierP1 = bonusMultiplierP2 = 2;
                        for (int i = 0; i < enemyCount; i++)
                            enemies[i] = Enemy();
                    }
                }
                if (e.key.code == sf::Keyboard::Space && player1.powerUps > 0 && Game && player1.alive) {
                    for (int i = 0; i < enemyCount; i++)
                        enemies[i].activateFreeze();
                    player2.frozen = true;
                    player2.freezeClock.restart();
                    player1.powerUps--;
                }
                if (e.key.code == sf::Keyboard::Return && player2.powerUps > 0 && Game && player2.alive) {
                    for (int i = 0; i < enemyCount; i++)
                        enemies[i].activateFreeze();
                    player1.frozen = true;
                    player1.freezeClock.restart();
                    player2.powerUps--;
                }
            }
        }

        if (Game) {
            // Player 1 controls (WASD)
            if (!player1.frozen && player1.alive) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { player1.dx = -1; player1.dy = 0; }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { player1.dx = 1; player1.dy = 0; }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { player1.dx = 0; player1.dy = -1; }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { player1.dx = 0; player1.dy = 1; }
            }

            // Player 2 controls (arrows)
            if (!player2.frozen && player2.alive) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) { player2.dx = -1; player2.dy = 0; }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) { player2.dx = 1; player2.dy = 0; }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) { player2.dx = 0; player2.dy = -1; }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) { player2.dx = 0; player2.dy = 1; }
            }

            if (timer > delay) {
                // Move Player 1
                if (player1.alive && !player1.frozen) {
                    player1.x += player1.dx;
                    player1.y += player1.dy;

                    if (player1.x < 0) player1.x = 0; if (player1.x > N - 1) player1.x = N - 1;
                    if (player1.y < 0) player1.y = 0; if (player1.y > M - 1) player1.y = M - 1;

                    player1.constructing = (grid[player1.y][player1.x] == 0);
                    // Die if hitting own path, other player's path, or other player's captured tiles
                    if (grid[player1.y][player1.x] == 2 && tileOwner[player1.y][player1.x] == 1) {
                        player1.alive = false;
                    }
                    else if (grid[player1.y][player1.x] == 3 ||
                        (grid[player1.y][player1.x] == 1 && tileOwner[player1.y][player1.x] == 2)) {
                        player1.alive = false;
                    }
                    else if (grid[player1.y][player1.x] == 0) {
                        grid[player1.y][player1.x] = 2; // Player 1's path
                        tileOwner[player1.y][player1.x] = 1; // Track path ownership
                        player1.capturedTiles++;
                    }
                }

                // Move Player 2
                if (player2.alive && !player2.frozen) {
                    player2.x += player2.dx;
                    player2.y += player2.dy;

                    if (player2.x < 0) player2.x = 0; if (player2.x > N - 1) player2.x = N - 1;
                    if (player2.y < 0) player2.y = 0; if (player2.y > M - 1) player2.y = M - 1;

                    player2.constructing = (grid[player2.y][player2.x] == 0);
                    // Die if hitting own path, other player's path, or other player's captured tiles
                    if (grid[player2.y][player2.x] == 3 && tileOwner[player2.y][player2.x] == 2) {
                        player2.alive = false;
                    }
                    else if (grid[player2.y][player2.x] == 2 ||
                        (grid[player2.y][player2.x] == 1 && tileOwner[player2.y][player2.x] == 1)) {
                        player2.alive = false;
                    }
                    else if (grid[player2.y][player2.x] == 0) {
                        grid[player2.y][player2.x] = 3; // Player 2's path
                        tileOwner[player2.y][player2.x] = 2; // Track path ownership
                        player2.capturedTiles++;
                    }
                }

                // Check player collisions (head-on)
                if (player1.alive && player2.alive && player1.x == player2.x && player1.y == player2.y) {
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
                }

                timer = 0;
            }

            // Move enemies
            for (int i = 0; i < enemyCount; i++) {
                enemies[i].move(grid, time);
                if (enemies[i].y / ts < M && enemies[i].x / ts < N) {
                    int enemyGridVal = grid[enemies[i].y / ts][enemies[i].x / ts];
                    if (enemyGridVal == 2 && player1.alive && player1.constructing) player1.alive = false;
                    if (enemyGridVal == 3 && player2.alive && player2.constructing) player2.alive = false;
                }
            }

            // Player 1 captures tiles
            if (player1.alive && grid[player1.y][player1.x] == 1) {
                player1.dx = player1.dy = 0;
                for (int i = 0; i < enemyCount; i++)
                    drop(enemies[i].y / ts, enemies[i].x / ts, grid);

                int newlyCaptured = 0;
                for (int i = 0; i < M; i++)
                    for (int j = 0; j < N; j++) {
                        if (grid[i][j] == -1) {
                            grid[i][j] = 0;
                            tileOwner[i][j] = 0;
                        }
                        else if (grid[i][j] == 2 && tileOwner[i][j] == 1) {
                            grid[i][j] = 1;
                            tileOwner[i][j] = 1; // Player 1 captures own path
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

                    if (player1.score >= 50 && (player1.score - newlyCaptured) < 50) player1.powerUps++;
                    if (player1.score >= 70 && (player1.score - newlyCaptured) < 70) player1.powerUps++;
                    if (player1.score >= 100 && (player1.score - newlyCaptured) < 100) player1.powerUps++;
                    if (player1.score >= 130 && (player1.score - newlyCaptured) < 130) player1.powerUps++;
                    if (player1.score >= 160 && ((player1.score - newlyCaptured) / 30 < (player1.score / 30))) player1.powerUps++;
                }
            }

            // Player 2 captures tiles
            if (player2.alive && grid[player2.y][player2.x] == 1) {
                player2.dx = player2.dy = 0;
                for (int i = 0; i < enemyCount; i++)
                    drop(enemies[i].y / ts, enemies[i].x / ts, grid);

                int newlyCaptured = 0;
                for (int i = 0; i < M; i++)
                    for (int j = 0; j < N; j++) {
                        if (grid[i][j] == -1) {
                            grid[i][j] = 0;
                            tileOwner[i][j] = 0;
                        }
                        else if (grid[i][j] == 3 && tileOwner[i][j] == 2) {
                            grid[i][j] = 1;
                            tileOwner[i][j] = 2; // Player 2 captures own path
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

                    if (player2.score >= 50 && (player2.score - newlyCaptured) < 50) player2.powerUps++;
                    if (player2.score >= 70 && (player2.score - newlyCaptured) < 70) player2.powerUps++;
                    if (player2.score >= 100 && (player2.score - newlyCaptured) < 100) player2.powerUps++;
                    if (player2.score >= 130 && (player2.score - newlyCaptured) < 130) player2.powerUps++;
                    if (player2.score >= 160 && ((player2.score - newlyCaptured) / 30 < (player2.score / 30))) player2.powerUps++;
                }
            }
        }

        // Check frozen status
        if (player1.frozen && player1.freezeClock.getElapsedTime().asSeconds() >= 3) {
            player1.frozen = false;
        }
        if (player2.frozen && player2.freezeClock.getElapsedTime().asSeconds() >= 3) {
            player2.frozen = false;
        }

        // Check game over
        if (!player1.alive && !player2.alive) {
            Game = false;
            std::string winner;
            if (player1.score > player2.score)
                winner = "Player 1 Wins!";
            else if (player2.score > player1.score)
                winner = "Player 2 Wins!";
            else
                winner = "It's a Tie!";
            winnerText.setString(winner);
        }

        // Render
        window.clear();

        // Draw grid
        for (int i = 0; i < M; i++)
            for (int j = 0; j < N; j++) {
                if (grid[i][j] == 0) continue;
                sTile.setColor(sf::Color::White); // Reset color
                if (grid[i][j] == 1) {
                    sTile.setTextureRect(sf::IntRect(0, 0, ts, ts)); // Border
                    // Tint captured tiles based on owner
                    if (tileOwner[i][j] == 1)
                        sTile.setColor(sf::Color(135, 206, 250, 200)); // Light blue for Player 1
                    else if (tileOwner[i][j] == 2)
                        sTile.setColor(sf::Color(255, 99, 71, 200)); // Light red for Player 2
                }
                if (grid[i][j] == 2) {
                    sTile.setTextureRect(sf::IntRect(54, 0, ts, ts)); // Player 1 path
                    sTile.setColor(player1.pathColor);
                }
                if (grid[i][j] == 3) {
                    sTile.setTextureRect(sf::IntRect(54, 0, ts, ts)); // Player 2 path
                    sTile.setColor(player2.pathColor);
                }
                sTile.setPosition(j * ts, i * ts);
                window.draw(sTile);
            }

        // Draw Player 1
        if (player1.alive) {
            sTile.setColor(sf::Color::White); // Reset color
            sTile.setTextureRect(sf::IntRect(36, 0, ts, ts));
            sTile.setPosition(player1.x * ts, player1.y * ts);
            window.draw(sTile);
        }

        // Draw Player 2
        if (player2.alive) {
            sTile.setColor(sf::Color::White); // Reset color
            sTile.setTextureRect(sf::IntRect(72, 0, ts, ts));
            sTile.setPosition(player2.x * ts, player2.y * ts);
            window.draw(sTile);
        }

        // Draw enemies
        sEnemy.rotate(10);
        for (int i = 0; i < enemyCount; i++) {
            if (enemies[i].frozen) sEnemy.setColor(sf::Color::Cyan);
            else sEnemy.setColor(sf::Color::White);
            sEnemy.setPosition(enemies[i].x, enemies[i].y);
            window.draw(sEnemy);
        }

        // Draw UI
        scoreTextP1.setString("P1 Score: " + std::to_string(player1.score));
        powerUpTextP1.setString("P1 Power-Ups: " + std::to_string(player1.powerUps));
        scoreTextP2.setString("P2 Score: " + std::to_string(player2.score));
        powerUpTextP2.setString("P2 Power-Ups: " + std::to_string(player2.powerUps));

        window.draw(scoreTextP1);
        window.draw(powerUpTextP1);
        window.draw(scoreTextP2);
        window.draw(powerUpTextP2);

        if (!Game) {
            window.draw(sGameover);
            window.draw(winnerText);
        }

        window.display();
    }
}