#include "menu.h"
#include "friend.h"
#include "login.h"
#include "game.h"
#include <iostream>
#include <sstream>

using namespace std;

bool displayMatchTransitionPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const string& player1, const string& player2, Inventory& inventory) {
    window.setTitle("Match Found");

    // Title
    sf::Text title("Match Found!", font, 30);
    title.setFillColor(sf::Color::Yellow);
    title.setPosition(300, 50);

    // Player 1 info
    sf::Text player1Text("Player 1: " + player1, font, 20);
    player1Text.setFillColor(sf::Color::Green);
    player1Text.setPosition(200, 150);

    sf::Text player1Stats("", font, 18);
    player1Stats.setFillColor(sf::Color::White);
    player1Stats.setPosition(200, 180);
    string player1StatsStr = pl.getPlayerStats(player1);
    string scoreStr = "Total Score: ";
    string powerUpsStr = "Total Power-Ups: ";
    size_t scorePos = player1StatsStr.find(scoreStr);
    size_t powerUpsPos = player1StatsStr.find(powerUpsStr);
    if (scorePos != string::npos && powerUpsPos != string::npos) {
        // Extract score
        size_t scoreEnd = player1StatsStr.find('\n', scorePos);
        string score = player1StatsStr.substr(scorePos + scoreStr.length(), scoreEnd - scorePos - scoreStr.length());
        // Extract power-ups
        string powerUps = player1StatsStr.substr(powerUpsPos + powerUpsStr.length());
        player1Stats.setString("Score: " + score + "\nPower-Ups: " + powerUps);
    }
    else {
        player1Stats.setString("Stats unavailable");
    }

    // Player 2 info
    sf::Text player2Text("Player 2: " + player2, font, 20);
    player2Text.setFillColor(sf::Color::Green);
    player2Text.setPosition(200, 310);

    sf::Text player2Stats("", font, 18);
    player2Stats.setFillColor(sf::Color::White);
    player2Stats.setPosition(200, 340);
    string player2StatsStr = pl.getPlayerStats(player2);
    scorePos = player2StatsStr.find(scoreStr);
    powerUpsPos = player2StatsStr.find(powerUpsStr);
    if (scorePos != string::npos && powerUpsPos != string::npos) {
        // Extract score
        size_t scoreEnd = player2StatsStr.find('\n', scorePos);
        string score = player2StatsStr.substr(scorePos + scoreStr.length(), scoreEnd - scorePos - scoreStr.length());
        // Extract power-ups
        string powerUps = player2StatsStr.substr(powerUpsPos + powerUpsStr.length());
        player2Stats.setString("Score: " + score + "\nPower-Ups: " + powerUps);
    }
    else {
        player2Stats.setString("Stats unavailable");
    }

    // Start button
    sf::Text startButton("Start Game", font, 20);
    startButton.setFillColor(sf::Color::Green);
    startButton.setPosition(350, 470);

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
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (startButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    return true;
                }
            }
        }

        if (clock.getElapsedTime().asSeconds() >= displayTime) {
            return true;
        }

        // Draw everything
        window.clear();
        inventory.drawBackground(window);
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

    // Add player to queue if not already in
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
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (cancelButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    return true;
                }
            }
        }

        if (clock.getElapsedTime().asSeconds() >= checkInterval) {
            if (queue.hasEnoughPlayers()) {
                string player1, player2;
                if (queue.dequeue(player1, player2)) {
                    if (displayMatchTransitionPage(pl, window, font, player1, player2, inventory)) {
                        playMultiplayerXonixGame(pl, window, font, player1, player2, inventory);
                    }
                    return true;
                }
                else {
                    statusMessage.setString("Error: Failed to find a valid match!");
                }
            }
            else {
                statusMessage.setString("Waiting for more players...");
            }
            clock.restart();
        }

        window.clear();
        inventory.drawBackground(window);
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
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                if (startButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
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
                        // Check if username is in friends list
                        string searchStr = usernameInput + "\n";
                        bool isFriend = friendsList.find(searchStr) != string::npos ||
                            friendsList.find(usernameInput) == friendsList.length() - usernameInput.length();
                        if (isFriend) {
                            if (displayMatchTransitionPage(pl, window, font, currentUser, usernameInput, inventory)) {
                                playMultiplayerXonixGame(pl, window, font, currentUser, usernameInput, inventory);
                            }
                            return true;
                        }
                        else {
                            errorMessage.setString("Error: User is not in your friend list.");
                        }
                    }
                }

                if (backButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    return true;
                }
            }
        }

        usernameField.setString(usernameInput);

        window.clear();
        inventory.drawBackground(window);
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
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (backButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    return true;
                }
            }
        }

        int queueSize;
        std::string* queueList = queue.getQueueContents(queueSize);
        string queueStr = "";
        for (int i = 0; i < queueSize; i++) {
            if (!queueList[i].empty()) {
                queueStr += queueList[i] + "\n";
            }
        }
        delete[] queueList;
        if (queueStr.empty()) {
            queueStr = "No players in queue.";
        }
        queueContents.setString(queueStr);

        window.clear();
        inventory.drawBackground(window);
        window.draw(title);
        window.draw(queueText);
        window.draw(queueContents);
        window.draw(backButton);
        window.draw(statusMessage);
        window.display();
    }
    return false;
}

bool displayInventoryPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const string& currentUser, Inventory& inventory) {
    window.setTitle("Inventory");

    // Title and headers
    sf::Text title("Inventory", font, 30);
    title.setPosition(250, 50);

    sf::Text themesHeader("Background Themes:", font, 24);
    themesHeader.setPosition(200, 100);

    sf::Text soundsHeader("Sound Tracks:", font, 24);
    soundsHeader.setPosition(200, 300);

    sf::Text backButton("Back", font, 20);
    backButton.setPosition(350, 550);

    sf::Text statusMessage("", font, 20);
    statusMessage.setFillColor(sf::Color::Green);
    statusMessage.setPosition(200, 500);

    // Parse background names (assume max 10 backgrounds)
    const int MAX_ITEMS = 10;
    string backgroundNames[MAX_ITEMS];
    int numBackgrounds = 0;
    string namesString = inventory.getBackgroundNames();
    size_t start = 0;
    size_t end = namesString.find('\n');
    // Loop to split string by newlines
    while (end != string::npos && numBackgrounds < MAX_ITEMS) {
        string name = namesString.substr(start, end - start);
        if (!name.empty()) {
            backgroundNames[numBackgrounds++] = name;
        }
        start = end + 1;
        end = namesString.find('\n', start);
    }
    // Handle last name (if no trailing newline)
    if (start < namesString.length() && numBackgrounds < MAX_ITEMS) {
        string name = namesString.substr(start);
        if (!name.empty()) {
            backgroundNames[numBackgrounds++] = name;
        }
    }

    // Parse sound names (assume max 10 sounds)
    string soundNames[MAX_ITEMS];
    int numSounds = 0;
    string soundNamesString = inventory.getSoundNames();
    start = 0;
    end = soundNamesString.find('\n');
    // Loop to split string by newlines
    while (end != string::npos && numSounds < MAX_ITEMS) {
        string name = soundNamesString.substr(start, end - start);
        if (!name.empty()) {
            soundNames[numSounds++] = name;
        }
        start = end + 1;
        end = soundNamesString.find('\n', start);
    }
    // Handle last name
    if (start < soundNamesString.length() && numSounds < MAX_ITEMS) {
        string name = soundNamesString.substr(start);
        if (!name.empty()) {
            soundNames[numSounds++] = name;
        }
    }

    // Create theme buttons
    sf::Text backgroundButtons[MAX_ITEMS];
    for (int i = 0; i < numBackgrounds; ++i) {
        backgroundButtons[i].setFont(font);
        backgroundButtons[i].setString(backgroundNames[i]);
        backgroundButtons[i].setCharacterSize(20);
        backgroundButtons[i].setPosition(300, 140 + i * 40);
        if (i + 1 == inventory.getCurrentBackgroundID()) {
            backgroundButtons[i].setFillColor(sf::Color::Yellow);
        }
        else {
            backgroundButtons[i].setFillColor(sf::Color::White);
        }
    }

    // Create sound buttons
    sf::Text soundButtons[MAX_ITEMS];
    for (int i = 0; i < numSounds; ++i) {
        soundButtons[i].setFont(font);
        soundButtons[i].setString(soundNames[i]);
        soundButtons[i].setCharacterSize(20);
        soundButtons[i].setPosition(300, 340 + i * 40);
        Player* player = pl.getPlayerByUsername(currentUser);
        if (player && i + 1 == player->preferredSoundID) {
            soundButtons[i].setFillColor(sf::Color::Yellow);
        }
        else {
            soundButtons[i].setFillColor(sf::Color::White);
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
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                // Handle theme selection
                for (int i = 0; i < numBackgrounds; ++i) {
                    if (backgroundButtons[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        inventory.setBackground(i + 1);
                        statusMessage.setString("Selected: " + backgroundNames[i]);
                        Player* player = pl.getPlayerByUsername(currentUser);
                        if (player) {
                            player->preferredThemeID = i + 1;
                            pl.savePlayerStats(currentUser, 0, 0, inventory.getCurrentBackgroundID(), inventory.getCurrentSoundID());
                        }
                        // Update button colors
                        for (int j = 0; j < numBackgrounds; ++j) {
                            backgroundButtons[j].setFillColor((j + 1 == inventory.getCurrentBackgroundID()) ? sf::Color::Yellow : sf::Color::White);
                        }
                    }
                }

                // Handle sound selection
                for (int i = 0; i < numSounds; ++i) {
                    if (soundButtons[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        inventory.setSound(i + 1);
                        statusMessage.setString("Selected: " + soundNames[i]);
                        Player* player = pl.getPlayerByUsername(currentUser);
                        if (player) {
                            player->preferredSoundID = i + 1;
                            pl.savePlayerStats(currentUser, 0, 0, inventory.getCurrentBackgroundID(), inventory.getCurrentSoundID());
                        }
                        // Update button colors
                        for (int j = 0; j < numSounds; ++j) {
                            soundButtons[j].setFillColor((j + 1 == player->preferredSoundID) ? sf::Color::Yellow : sf::Color::White);
                        }
                    }
                }

                if (backButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    return true;
                }
            }
        }

        // Draw everything
        window.clear();
        inventory.drawBackground(window);
        window.draw(title);
        window.draw(themesHeader);
        window.draw(soundsHeader);
        for (int i = 0; i < numBackgrounds; ++i) {
            window.draw(backgroundButtons[i]);
        }
        for (int i = 0; i < numSounds; ++i) {
            window.draw(soundButtons[i]);
        }
        window.draw(backButton);
        window.draw(statusMessage);
        window.display();
    }
    return false;
}

void displayMenu(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, string& currentUser, Inventory& inventory) {
    // Create and initialize a static MatchmakingQueue instead of using vector
    static MatchmakingQueue queue;

    window.setTitle("Main Menu");
    window.setSize(sf::Vector2u(800, 600));

    sf::Text title("Main Menu", font, 30);
    title.setFillColor(sf::Color::White);
    title.setPosition(300, 50);
    title.setOutlineColor(sf::Color::Black);
    title.setOutlineThickness(1);

    // Left column text labels - using static arrays instead of vectors
    sf::Text leftLabels[5];
    const char* leftLabelStrings[5] = { "Single Player", "Resume Game", "Multiplayer", "Manual", "Queue" };
    for (int i = 0; i < 5; ++i) {
        leftLabels[i].setFont(font);
        leftLabels[i].setString(leftLabelStrings[i]);
        leftLabels[i].setCharacterSize(22);
        leftLabels[i].setFillColor(sf::Color::White);
        leftLabels[i].setPosition(170, 150 + i * 50);
        leftLabels[i].setOutlineColor(sf::Color::Black);
        leftLabels[i].setOutlineThickness(1);
    }

    // Right column text labels - using static arrays instead of vectors
    sf::Text rightLabels[5];
    const char* rightLabelStrings[5] = { "Inventory", "Friends", "Profile", "Leaderboard", "Log Out" };
    for (int i = 0; i < 5; ++i) {
        rightLabels[i].setFont(font);
        rightLabels[i].setString(rightLabelStrings[i]);
        rightLabels[i].setCharacterSize(22);
        rightLabels[i].setFillColor(sf::Color::White);
        rightLabels[i].setPosition(420, 150 + i * 50);
        rightLabels[i].setOutlineColor(sf::Color::Black);
        rightLabels[i].setOutlineThickness(1);
    }

    sf::Text feedbackMessage("", font, 20);
    feedbackMessage.setFillColor(sf::Color::Red);
    feedbackMessage.setPosition(200, 500);
    feedbackMessage.setOutlineColor(sf::Color::Black);
    feedbackMessage.setOutlineThickness(1);
    bool showFeedback = false;
    sf::Clock feedbackClock;

    bool friendsLoaded = false;
    bool hasSaveFile = saveFileExists(currentUser.c_str());

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                if (leftLabels[0].getGlobalBounds().contains(mousePos.x, mousePos.y)) { // Single Player
                    playXonixGame(pl, window, font, currentUser, inventory, nullptr);
                    hasSaveFile = saveFileExists(currentUser.c_str());
                }

                if (leftLabels[1].getGlobalBounds().contains(mousePos.x, mousePos.y) && hasSaveFile) { // Resume Game
                    SaveGameState state;
                    if (loadGame(currentUser, state)) {
                        playXonixGame(pl, window, font, currentUser, inventory, &state);
                        hasSaveFile = saveFileExists(currentUser.c_str());
                    }
                    else {
                        feedbackMessage.setString("Invalid Save File");
                        showFeedback = true;
                        feedbackClock.restart();
                    }
                }

                if (leftLabels[2].getGlobalBounds().contains(mousePos.x, mousePos.y)) { // Multiplayer
                    displayMatchmakingPage(pl, window, font, currentUser, queue, inventory);
                }

                if (leftLabels[3].getGlobalBounds().contains(mousePos.x, mousePos.y)) { // Manual
                    displayManualMatchPage(pl, window, font, currentUser, inventory);
                }

                if (leftLabels[4].getGlobalBounds().contains(mousePos.x, mousePos.y)) { // Queue
                    displayQueuePage(pl, window, font, currentUser, queue, inventory);
                }

                if (rightLabels[0].getGlobalBounds().contains(mousePos.x, mousePos.y)) { // Inventory
                    displayInventoryPage(pl, window, font, currentUser, inventory);
                }

                if (rightLabels[1].getGlobalBounds().contains(mousePos.x, mousePos.y)) { // Friends
                    if (!friendsLoaded) {
                        pl.loadFriendsAndRequests();
                        friendsLoaded = true;
                    }
                    displayFriendPage(pl, window, font, currentUser, inventory);
                }

                if (rightLabels[2].getGlobalBounds().contains(mousePos.x, mousePos.y)) { // Profile
                    displayProfilePage(pl, window, font, currentUser, inventory);
                }

                if (rightLabels[3].getGlobalBounds().contains(mousePos.x, mousePos.y)) { // Leaderboard
                    displayLeaderboardPage(pl, window, font, currentUser, inventory);
                }

                if (rightLabels[4].getGlobalBounds().contains(mousePos.x, mousePos.y)) { // Log Out
                    return;
                }
            }
        }

        // Update text colors based on mouse hover
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        for (int i = 0; i < 5; ++i) {
            if (i == 1 && !hasSaveFile) {
                leftLabels[i].setFillColor(sf::Color(150, 150, 150)); // Gray out Resume Game if no save
            }
            else {
                leftLabels[i].setFillColor(leftLabels[i].getGlobalBounds().contains(mousePos.x, mousePos.y) ? sf::Color(70, 180, 70) : sf::Color::White);
            }
        }
        for (int i = 0; i < 5; ++i) {
            rightLabels[i].setFillColor(rightLabels[i].getGlobalBounds().contains(mousePos.x, mousePos.y) ? sf::Color(70, 180, 70) : sf::Color::White);
        }

        // Draw everything
        window.clear();
        inventory.drawBackground(window);
        window.draw(title);
        for (int i = 0; i < 5; ++i) {
            window.draw(leftLabels[i]);
        }
        for (int i = 0; i < 5; ++i) {
            window.draw(rightLabels[i]);
        }
        if (showFeedback && feedbackClock.getElapsedTime().asSeconds() < 2) {
            window.draw(feedbackMessage);
        }
        else {
            showFeedback = false;
        }
        window.display();
    }
}