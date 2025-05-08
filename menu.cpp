#include "menu.h"
#include "friend.h"
#include "login.h"
#include "game.h"
#include <iostream>

using namespace std;

bool displayMatchTransitionPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const string& player1, const string& player2, Inventory& inventory) {
    window.setTitle("Match Found");

    sf::Text title("Match Found!", font, 30);
    title.setFillColor(sf::Color::Yellow);
    title.setPosition(300, 50);

    sf::Text player1Text("Player 1: " + player1, font, 20);
    player1Text.setFillColor(sf::Color::Green);
    player1Text.setPosition(200, 150);

    string player1StatsStr = pl.getPlayerStats(player1);
    sf::Text player1Stats("", font, 18);
    player1Stats.setFillColor(sf::Color::White);
    player1Stats.setPosition(200, 180);
    size_t scorePos = player1StatsStr.find("Total Score: ");
    size_t powerUpsPos = player1StatsStr.find("Total Power-Ups: ");
    if (scorePos != string::npos && powerUpsPos != string::npos) {
        string score = player1StatsStr.substr(scorePos + 13, player1StatsStr.find('\n', scorePos) - scorePos - 13);
        string powerUps = player1StatsStr.substr(powerUpsPos + 16);
        player1Stats.setString("Score: " + score + "\nPower-Ups: " + powerUps);
    }
    else {
        player1Stats.setString("Stats unavailable");
    }

    sf::Text player2Text("Player 2: " + player2, font, 20);
    player2Text.setFillColor(sf::Color::Green);
    player2Text.setPosition(200, 250);

    string player2StatsStr = pl.getPlayerStats(player2);
    sf::Text player2Stats("", font, 18);
    player2Stats.setFillColor(sf::Color::White);
    player2Stats.setPosition(200, 280);
    scorePos = player2StatsStr.find("Total Score: ");
    powerUpsPos = player2StatsStr.find("Total Power-Ups: ");
    if (scorePos != string::npos && powerUpsPos != string::npos) {
        string score = player2StatsStr.substr(scorePos + 13, player2StatsStr.find('\n', scorePos) - scorePos - 13);
        string powerUps = player2StatsStr.substr(powerUpsPos + 16);
        player2Stats.setString("Score: " + score + "\nPower-Ups: " + powerUps);
    }
    else {
        player2Stats.setString("Stats unavailable");
    }

    sf::Text startButton("Start Game", font, 20);
    startButton.setFillColor(sf::Color::Green);
    startButton.setPosition(350, 400);

    sf::Clock clock;
    float displayTime = 5.0f;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return false;
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (startButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    return true;
                }
            }
        }

        if (clock.getElapsedTime().asSeconds() >= displayTime) {
            return true;
        }

        window.clear();
        inventory.drawBackground(window); // Draw background
        window.draw(title);
        window.draw(player1Text);
        window.draw(player1Stats);
        window.draw(player2Text);
        window.draw(player2Stats);
        window.draw(startButton);
        window.display();
    }
    return false;
}

bool displayMatchmakingPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const string& currentUser, MatchmakingQueue& queue, Inventory& inventory) {
    window.setTitle("Matchmaking");

    sf::Text title("Waiting for Match...", font, 30);
    title.setPosition(300, 200);

    sf::Text cancelButton("Cancel", font, 20);
    cancelButton.setPosition(350, 400);

    sf::Text statusMessage("", font, 20);
    statusMessage.setFillColor(sf::Color::Red);
    statusMessage.setPosition(200, 350);

    sf::Clock clock;
    float checkInterval = 2.0f;

    // Ensure the current user is in the queue
    if (!queue.isPlayerInQueue(currentUser)) {
        Player* player = pl.getPlayerByUsername(currentUser);
        if (player && !queue.enqueue(player->playerID, pl.getPlayerScore(currentUser), currentUser)) {
            statusMessage.setString("Error: Failed to join queue!");
        }
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return false;
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (cancelButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    return true;
                }
            }
        }

        if (clock.getElapsedTime().asSeconds() >= checkInterval) {
            std::cout << "Checking queue: Size = " << queue.getSize() << std::endl;
            queue.debugPrintQueue();
            if (queue.hasEnoughPlayers()) {
                string player1, player2;
                if (queue.dequeue(player1, player2)) {
                    std::cout << "Match found: " << player1 << " vs " << player2 << std::endl;
                    if (displayMatchTransitionPage(pl, window, font, player1, player2, inventory)) {
                        playMultiplayerXonixGame(pl, window, font, player1, player2);
                    }
                    return true; // Return to menu after match
                }
                else {
                    statusMessage.setString("Error: Failed to find a valid match!");
                    std::cout << "Dequeue failed: Invalid match" << std::endl;
                }
            }
            else {
                statusMessage.setString("Waiting for more players...");
            }
            clock.restart();
        }

        window.clear();
        inventory.drawBackground(window); // Draw background
        window.draw(title);
        window.draw(cancelButton);
        window.draw(statusMessage);
        window.display();
    }
    return false;
}

bool displayManualMatchPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const string& currentUser, Inventory& inventory) {
    window.setTitle("Manual Match");

    sf::Text title("Enter Friend's Username", font, 30);
    title.setPosition(250, 50);

    sf::Text usernameText("Username:", font, 20);
    usernameText.setPosition(200, 200);

    sf::Text startButton("Start Game", font, 20);
    startButton.setPosition(350, 400);

    sf::Text backButton("Back", font, 20);
    backButton.setPosition(350, 450);

    sf::Text errorMessage("", font, 20);
    errorMessage.setFillColor(sf::Color::Red);
    errorMessage.setPosition(200, 350);

    string usernameInput;
    sf::Text usernameField("", font, 20);
    usernameField.setPosition(400, 200);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return false;
            }

            if (event.type == sf::Event::TextEntered) {
                char enteredChar = static_cast<char>(event.text.unicode);
                if (enteredChar == '\b' && !usernameInput.empty()) {
                    usernameInput.pop_back();
                }
                else if (enteredChar >= 32 && enteredChar <= 126) {
                    usernameInput += enteredChar;
                }
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (startButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    if (usernameInput.empty()) {
                        errorMessage.setString("Error: Username cannot be empty.");
                    }
                    else if (!pl.usernameExists(usernameInput)) {
                        errorMessage.setString("Error: Username not found.");
                    }
                    else if (usernameInput == currentUser) {
                        errorMessage.setString("Error: Cannot play against yourself.");
                    }
                    else {
                        string friendsList = pl.getFriendsList(currentUser);
                        if (friendsList.find(usernameInput + "\n") != string::npos || friendsList.find(usernameInput) == friendsList.length() - usernameInput.length()) {
                            if (displayMatchTransitionPage(pl, window, font, currentUser, usernameInput, inventory)) {
                                playMultiplayerXonixGame(pl, window, font, currentUser, usernameInput);
                            }
                            return true;
                        }
                        else {
                            errorMessage.setString("Error: User is not in your friend list.");
                        }
                    }
                }

                if (backButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    return true;
                }
            }
        }

        usernameField.setString(usernameInput);

        window.clear();
        inventory.drawBackground(window); // Draw background
        window.draw(title);
        window.draw(usernameText);
        window.draw(usernameField);
        window.draw(startButton);
        window.draw(backButton);
        window.draw(errorMessage);
        window.display();
    }
    return false;
}

bool displayQueuePage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const string& currentUser, MatchmakingQueue& queue, Inventory& inventory) {
    window.setTitle("Matchmaking Queue");

    sf::Text title("Matchmaking Queue", font, 30);
    title.setPosition(300, 50);

    sf::Text queueText("Players in Queue:", font, 20);
    queueText.setPosition(200, 100);

    sf::Text queueContents("", font, 18);
    queueContents.setPosition(200, 130);

    sf::Text backButton("Back", font, 20);
    backButton.setPosition(350, 400);

    sf::Text statusMessage("", font, 20);
    statusMessage.setFillColor(sf::Color::Red);
    statusMessage.setPosition(200, 350);

    // Add player to queue if not already present
    Player* player = pl.getPlayerByUsername(currentUser);
    if (player && !queue.isPlayerInQueue(currentUser)) {
        if (queue.enqueue(player->playerID, pl.getPlayerScore(currentUser), currentUser)) {
            statusMessage.setString("Added to queue!");
        }
        else {
            statusMessage.setString("Error: Queue is full!");
        }
    }
    else if (queue.isPlayerInQueue(currentUser)) {
        statusMessage.setString("Already in queue!");
    }
    else {
        statusMessage.setString("Error: User not found!");
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return false;
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (backButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    return true;
                }
            }
        }

        // Update queue display
        int queueSize;
        std::string* queueList = queue.getQueueContents(queueSize);
        string queueStr = "";
        for (int i = 0; i < queueSize; i++) {
            if (!queueList[i].empty()) {
                queueStr += queueList[i] + "\n";
            }
        }
        delete[] queueList; // Clean up allocated memory
        if (queueStr.empty()) {
            queueStr = "No players in queue.";
        }
        queueContents.setString(queueStr);

        window.clear();
        inventory.drawBackground(window); // Draw background
        window.draw(title);
        window.draw(queueText);
        window.draw(queueContents);
        window.draw(backButton);
        window.draw(statusMessage);
        window.display();
    }
    return false;
}

bool displayInventoryPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& currentUser, Inventory& inventory) {
    window.setTitle("Inventory");

    sf::Text title("Select Background Theme", font, 30);
    title.setPosition(250, 50);

    sf::Text backButton("Back", font, 20);
    backButton.setPosition(350, 500);

    sf::Text statusMessage("", font, 20);
    statusMessage.setFillColor(sf::Color::Green);
    statusMessage.setPosition(200, 450);

    // Display background names
    const auto& backgroundNames = inventory.getBackgroundNames();
    vector<sf::Text> backgroundButtons;
    for (size_t i = 0; i < backgroundNames.size(); ++i) {
        sf::Text button(backgroundNames[i], font, 20);
        button.setPosition(300, 100 + static_cast<float>(i * 50));
        backgroundButtons.push_back(button);
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return false;
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                // Check for background selection
                for (size_t i = 0; i < backgroundButtons.size(); ++i) {
                    if (backgroundButtons[i].getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                        inventory.setBackground(static_cast<int>(i));
                        statusMessage.setString("Selected: " + backgroundNames[i]);
                    }
                }

                if (backButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    return true;
                }
            }
        }

        window.clear();
        inventory.drawBackground(window); // Draw background
        window.draw(title);
        for (const auto& button : backgroundButtons) {
            window.draw(button);
        }
        window.draw(backButton);
        window.draw(statusMessage);
        window.display();
    }
    return false;
}

void displayMenu(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, string& currentUser, Inventory& inventory) {
    static MatchmakingQueue queue; // Persistent queue across menu visits

    window.setTitle("Main Menu");

    sf::Text title("Main Menu", font, 30);
    title.setPosition(300, 50);

    sf::Text singlePlayerButton("Single Player", font, 20);
    singlePlayerButton.setPosition(350, 150);

    sf::Text multiplayerButton("Multiplayer", font, 20);
    multiplayerButton.setPosition(350, 200);

    sf::Text manualButton("Manual", font, 20);
    manualButton.setPosition(350, 250);

    sf::Text queueButton("Add Me to Queue", font, 20);
    queueButton.setPosition(350, 300);

    sf::Text levelButton("Levels", font, 20);
    levelButton.setPosition(350, 350);

    sf::Text inventoryButton("Inventory", font, 20);
    inventoryButton.setPosition(350, 400);

    sf::Text friendsButton("Friends", font, 20);
    friendsButton.setPosition(350, 450);

    sf::Text profileButton("Profile", font, 20);
    profileButton.setPosition(350, 500);

    sf::Text leaderboardButton("Leaderboard", font, 20);
    leaderboardButton.setPosition(350, 550);

    sf::Text backButton("Log Out", font, 20);
    backButton.setPosition(350, 600);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (singlePlayerButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    playXonixGame(pl, window, font, currentUser);
                }

                if (multiplayerButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    if (!displayMatchmakingPage(pl, window, font, currentUser, queue, inventory)) {
                        window.close();
                    }
                }

                if (manualButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    if (!displayManualMatchPage(pl, window, font, currentUser, inventory)) {
                        window.close();
                    }
                }

                if (queueButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    if (!displayQueuePage(pl, window, font, currentUser, queue, inventory)) {
                        window.close();
                    }
                }

                if (levelButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    window.setTitle("Level");
                    displayPlaceholderPage(pl, window, font, currentUser, inventory);
                }

                if (inventoryButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    if (!displayInventoryPage(pl, window, font, currentUser, inventory)) {
                        window.close();
                    }
                }

                static bool friendsLoaded = false; // Static flag to ensure it persists across function calls

                if (friendsButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    if (!friendsLoaded) {
                        pl.loadFriendsAndRequests(); // Load friends and requests only once
                        friendsLoaded = true;        // Set the flag to true after loading
                    }
                    bool backPressed = displayFriendPage(pl, window, font, currentUser, inventory);
                    if (!backPressed) {
                        window.close();
                    }
                }

                if (profileButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    displayProfilePage(pl, window, font, currentUser, inventory);
                }

                if (leaderboardButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    displayLeaderboardPage(pl, window, font, currentUser, inventory);
                }

                if (backButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    return;
                }
            }
        }

        window.clear();
        inventory.drawBackground(window); // Draw background
        window.draw(title);
        window.draw(singlePlayerButton);
        window.draw(multiplayerButton);
        window.draw(manualButton);
        window.draw(queueButton);
        window.draw(levelButton);
        window.draw(inventoryButton);
        window.draw(friendsButton);
        window.draw(profileButton);
        window.draw(leaderboardButton);
        window.draw(backButton);
        window.display();
    }
}