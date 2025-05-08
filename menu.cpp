//#include "menu.h"
//#include "friend.h"
//#include "login.h"
//#include "game.h"
//#include <iostream>
//#include <sstream>
//#include <vector>
//
//using namespace std;
//
//bool displayMatchTransitionPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const string& player1, const string& player2, Inventory& inventory) {
//    window.setTitle("Match Found");
//
//    sf::Text title("Match Found!", font, 30);
//    title.setFillColor(sf::Color::Yellow);
//    title.setPosition(300, 50);
//
//    sf::Text player1Text("Player 1: " + player1, font, 20);
//    player1Text.setFillColor(sf::Color::Green);
//    player1Text.setPosition(200, 150);
//
//    string player1StatsStr = pl.getPlayerStats(player1);
//    sf::Text player1Stats("", font, 18);
//    player1Stats.setFillColor(sf::Color::White);
//    player1Stats.setPosition(200, 180);
//    size_t scorePos = player1StatsStr.find("Total Score: ");
//    size_t powerUpsPos = player1StatsStr.find("Total Power-Ups: ");
//    if (scorePos != string::npos && powerUpsPos != string::npos) {
//        string score = player1StatsStr.substr(scorePos + 13, player1StatsStr.find('\n', scorePos) - scorePos - 13);
//        string powerUps = player1StatsStr.substr(powerUpsPos + 16);
//        player1Stats.setString("Score: " + score + "\nPower-Ups: " + powerUps);
//    }
//    else {
//        player1Stats.setString("Stats unavailable");
//    }
//
//    sf::Text player2Text("Player 2: " + player2, font, 20);
//    player2Text.setFillColor(sf::Color::Green);
//    player2Text.setPosition(200, 250);
//
//    string player2StatsStr = pl.getPlayerStats(player2);
//    sf::Text player2Stats("", font, 18);
//    player2Stats.setFillColor(sf::Color::White);
//    player2Stats.setPosition(200, 280);
//    scorePos = player2StatsStr.find("Total Score: ");
//    powerUpsPos = player2StatsStr.find("Total Power-Ups: ");
//    if (scorePos != string::npos && powerUpsPos != string::npos) {
//        string score = player2StatsStr.substr(scorePos + 13, player2StatsStr.find('\n', scorePos) - scorePos - 13);
//        string powerUps = player2StatsStr.substr(powerUpsPos + 16);
//        player2Stats.setString("Score: " + score + "\nPower-Ups: " + powerUps);
//    }
//    else {
//        player2Stats.setString("Stats unavailable");
//    }
//
//    sf::Text startButton("Start Game", font, 20);
//    startButton.setFillColor(sf::Color::Green);
//    startButton.setPosition(350, 400);
//
//    sf::Clock clock;
//    float displayTime = 5.0f;
//
//    while (window.isOpen()) {
//        sf::Event event;
//        while (window.pollEvent(event)) {
//            if (event.type == sf::Event::Closed) {
//                window.close();
//                return false;
//            }
//
//            if (event.type == sf::Event::MouseButtonPressed) {
//                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
//                if (startButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
//                    return true;
//                }
//            }
//        }
//
//        if (clock.getElapsedTime().asSeconds() >= displayTime) {
//            return true;
//        }
//
//        window.clear();
//        inventory.drawBackground(window);
//        window.draw(title);
//        window.draw(player1Text);
//        window.draw(player1Stats);
//        window.draw(player2Text);
//        window.draw(player2Stats);
//        window.draw(startButton);
//        window.display();
//    }
//    return false;
//}
//
//bool displayMatchmakingPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const string& currentUser, MatchmakingQueue& queue, Inventory& inventory) {
//    window.setTitle("Matchmaking");
//
//    sf::Text title("Waiting for Match...", font, 30);
//    title.setPosition(300, 200);
//
//    sf::Text cancelButton("Cancel", font, 20);
//    cancelButton.setPosition(350, 400);
//
//    sf::Text statusMessage("", font, 20);
//    statusMessage.setFillColor(sf::Color::Red);
//    statusMessage.setPosition(200, 350);
//
//    sf::Clock clock;
//    float checkInterval = 2.0f;
//
//    if (!queue.isPlayerInQueue(currentUser)) {
//        Player* player = pl.getPlayerByUsername(currentUser);
//        if (player && !queue.enqueue(player->playerID, pl.getPlayerScore(currentUser), currentUser)) {
//            statusMessage.setString("Error: Failed to join queue!");
//        }
//    }
//
//    while (window.isOpen()) {
//        sf::Event event;
//        while (window.pollEvent(event)) {
//            if (event.type == sf::Event::Closed) {
//                window.close();
//                return false;
//            }
//
//            if (event.type == sf::Event::MouseButtonPressed) {
//                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
//                if (cancelButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
//                    return true;
//                }
//            }
//        }
//
//        if (clock.getElapsedTime().asSeconds() >= checkInterval) {
//            std::cout << "Checking queue: Size = " << queue.getSize() << std::endl;
//            queue.debugPrintQueue();
//            if (queue.hasEnoughPlayers()) {
//                string player1, player2;
//                if (queue.dequeue(player1, player2)) {
//                    std::cout << "Match found: " << player1 << " vs " << player2 << std::endl;
//                    if (displayMatchTransitionPage(pl, window, font, player1, player2, inventory)) {
//                        playMultiplayerXonixGame(pl, window, font, player1, player2, inventory);
//                    }
//                    return true;
//                }
//                else {
//                    statusMessage.setString("Error: Failed to find a valid match!");
//                    std::cout << "Dequeue failed: Invalid match" << std::endl;
//                }
//            }
//            else {
//                statusMessage.setString("Waiting for more players...");
//            }
//            clock.restart();
//        }
//
//        window.clear();
//        inventory.drawBackground(window);
//        window.draw(title);
//        window.draw(cancelButton);
//        window.draw(statusMessage);
//        window.display();
//    }
//    return false;
//}
//
//bool displayManualMatchPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const string& currentUser, Inventory& inventory) {
//    window.setTitle("Manual Match");
//
//    sf::Text title("Enter Friend's Username", font, 30);
//    title.setPosition(250, 50);
//
//    sf::Text usernameText("Username:", font, 20);
//    usernameText.setPosition(200, 200);
//
//    sf::Text startButton("Start Game", font, 20);
//    startButton.setPosition(350, 400);
//
//    sf::Text backButton("Back", font, 20);
//    backButton.setPosition(350, 450);
//
//    sf::Text errorMessage("", font, 20);
//    errorMessage.setFillColor(sf::Color::Red);
//    errorMessage.setPosition(200, 350);
//
//    string usernameInput;
//    sf::Text usernameField("", font, 20);
//    usernameField.setPosition(400, 200);
//
//    while (window.isOpen()) {
//        sf::Event event;
//        while (window.pollEvent(event)) {
//            if (event.type == sf::Event::Closed) {
//                window.close();
//                return false;
//            }
//
//            if (event.type == sf::Event::TextEntered) {
//                char enteredChar = static_cast<char>(event.text.unicode);
//                if (enteredChar == '\b' && !usernameInput.empty()) {
//                    usernameInput.pop_back();
//                }
//                else if (enteredChar >= 32 && enteredChar <= 126) {
//                    usernameInput += enteredChar;
//                }
//            }
//
//            if (event.type == sf::Event::MouseButtonPressed) {
//                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
//
//                if (startButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
//                    if (usernameInput.empty()) {
//                        errorMessage.setString("Error: Username cannot be empty.");
//                    }
//                    else if (!pl.usernameExists(usernameInput)) {
//                        errorMessage.setString("Error: Username not found.");
//                    }
//                    else if (usernameInput == currentUser) {
//                        errorMessage.setString("Error: Cannot play against yourself.");
//                    }
//                    else {
//                        string friendsList = pl.getFriendsList(currentUser);
//                        if (friendsList.find(usernameInput + "\n") != string::npos ||
//                            friendsList.find(usernameInput) == friendsList.length() - usernameInput.length()) {
//                            if (displayMatchTransitionPage(pl, window, font, currentUser, usernameInput, inventory)) {
//                                playMultiplayerXonixGame(pl, window, font, currentUser, usernameInput, inventory);
//                            }
//                            return true;
//                        }
//                        else {
//                            errorMessage.setString("Error: User is not in your friend list.");
//                        }
//                    }
//                }
//
//                if (backButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
//                    return true;
//                }
//            }
//        }
//
//        usernameField.setString(usernameInput);
//
//        window.clear();
//        inventory.drawBackground(window);
//        window.draw(title);
//        window.draw(usernameText);
//        window.draw(usernameField);
//        window.draw(startButton);
//        window.draw(backButton);
//        window.draw(errorMessage);
//        window.display();
//    }
//    return false;
//}
//
//bool displayQueuePage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const string& currentUser, MatchmakingQueue& queue, Inventory& inventory) {
//    window.setTitle("Matchmaking Queue");
//
//    sf::Text title("Matchmaking Queue", font, 30);
//    title.setPosition(300, 50);
//
//    sf::Text queueText("Players in Queue:", font, 20);
//    queueText.setPosition(200, 100);
//
//    sf::Text queueContents("", font, 18);
//    queueContents.setPosition(200, 130);
//
//    sf::Text backButton("Back", font, 20);
//    backButton.setPosition(350, 400);
//
//    sf::Text statusMessage("", font, 20);
//    statusMessage.setFillColor(sf::Color::Red);
//    statusMessage.setPosition(200, 350);
//
//    Player* player = pl.getPlayerByUsername(currentUser);
//    if (player && !queue.isPlayerInQueue(currentUser)) {
//        if (queue.enqueue(player->playerID, pl.getPlayerScore(currentUser), currentUser)) {
//            statusMessage.setString("Added to queue!");
//        }
//        else {
//            statusMessage.setString("Error: Queue is full!");
//        }
//    }
//    else if (queue.isPlayerInQueue(currentUser)) {
//        statusMessage.setString("Already in queue!");
//    }
//    else {
//        statusMessage.setString("Error: User not found!");
//    }
//
//    while (window.isOpen()) {
//        sf::Event event;
//        while (window.pollEvent(event)) {
//            if (event.type == sf::Event::Closed) {
//                window.close();
//                return false;
//            }
//
//            if (event.type == sf::Event::MouseButtonPressed) {
//                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
//                if (backButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
//                    return true;
//                }
//            }
//        }
//
//        int queueSize;
//        std::string* queueList = queue.getQueueContents(queueSize);
//        string queueStr = "";
//        for (int i = 0; i < queueSize; i++) {
//            if (!queueList[i].empty()) {
//                queueStr += queueList[i] + "\n";
//            }
//        }
//        delete[] queueList;
//        if (queueStr.empty()) {
//            queueStr = "No players in queue.";
//        }
//        queueContents.setString(queueStr);
//
//        window.clear();
//        inventory.drawBackground(window);
//        window.draw(title);
//        window.draw(queueText);
//        window.draw(queueContents);
//        window.draw(backButton);
//        window.draw(statusMessage);
//        window.display();
//    }
//    return false;
//}
//
//bool displayInventoryPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& currentUser, Inventory& inventory) {
//    window.setTitle("Inventory");
//
//    sf::Text title("Inventory", font, 30);
//    title.setPosition(250, 50);
//
//    sf::Text themesHeader("Background Themes:", font, 24);
//    themesHeader.setPosition(200, 100);
//
//    sf::Text soundsHeader("Sound Tracks:", font, 24);
//    soundsHeader.setPosition(200, 300);
//
//    sf::Text backButton("Back", font, 20);
//    backButton.setPosition(350, 550);
//
//    sf::Text statusMessage("", font, 20);
//    statusMessage.setFillColor(sf::Color::Green);
//    statusMessage.setPosition(200, 500);
//
//    // Parse background names from string
//    string namesString = inventory.getBackgroundNames();
//    vector<string> backgroundNames;
//    stringstream ss(namesString);
//    string name;
//    while (getline(ss, name)) {
//        if (!name.empty()) {
//            backgroundNames.push_back(name);
//        }
//    }
//
//    // Parse sound names from string
//    string soundNamesString = inventory.getSoundNames();
//    vector<string> soundNames;
//    stringstream ssSounds(soundNamesString);
//    while (getline(ssSounds, name)) {
//        if (!name.empty()) {
//            soundNames.push_back(name);
//        }
//    }
//
//    // Create theme buttons
//    vector<sf::Text> backgroundButtons;
//    for (size_t i = 0; i < backgroundNames.size(); ++i) {
//        sf::Text button(backgroundNames[i], font, 20);
//        button.setPosition(300, 140 + static_cast<float>(i * 40));
//        if (i + 1 == inventory.getCurrentBackgroundID()) {
//            button.setFillColor(sf::Color::Yellow);
//        }
//        backgroundButtons.push_back(button);
//    }
//
//    // Create sound buttons
//    vector<sf::Text> soundButtons;
//    for (size_t i = 0; i < soundNames.size(); ++i) {
//        sf::Text button(soundNames[i], font, 20);
//        button.setPosition(300, 340 + static_cast<float>(i * 40));
//        Player* player = pl.getPlayerByUsername(currentUser);
//        if (player && i + 1 == player->preferredSoundID) {
//            button.setFillColor(sf::Color::Yellow);
//        }
//        soundButtons.push_back(button);
//    }
//
//    while (window.isOpen()) {
//        sf::Event event;
//        while (window.pollEvent(event)) {
//            if (event.type == sf::Event::Closed) {
//                window.close();
//                return false;
//            }
//
//            if (event.type == sf::Event::MouseButtonPressed) {
//                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
//
//                // Handle theme selection
//                for (size_t i = 0; i < backgroundButtons.size(); ++i) {
//                    if (backgroundButtons[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
//                        inventory.setBackground(i + 1);
//                        statusMessage.setString("Selected: " + backgroundNames[i]);
//                        Player* player = pl.getPlayerByUsername(currentUser);
//                        if (player) {
//                            player->preferredThemeID = i + 1;
//                            pl.savePlayerStats(currentUser, 0, 0, inventory.getCurrentBackgroundID(), inventory.getCurrentSoundID());
//
//                        }
//                        for (size_t j = 0; j < backgroundButtons.size(); ++j) {
//                            backgroundButtons[j].setFillColor(j + 1 == inventory.getCurrentBackgroundID() ? sf::Color::Yellow : sf::Color::White);
//                        }
//                    }
//                }
//
//                // Handle sound selection
//                for (size_t i = 0; i < soundButtons.size(); ++i) {
//                    if (soundButtons[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
//                        inventory.setSound(i + 1);
//                        statusMessage.setString("Selected: " + soundNames[i]);
//                        Player* player = pl.getPlayerByUsername(currentUser);
//                        if (player) {
//                            player->preferredSoundID = i + 1;
//                            pl.savePlayerStats(currentUser, 0, 0, inventory.getCurrentBackgroundID(), inventory.getCurrentSoundID());
//
//                        }
//                        for (size_t j = 0; j < soundButtons.size(); ++j) {
//                            soundButtons[j].setFillColor(j + 1 == player->preferredSoundID ? sf::Color::Yellow : sf::Color::White);
//                        }
//                    }
//                }
//
//                if (backButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
//                    return true;
//                }
//            }
//        }
//
//        window.clear();
//        inventory.drawBackground(window);
//        window.draw(title);
//        window.draw(themesHeader);
//        window.draw(soundsHeader);
//        for (const auto& button : backgroundButtons) {
//            window.draw(button);
//        }
//        for (const auto& button : soundButtons) {
//            window.draw(button);
//        }
//        window.draw(backButton);
//        window.draw(statusMessage);
//        window.display();
//    }
//    return false;
//}
//
//void displayMenu(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, string& currentUser, Inventory& inventory) {
//    static MatchmakingQueue queue;
//
//    window.setTitle("Main Menu");
//
//    sf::Text title("Main Menu", font, 30);
//    title.setPosition(300, 50);
//
//    sf::Text singlePlayerButton("Single Player", font, 20);
//    singlePlayerButton.setPosition(350, 150);
//
//    sf::Text multiplayerButton("Multiplayer", font, 20);
//    multiplayerButton.setPosition(350, 200);
//
//    sf::Text manualButton("Manual", font, 20);
//    manualButton.setPosition(350, 250);
//
//    sf::Text queueButton("Add Me to Queue", font, 20);
//    queueButton.setPosition(350, 300);
//
//    sf::Text levelButton("Levels", font, 20);
//    levelButton.setPosition(350, 350);
//
//    sf::Text inventoryButton("Inventory", font, 20);
//    inventoryButton.setPosition(350, 400);
//
//    sf::Text friendsButton("Friends", font, 20);
//    friendsButton.setPosition(350, 450);
//
//    sf::Text profileButton("Profile", font, 20);
//    profileButton.setPosition(350, 500);
//
//    sf::Text leaderboardButton("Leaderboard", font, 20);
//    leaderboardButton.setPosition(350, 550);
//
//    sf::Text backButton("Log Out", font, 20);
//    backButton.setPosition(350, 600);
//
//    static bool friendsLoaded = false;
//
//    while (window.isOpen()) {
//        sf::Event event;
//        while (window.pollEvent(event)) {
//            if (event.type == sf::Event::Closed) {
//                window.close();
//            }
//
//            if (event.type == sf::Event::MouseButtonPressed) {
//                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
//
//                if (singlePlayerButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
//                    playXonixGame(pl, window, font, currentUser, inventory);
//                }
//
//                if (multiplayerButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
//                    if (!displayMatchmakingPage(pl, window, font, currentUser, queue, inventory)) {
//                        window.close();
//                    }
//                }
//
//                if (manualButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
//                    if (!displayManualMatchPage(pl, window, font, currentUser, inventory)) {
//                        window.close();
//                    }
//                }
//
//                if (queueButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
//                    if (!displayQueuePage(pl, window, font, currentUser, queue, inventory)) {
//                        window.close();
//                    }
//                }
//
//                if (levelButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
//                    window.setTitle("Level");
//                    displayPlaceholderPage(pl, window, font, currentUser, inventory);
//                }
//
//                if (inventoryButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
//                    if (!displayInventoryPage(pl, window, font, currentUser, inventory)) {
//                        window.close();
//                    }
//                }
//
//                if (friendsButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
//                    if (!friendsLoaded) {
//                        pl.loadFriendsAndRequests();
//                        friendsLoaded = true;
//                    }
//                    bool backPressed = displayFriendPage(pl, window, font, currentUser, inventory);
//                    if (!backPressed) {
//                        window.close();
//                    }
//                }
//
//                if (profileButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
//                    displayProfilePage(pl, window, font, currentUser, inventory);
//                }
//
//                if (leaderboardButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
//                    displayLeaderboardPage(pl, window, font, currentUser, inventory);
//                }
//
//                if (backButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
//                    return;
//                }
//            }
//        }
//
//        window.clear();
//        inventory.drawBackground(window);
//        window.draw(title);
//        window.draw(singlePlayerButton);
//        window.draw(multiplayerButton);
//        window.draw(manualButton);
//        window.draw(queueButton);
//        window.draw(levelButton);
//        window.draw(inventoryButton);
//        window.draw(friendsButton);
//        window.draw(profileButton);
//        window.draw(leaderboardButton);
//        window.draw(backButton);
//        window.display();
//    }
//}
//
////#include "menu.h"
////#include "friend.h"
////#include "login.h"
////#include "game.h"
////#include <iostream>
////#include <sstream>
////#include <vector>
////
////using namespace std;
////
////// Helper function to create a styled button
////sf::RectangleShape createButton(float x, float y, float width, float height, sf::Color fillColor) {
////    sf::RectangleShape button(sf::Vector2f(width, height));
////    button.setPosition(x, y);
////    button.setFillColor(fillColor);
////    button.setOutlineColor(sf::Color(50, 50, 50));
////    button.setOutlineThickness(2);
////    return button;
////}
////
////bool displayMatchTransitionPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const string& player1, const string& player2, Inventory& inventory) {
////    window.setTitle("Match Found");
////
////    // Title
////    sf::Text title("Match Found!", font, 36);
////    title.setFillColor(sf::Color::Yellow);
////    title.setStyle(sf::Text::Bold);
////    title.setPosition(280, 40);
////    title.setOutlineColor(sf::Color::Black);
////    title.setOutlineThickness(1);
////
////    // Player 1 Info
////    sf::Text player1Text("Player 1: " + player1, font, 24);
////    player1Text.setFillColor(sf::Color(100, 255, 100));
////    player1Text.setPosition(200, 120);
////
////    string player1StatsStr = pl.getPlayerStats(player1);
////    sf::Text player1Stats("", font, 20);
////    player1Stats.setFillColor(sf::Color::White);
////    player1Stats.setPosition(200, 150);
////    size_t scorePos = player1StatsStr.find("Total Score: ");
////    size_t powerUpsPos = player1StatsStr.find("Total Power-Ups: ");
////    if (scorePos != string::npos && powerUpsPos != string::npos) {
////        string score = player1StatsStr.substr(scorePos + 13, player1StatsStr.find('\n', scorePos) - scorePos - 13);
////        string powerUps = player1StatsStr.substr(powerUpsPos + 16);
////        player1Stats.setString("Score: " + score + "\nPower-Ups: " + powerUps);
////    }
////    else {
////        player1Stats.setString("Stats unavailable");
////    }
////
////    // Player 2 Info
////    sf::Text player2Text("Player 2: " + player2, font, 24);
////    player2Text.setFillColor(sf::Color(100, 255, 100));
////    player2Text.setPosition(200, 220);
////
////    string player2StatsStr = pl.getPlayerStats(player2);
////    sf::Text player2Stats("", font, 20);
////    player2Stats.setFillColor(sf::Color::White);
////    player2Stats.setPosition(200, 250);
////    scorePos = player2StatsStr.find("Total Score: ");
////    powerUpsPos = player2StatsStr.find("Total Power-Ups: ");
////    if (scorePos != string::npos && powerUpsPos != string::npos) {
////        string score = player2StatsStr.substr(scorePos + 13, player2StatsStr.find('\n', scorePos) - scorePos - 13);
////        string powerUps = player2StatsStr.substr(powerUpsPos + 16);
////        player2Stats.setString("Score: " + score + "\nPower-Ups: " + powerUps);
////    }
////    else {
////        player2Stats.setString("Stats unavailable");
////    }
////
////    // Start Button
////    sf::RectangleShape startButtonBg = createButton(320, 360, 160, 40, sf::Color(50, 150, 50));
////    sf::Text startButton("Start Game", font, 22);
////    startButton.setFillColor(sf::Color::White);
////    startButton.setPosition(340, 365);
////
////    sf::Clock clock;
////    float displayTime = 5.0f;
////
////    while (window.isOpen()) {
////        sf::Event event;
////        while (window.pollEvent(event)) {
////            if (event.type == sf::Event::Closed) {
////                window.close();
////                return false;
////            }
////
////            if (event.type == sf::Event::MouseButtonPressed) {
////                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
////                if (startButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
////                    return true;
////                }
////            }
////        }
////
////        // Hover effect
////        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
////        startButtonBg.setFillColor(startButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? sf::Color(70, 180, 70) : sf::Color(50, 150, 50));
////
////        if (clock.getElapsedTime().asSeconds() >= displayTime) {
////            return true;
////        }
////
////        window.clear();
////        inventory.drawBackground(window);
////        window.draw(title);
////        window.draw(player1Text);
////        window.draw(player1Stats);
////        window.draw(player2Text);
////        window.draw(player2Stats);
////        window.draw(startButtonBg);
////        window.draw(startButton);
////        window.display();
////    }
////    return false;
////}
////
////bool displayMatchmakingPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const string& currentUser, MatchmakingQueue& queue, Inventory& inventory) {
////    window.setTitle("Matchmaking");
////
////    // Title
////    sf::Text title("Waiting for Match...", font, 36);
////    title.setFillColor(sf::Color::White);
////    title.setStyle(sf::Text::Bold);
////    title.setPosition(260, 150);
////    title.setOutlineColor(sf::Color::Black);
////    title.setOutlineThickness(1);
////
////    // Cancel Button
////    sf::RectangleShape cancelButtonBg = createButton(320, 360, 160, 40, sf::Color(150, 50, 50));
////    sf::Text cancelButton("Cancel", font, 22);
////    cancelButton.setFillColor(sf::Color::White);
////    cancelButton.setPosition(355, 365);
////
////    // Status Message
////    sf::Text statusMessage("", font, 20);
////    statusMessage.setFillColor(sf::Color::Red);
////    statusMessage.setPosition(200, 310);
////
////    sf::Clock clock;
////    float checkInterval = 2.0f;
////
////    if (!queue.isPlayerInQueue(currentUser)) {
////        Player* player = pl.getPlayerByUsername(currentUser);
////        if (player && !queue.enqueue(player->playerID, pl.getPlayerScore(currentUser), currentUser)) {
////            statusMessage.setString("Error: Failed to join queue!");
////        }
////    }
////
////    while (window.isOpen()) {
////        sf::Event event;
////        while (window.pollEvent(event)) {
////            if (event.type == sf::Event::Closed) {
////                window.close();
////                return false;
////            }
////
////            if (event.type == sf::Event::MouseButtonPressed) {
////                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
////                if (cancelButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
////                    return true;
////                }
////            }
////        }
////
////        // Hover effect
////        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
////        cancelButtonBg.setFillColor(cancelButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? sf::Color(180, 70, 70) : sf::Color(150, 50, 50));
////
////        if (clock.getElapsedTime().asSeconds() >= checkInterval) {
////            std::cout << "Checking queue: Size = " << queue.getSize() << std::endl;
////            queue.debugPrintQueue();
////            if (queue.hasEnoughPlayers()) {
////                string player1, player2;
////                if (queue.dequeue(player1, player2)) {
////                    std::cout << "Match found: " << player1 << " vs " << player2 << std::endl;
////                    if (displayMatchTransitionPage(pl, window, font, player1, player2, inventory)) {
////                        playMultiplayerXonixGame(pl, window, font, player1, player2, inventory);
////                    }
////                    return true;
////                }
////                else {
////                    statusMessage.setString("Error: Failed to find a valid match!");
////                    std::cout << "Dequeue failed: Invalid match" << std::endl;
////                }
////            }
////            else {
////                statusMessage.setString("Waiting for more players...");
////            }
////            clock.restart();
////        }
////
////        window.clear();
////        inventory.drawBackground(window);
////        window.draw(title);
////        window.draw(cancelButtonBg);
////        window.draw(cancelButton);
////        window.draw(statusMessage);
////        window.display();
////    }
////    return false;
////}
////
////bool displayManualMatchPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const string& currentUser, Inventory& inventory) {
////    window.setTitle("Manual Match");
////
////    // Title
////    sf::Text title("Enter Friend's Username", font, 36);
////    title.setFillColor(sf::Color::White);
////    title.setStyle(sf::Text::Bold);
////    title.setPosition(220, 40);
////    title.setOutlineColor(sf::Color::Black);
////    title.setOutlineThickness(1);
////
////    // Username Input
////    sf::Text usernameText("Username:", font, 24);
////    usernameText.setFillColor(sf::Color::White);
////    usernameText.setPosition(200, 120);
////
////    sf::RectangleShape inputBox(sf::Vector2f(200, 30));
////    inputBox.setPosition(320, 120);
////    inputBox.setFillColor(sf::Color(50, 50, 50));
////    inputBox.setOutlineColor(sf::Color::White);
////    inputBox.setOutlineThickness(1);
////
////    // Buttons
////    sf::RectangleShape startButtonBg = createButton(320, 360, 160, 40, sf::Color(50, 150, 50));
////    sf::Text startButton("Start Game", font, 22);
////    startButton.setFillColor(sf::Color::White);
////    startButton.setPosition(340, 365);
////
////    sf::RectangleShape backButtonBg = createButton(320, 410, 160, 40, sf::Color(150, 50, 50));
////    sf::Text backButton("Back", font, 22);
////    backButton.setFillColor(sf::Color::White);
////    backButton.setPosition(365, 415);
////
////    // Error Message
////    sf::Text errorMessage("", font, 20);
////    errorMessage.setFillColor(sf::Color::Red);
////    errorMessage.setPosition(200, 310);
////
////    string usernameInput;
////    sf::Text usernameField("", font, 22);
////    usernameField.setFillColor(sf::Color::White);
////    usernameField.setPosition(330, 122);
////
////    while (window.isOpen()) {
////        sf::Event event;
////        while (window.pollEvent(event)) {
////            if (event.type == sf::Event::Closed) {
////                window.close();
////                return false;
////            }
////
////            if (event.type == sf::Event::TextEntered) {
////                char enteredChar = static_cast<char>(event.text.unicode);
////                if (enteredChar == '\b' && !usernameInput.empty()) {
////                    usernameInput.pop_back();
////                }
////                else if (enteredChar >= 32 && enteredChar <= 126) {
////                    usernameInput += enteredChar;
////                }
////            }
////
////            if (event.type == sf::Event::MouseButtonPressed) {
////                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
////
////                if (startButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
////                    if (usernameInput.empty()) {
////                        errorMessage.setString("Error: Username cannot be empty.");
////                    }
////                    else if (!pl.usernameExists(usernameInput)) {
////                        errorMessage.setString("Error: Username not found.");
////                    }
////                    else if (usernameInput == currentUser) {
////                        errorMessage.setString("Error: Cannot play against yourself.");
////                    }
////                    else {
////                        string friendsList = pl.getFriendsList(currentUser);
////                        if (friendsList.find(usernameInput + "\n") != string::npos ||
////                            friendsList.find(usernameInput) == friendsList.length() - usernameInput.length()) {
////                            if (displayMatchTransitionPage(pl, window, font, currentUser, usernameInput, inventory)) {
////                                playMultiplayerXonixGame(pl, window, font, currentUser, usernameInput, inventory);
////                            }
////                            return true;
////                        }
////                        else {
////                            errorMessage.setString("Error: User is not in your friend list.");
////                        }
////                    }
////                }
////
////                if (backButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
////                    return true;
////                }
////            }
////        }
////
////        // Hover effects
////        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
////        startButtonBg.setFillColor(startButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? sf::Color(70, 180, 70) : sf::Color(50, 150, 50));
////        backButtonBg.setFillColor(backButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? sf::Color(180, 70, 70) : sf::Color(150, 50, 50));
////
////        usernameField.setString(usernameInput);
////
////        window.clear();
////        inventory.drawBackground(window);
////        window.draw(title);
////        window.draw(usernameText);
////        window.draw(inputBox);
////        window.draw(usernameField);
////        window.draw(startButtonBg);
////        window.draw(startButton);
////        window.draw(backButtonBg);
////        window.draw(backButton);
////        window.draw(errorMessage);
////        window.display();
////    }
////    return false;
////}
////
////bool displayQueuePage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const string& currentUser, MatchmakingQueue& queue, Inventory& inventory) {
////    window.setTitle("Matchmaking Queue");
////
////    // Title
////    sf::Text title("Matchmaking Queue", font, 36);
////    title.setFillColor(sf::Color::White);
////    title.setStyle(sf::Text::Bold);
////    title.setPosition(260, 40);
////    title.setOutlineColor(sf::Color::Black);
////    title.setOutlineThickness(1);
////
////    // Queue Info
////    sf::Text queueText("Players in Queue:", font, 24);
////    queueText.setFillColor(sf::Color::White);
////    queueText.setPosition(200, 100);
////
////    sf::RectangleShape queueBox(sf::Vector2f(400, 150));
////    queueBox.setPosition(200, 130);
////    queueBox.setFillColor(sf::Color(50, 50, 50, 200));
////    queueBox.setOutlineColor(sf::Color::White);
////    queueBox.setOutlineThickness(1);
////
////    sf::Text queueContents("", font, 20);
////    queueContents.setFillColor(sf::Color::White);
////    queueContents.setPosition(210, 140);
////
////    // Back Button
////    sf::RectangleShape backButtonBg = createButton(320, 360, 160, 40, sf::Color(150, 50, 50));
////    sf::Text backButton("Back", font, 22);
////    backButton.setFillColor(sf::Color::White);
////    backButton.setPosition(365, 365);
////
////    // Status Message
////    sf::Text statusMessage("", font, 20);
////    statusMessage.setFillColor(sf::Color::Green);
////    statusMessage.setPosition(200, 310);
////
////    Player* player = pl.getPlayerByUsername(currentUser);
////    if (player && !queue.isPlayerInQueue(currentUser)) {
////        if (queue.enqueue(player->playerID, pl.getPlayerScore(currentUser), currentUser)) {
////            statusMessage.setString("Added to queue!");
////        }
////        else {
////            statusMessage.setString("Error: Queue is full!");
////        }
////    }
////    else if (queue.isPlayerInQueue(currentUser)) {
////        statusMessage.setString("Already in queue!");
////    }
////    else {
////        statusMessage.setString("Error: User not found!");
////    }
////
////    while (window.isOpen()) {
////        sf::Event event;
////        while (window.pollEvent(event)) {
////            if (event.type == sf::Event::Closed) {
////                window.close();
////                return false;
////            }
////
////            if (event.type == sf::Event::MouseButtonPressed) {
////                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
////                if (backButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
////                    return true;
////                }
////            }
////        }
////
////        // Hover effect
////        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
////        backButtonBg.setFillColor(backButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? sf::Color(180, 70, 70) : sf::Color(150, 50, 50));
////
////        int queueSize;
////        std::string* queueList = queue.getQueueContents(queueSize);
////        string queueStr = "";
////        for (int i = 0; i < queueSize; i++) {
////            if (!queueList[i].empty()) {
////                queueStr += queueList[i] + "\n";
////            }
////        }
////        delete[] queueList;
////        if (queueStr.empty()) {
////            queueStr = "No players in queue.";
////        }
////        queueContents.setString(queueStr);
////
////        window.clear();
////        inventory.drawBackground(window);
////        window.draw(queueBox);
////        window.draw(title);
////        window.draw(queueText);
////        window.draw(queueContents);
////        window.draw(backButtonBg);
////        window.draw(backButton);
////        window.draw(statusMessage);
////        window.display();
////    }
////    return false;
////}
////
////bool displayInventoryPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& currentUser, Inventory& inventory) {
////    window.setTitle("Inventory");
////
////    // Title
////    sf::Text title("Inventory", font, 36);
////    title.setFillColor(sf::Color::White);
////    title.setStyle(sf::Text::Bold);
////    title.setPosition(330, 40);
////    title.setOutlineColor(sf::Color::Black);
////    title.setOutlineThickness(1);
////
////    // Headers
////    sf::Text themesHeader("Background Themes:", font, 28);
////    themesHeader.setFillColor(sf::Color::White);
////    themesHeader.setPosition(200, 100);
////
////    sf::Text soundsHeader("Sound Tracks:", font, 28);
////    soundsHeader.setFillColor(sf::Color::White);
////    soundsHeader.setPosition(200, 260);
////
////    // Back Button
////    sf::RectangleShape backButtonBg = createButton(320, 510, 160, 40, sf::Color(150, 50, 50));
////    sf::Text backButton("Back", font, 22);
////    backButton.setFillColor(sf::Color::White);
////    backButton.setPosition(365, 515);
////
////    // Status Message
////    sf::Text statusMessage("", font, 20);
////    statusMessage.setFillColor(sf::Color::Green);
////    statusMessage.setPosition(200, 460);
////
////    // Parse background names
////    string namesString = inventory.getBackgroundNames();
////    vector<string> backgroundNames;
////    stringstream ss(namesString);
////    string name;
////    while (getline(ss, name)) {
////        if (!name.empty()) {
////            backgroundNames.push_back(name);
////        }
////    }
////
////    // Parse sound names
////    string soundNamesString = inventory.getSoundNames();
////    vector<string> soundNames;
////    stringstream ssSounds(soundNamesString);
////    while (getline(ssSounds, name)) {
////        if (!name.empty()) {
////            soundNames.push_back(name);
////        }
////    }
////
////    // Create theme buttons
////    vector<sf::RectangleShape> backgroundButtonBgs;
////    vector<sf::Text> backgroundButtons;
////    for (size_t i = 0; i < backgroundNames.size(); ++i) {
////        sf::RectangleShape bg = createButton(300, 140 + i * 40, 200, 30, sf::Color(50, 50, 150));
////        sf::Text button(backgroundNames[i], font, 20);
////        button.setPosition(310, 142 + i * 40);
////        button.setFillColor(i + 1 == inventory.getCurrentBackgroundID() ? sf::Color::Yellow : sf::Color::White);
////        backgroundButtonBgs.push_back(bg);
////        backgroundButtons.push_back(button);
////    }
////
////    // Create sound buttons
////    vector<sf::RectangleShape> soundButtonBgs;
////    vector<sf::Text> soundButtons;
////    for (size_t i = 0; i < soundNames.size(); ++i) {
////        sf::RectangleShape bg = createButton(300, 300 + i * 40, 200, 30, sf::Color(50, 50, 150));
////        sf::Text button(soundNames[i], font, 20);
////        button.setPosition(310, 302 + i * 40);
////        Player* player = pl.getPlayerByUsername(currentUser);
////        button.setFillColor(player && i + 1 == player->preferredSoundID ? sf::Color::Yellow : sf::Color::White);
////        soundButtonBgs.push_back(bg);
////        soundButtons.push_back(button);
////    }
////
////    while (window.isOpen()) {
////        sf::Event event;
////        while (window.pollEvent(event)) {
////            if (event.type == sf::Event::Closed) {
////                window.close();
////                return false;
////            }
////
////            if (event.type == sf::Event::MouseButtonPressed) {
////                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
////
////                // Handle theme selection
////                for (size_t i = 0; i < backgroundButtons.size(); ++i) {
////                    if (backgroundButtons[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
////                        inventory.setBackground(i + 1);
////                        statusMessage.setString("Selected: " + backgroundNames[i]);
////                        Player* player = pl.getPlayerByUsername(currentUser);
////                        if (player) {
////                            player->preferredThemeID = i + 1;
////                            pl.savePlayerStats(currentUser, 0, 0, inventory.getCurrentBackgroundID(), inventory.getCurrentSoundID());
////                        }
////                        for (size_t j = 0; j < backgroundButtons.size(); ++j) {
////                            backgroundButtons[j].setFillColor(j + 1 == inventory.getCurrentBackgroundID() ? sf::Color::Yellow : sf::Color::White);
////                        }
////                    }
////                }
////
////                // Handle sound selection
////                for (size_t i = 0; i < soundButtons.size(); ++i) {
////                    if (soundButtons[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
////                        inventory.setSound(i + 1);
////                        statusMessage.setString("Selected: " + soundNames[i]);
////                        Player* player = pl.getPlayerByUsername(currentUser);
////                        if (player) {
////                            player->preferredSoundID = i + 1;
////                            pl.savePlayerStats(currentUser, 0, 0, inventory.getCurrentBackgroundID(), inventory.getCurrentSoundID());
////                        }
////                        for (size_t j = 0; j < soundButtons.size(); ++j) {
////                            soundButtons[j].setFillColor(j + 1 == player->preferredSoundID ? sf::Color::Yellow : sf::Color::White);
////                        }
////                    }
////                }
////
////                if (backButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
////                    return true;
////                }
////            }
////        }
////
////        // Hover effects
////        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
////        backButtonBg.setFillColor(backButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? sf::Color(180, 70, 70) : sf::Color(150, 50, 50));
////        for (size_t i = 0; i < backgroundButtonBgs.size(); ++i) {
////            backgroundButtonBgs[i].setFillColor(backgroundButtons[i].getGlobalBounds().contains(mousePos.x, mousePos.y) ? sf::Color(70, 70, 180) : sf::Color(50, 50, 150));
////        }
////        for (size_t i = 0; i < soundButtonBgs.size(); ++i) {
////            soundButtonBgs[i].setFillColor(soundButtons[i].getGlobalBounds().contains(mousePos.x, mousePos.y) ? sf::Color(70, 70, 180) : sf::Color(50, 50, 150));
////        }
////
////        window.clear();
////        inventory.drawBackground(window);
////        window.draw(title);
////        window.draw(themesHeader);
////        window.draw(soundsHeader);
////        for (size_t i = 0; i < backgroundButtons.size(); ++i) {
////            window.draw(backgroundButtonBgs[i]);
////            window.draw(backgroundButtons[i]);
////        }
////        for (size_t i = 0; i < soundButtons.size(); ++i) {
////            window.draw(soundButtonBgs[i]);
////            window.draw(soundButtons[i]);
////        }
////        window.draw(backButtonBg);
////        window.draw(backButton);
////        window.draw(statusMessage);
////        window.display();
////    }
////    return false;
////}
////
////void displayMenu(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, string& currentUser, Inventory& inventory) {
////    static MatchmakingQueue queue;
////
////    window.setTitle("Main Menu");
////
////    // Title
////    sf::Text title("Main Menu", font, 36);
////    title.setFillColor(sf::Color::White);
////    title.setStyle(sf::Text::Bold);
////    title.setPosition(330, 40);
////    title.setOutlineColor(sf::Color::Black);
////    title.setOutlineThickness(1);
////
////    // Buttons
////    sf::RectangleShape singlePlayerButtonBg = createButton(320, 120, 160, 40, sf::Color(50, 150, 50));
////    sf::Text singlePlayerButton("Single Player", font, 22);
////    singlePlayerButton.setFillColor(sf::Color::White);
////    singlePlayerButton.setPosition(335, 125);
////
////    sf::RectangleShape multiplayerButtonBg = createButton(320, 170, 160, 40, sf::Color(50, 150, 50));
////    sf::Text multiplayerButton("Multiplayer", font, 22);
////    multiplayerButton.setFillColor(sf::Color::White);
////    multiplayerButton.setPosition(340, 175);
////
////    sf::RectangleShape manualButtonBg = createButton(320, 220, 160, 40, sf::Color(50, 150, 50));
////    sf::Text manualButton("Manual", font, 22);
////    manualButton.setFillColor(sf::Color::White);
////    manualButton.setPosition(355, 225);
////
////    sf::RectangleShape queueButtonBg = createButton(320, 270, 160, 40, sf::Color(50, 150, 50));
////    sf::Text queueButton("Add Me to Queue", font, 22);
////    queueButton.setFillColor(sf::Color::White);
////    queueButton.setPosition(330, 275);
////
////    sf::RectangleShape levelButtonBg = createButton(320, 320, 160, 40, sf::Color(50, 150, 50));
////    sf::Text levelButton("Levels", font, 22);
////    levelButton.setFillColor(sf::Color::White);
////    levelButton.setPosition(355, 325);
////
////    sf::RectangleShape inventoryButtonBg = createButton(320, 370, 160, 40, sf::Color(50, 150, 50));
////    sf::Text inventoryButton("Inventory", font, 22);
////    inventoryButton.setFillColor(sf::Color::White);
////    inventoryButton.setPosition(345, 375);
////
////    sf::RectangleShape friendsButtonBg = createButton(320, 420, 160, 40, sf::Color(50, 150, 50));
////    sf::Text friendsButton("Friends", font, 22);
////    friendsButton.setFillColor(sf::Color::White);
////    friendsButton.setPosition(350, 425);
////
////    sf::RectangleShape profileButtonBg = createButton(320, 470, 160, 40, sf::Color(50, 150, 50));
////    sf::Text profileButton("Profile", font, 22);
////    profileButton.setFillColor(sf::Color::White);
////    profileButton.setPosition(350, 475);
////
////    sf::RectangleShape leaderboardButtonBg = createButton(320, 520, 160, 40, sf::Color(50, 150, 50));
////    sf::Text leaderboardButton("Leaderboard", font, 22);
////    leaderboardButton.setFillColor(sf::Color::White);
////    leaderboardButton.setPosition(335, 525);
////
////    sf::RectangleShape backButtonBg = createButton(320, 570, 160, 40, sf::Color(150, 50, 50));
////    sf::Text backButton("Log Out", font, 22);
////    backButton.setFillColor(sf::Color::White);
////    backButton.setPosition(350, 575);
////
////    static bool friendsLoaded = false;
////
////    while (window.isOpen()) {
////        sf::Event event;
////        while (window.pollEvent(event)) {
////            if (event.type == sf::Event::Closed) {
////                window.close();
////            }
////
////            if (event.type == sf::Event::MouseButtonPressed) {
////                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
////
////                if (singlePlayerButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
////                    playXonixGame(pl, window, font, currentUser, inventory);
////                }
////
////                if (multiplayerButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
////                    if (!displayMatchmakingPage(pl, window, font, currentUser, queue, inventory)) {
////                        window.close();
////                    }
////                }
////
////                if (manualButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
////                    if (!displayManualMatchPage(pl, window, font, currentUser, inventory)) {
////                        window.close();
////                    }
////                }
////
////                if (queueButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
////                    if (!displayQueuePage(pl, window, font, currentUser, queue, inventory)) {
////                        window.close();
////                    }
////                }
////
////                if (levelButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
////                    window.setTitle("Level");
////                    displayPlaceholderPage(pl, window, font, currentUser, inventory);
////                }
////
////                if (inventoryButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
////                    if (!displayInventoryPage(pl, window, font, currentUser, inventory)) {
////                        window.close();
////                    }
////                }
////
////                if (friendsButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
////                    if (!friendsLoaded) {
////                        pl.loadFriendsAndRequests();
////                        friendsLoaded = true;
////                    }
////                    bool backPressed = displayFriendPage(pl, window, font, currentUser, inventory);
////                    if (!backPressed) {
////                        window.close();
////                    }
////                }
////
////                if (profileButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
////                    displayProfilePage(pl, window, font, currentUser, inventory);
////                }
////
////                if (leaderboardButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
////                    displayLeaderboardPage(pl, window, font, currentUser, inventory);
////                }
////
////                if (backButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
////                    return;
////                }
////            }
////        }
////
////        // Hover effects
////        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
////        singlePlayerButtonBg.setFillColor(singlePlayerButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? sf::Color(70, 180, 70) : sf::Color(50, 150, 50));
////        multiplayerButtonBg.setFillColor(multiplayerButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? sf::Color(70, 180, 70) : sf::Color(50, 150, 50));
////        manualButtonBg.setFillColor(manualButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? sf::Color(70, 180, 70) : sf::Color(50, 150, 50));
////        queueButtonBg.setFillColor(queueButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? sf::Color(70, 180, 70) : sf::Color(50, 150, 50));
////        levelButtonBg.setFillColor(levelButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? sf::Color(70, 180, 70) : sf::Color(50, 150, 50));
////        inventoryButtonBg.setFillColor(inventoryButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? sf::Color(70, 180, 70) : sf::Color(50, 150, 50));
////        friendsButtonBg.setFillColor(friendsButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? sf::Color(70, 180, 70) : sf::Color(50, 150, 50));
////        profileButtonBg.setFillColor(profileButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? sf::Color(70, 180, 70) : sf::Color(50, 150, 50));
////        leaderboardButtonBg.setFillColor(leaderboardButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? sf::Color(70, 180, 70) : sf::Color(50, 150, 50));
////        backButtonBg.setFillColor(backButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? sf::Color(180, 70, 70) : sf::Color(150, 50, 50));
////
////        window.clear();
////        inventory.drawBackground(window);
////        window.draw(title);
////        window.draw(singlePlayerButtonBg);
////        window.draw(singlePlayerButton);
////        window.draw(multiplayerButtonBg);
////        window.draw(multiplayerButton);
////        window.draw(manualButtonBg);
////        window.draw(manualButton);
////        window.draw(queueButtonBg);
////        window.draw(queueButton);
////        window.draw(levelButtonBg);
////        window.draw(levelButton);
////        window.draw(inventoryButtonBg);
////        window.draw(inventoryButton);
////        window.draw(friendsButtonBg);
////        window.draw(friendsButton);
////        window.draw(profileButtonBg);
////        window.draw(profileButton);
////        window.draw(leaderboardButtonBg);
////        window.draw(leaderboardButton);
////        window.draw(backButtonBg);
////        window.draw(backButton);
////        window.display();
////    }

#include "menu.h"
#include "friend.h"
#include "login.h"
#include "game.h"
#include <iostream>
#include <sstream>

using namespace std;

bool displayMatchTransitionPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const string& player1, const string& player2, Inventory& inventory) {
    window.setTitle("Match Found");

    sf::Text title("Match Found!", font, 30);
    title.setFillColor(sf::Color::Yellow);
    title.setPosition(300, 30);

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
        string score = player2StatsStr.substr(scorePos + 13, player1StatsStr.find('\n', scorePos) - scorePos - 13);
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
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (startButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    return true;
                }
            }
        }

        if (clock.getElapsedTime().asSeconds() >= displayTime) {
            return true;
        }

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
            std::cout << "Checking queue: Size = " << queue.getSize() << std::endl;
            queue.debugPrintQueue();
            if (queue.hasEnoughPlayers()) {
                string player1, player2;
                if (queue.dequeue(player1, player2)) {
                    std::cout << "Match found: " << player1 << " vs " << player2 << std::endl;
                    if (displayMatchTransitionPage(pl, window, font, player1, player2, inventory)) {
                        playMultiplayerXonixGame(pl, window, font, player1, player2, inventory);
                    }
                    return true;
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
                        if (friendsList.find(usernameInput + "\n") != string::npos ||
                            friendsList.find(usernameInput) == friendsList.length() - usernameInput.length()) {
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

bool displayInventoryPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& currentUser, Inventory& inventory) {
    window.setTitle("Inventory");

    sf::Text title("Select Background Theme", font, 30);
    title.setPosition(250, 50);

    sf::Text backButton("Back", font, 20);
    backButton.setPosition(350, 500);

    sf::Text statusMessage("", font, 20);
    statusMessage.setFillColor(sf::Color::Green);
    statusMessage.setPosition(200, 450);

    // Parse background names from string
    string namesString = inventory.getBackgroundNames();
    vector<string> backgroundNames;
    stringstream ss(namesString);
    string name;
    while (getline(ss, name)) {
        if (!name.empty()) {
            backgroundNames.push_back(name);
        }
    }

    vector<sf::Text> backgroundButtons;
    for (size_t i = 0; i < backgroundNames.size(); ++i) {
        sf::Text button(backgroundNames[i], font, 20);
        button.setPosition(300, 100 + static_cast<float>(i * 50));
        if (i + 1 == inventory.getCurrentBackgroundID()) {
            button.setFillColor(sf::Color::Yellow);
        }
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
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                for (size_t i = 0; i < backgroundButtons.size(); ++i) {
                    if (backgroundButtons[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        inventory.setBackground(i + 1);
                        statusMessage.setString("Selected: " + backgroundNames[i]);
                        // Update highlight
                        for (size_t j = 0; j < backgroundButtons.size(); ++j) {
                            backgroundButtons[j].setFillColor(j + 1 == inventory.getCurrentBackgroundID() ? sf::Color::Yellow : sf::Color::White);
                        }
                    }
                }

                if (backButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    return true;
                }
            }
        }

        window.clear();
        inventory.drawBackground(window);
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
    static MatchmakingQueue queue;

    window.setTitle("Main Menu");

    sf::Text title("Main Menu", font, 30);
    title.setPosition(300, 30);

    // List of buttons
    std::vector<sf::Text> buttons = {
        sf::Text("Single Player", font, 20),
        sf::Text("Multiplayer", font, 20),
        sf::Text("Manual", font, 20),
        sf::Text("Add Me to Queue", font, 20),
        sf::Text("Levels", font, 20),
        sf::Text("Inventory", font, 20),
        sf::Text("Friends", font, 20),
        sf::Text("Profile", font, 20),
        sf::Text("Leaderboard", font, 20),
        sf::Text("Log Out", font, 20)
    };

    // Get window size
    sf::Vector2u windowSize = window.getSize();
    float windowWidth = static_cast<float>(windowSize.x);
    float windowHeight = static_cast<float>(windowSize.y);

    // Calculate positions for two columns
    float columnWidth = windowWidth / 2.0f;
    float buttonHeight = 40.0f; // Approximate height of each button
    float verticalSpacing = 20.0f; // Spacing between buttons
    float startXLeft = columnWidth / 2.0f - 100.0f; // Center left column
    float startXRight = columnWidth + columnWidth / 2.0f - 100.0f; // Center right column
    float startY = 100.0f; // Starting Y position

    // Position buttons in two columns
    for (size_t i = 0; i < buttons.size(); ++i) {
        if (i % 2 == 0) {
            // Left column
            buttons[i].setPosition(startXLeft, startY + (i / 2) * (buttonHeight + verticalSpacing));
        }
        else {
            // Right column
            buttons[i].setPosition(startXRight, startY + (i / 2) * (buttonHeight + verticalSpacing));
        }
    }

    static bool friendsLoaded = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                // Check which button was clicked
                if (buttons[0].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    playXonixGame(pl, window, font, currentUser, inventory);
                }
                if (buttons[1].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    if (!displayMatchmakingPage(pl, window, font, currentUser, queue, inventory)) {
                        window.close();
                    }
                }
                if (buttons[2].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    if (!displayManualMatchPage(pl, window, font, currentUser, inventory)) {
                        window.close();
                    }
                }
                if (buttons[3].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    if (!displayQueuePage(pl, window, font, currentUser, queue, inventory)) {
                        window.close();
                    }
                }
                if (buttons[4].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    window.setTitle("Level");
                    displayPlaceholderPage(pl, window, font, currentUser, inventory);
                }
                if (buttons[5].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    if (!displayInventoryPage(pl, window, font, currentUser, inventory)) {
                        window.close();
                    }
                }
                if (buttons[6].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    if (!friendsLoaded) {
                        pl.loadFriendsAndRequests();
                        friendsLoaded = true;
                    }
                    bool backPressed = displayFriendPage(pl, window, font, currentUser, inventory);
                    if (!backPressed) {
                        window.close();
                    }
                }
                if (buttons[7].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    displayProfilePage(pl, window, font, currentUser, inventory);
                }
                if (buttons[8].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    displayLeaderboardPage(pl, window, font, currentUser, inventory);
                }
                if (buttons[9].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    return;
                }
            }
        }

        window.clear();
        inventory.drawBackground(window);
        window.draw(title);
        for (const auto& button : buttons) {
            window.draw(button);
        }
        window.display();
    }
}

