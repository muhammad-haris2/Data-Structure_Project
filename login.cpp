#include "game.h" // Changed from login.h to game.h for SaveGameState, playXonixGame, loadGame
#include "menu.h"
#include <SFML/Graphics.hpp> // Added for SFML types
#include <SFML/Audio.hpp> // Added for Music
#include <iostream>
#include <fstream>
#include "login.h"

using namespace std;
using namespace sf;



// Check if save file exists
bool saveFileExists(const char* username) {
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
    bool exists = file.is_open();
    file.close();
    return exists;
}

void displayMainMenu(PlayerList& pl, RenderWindow& window, Font& font, string& currentUser, Inventory& inventory) {
    window.setTitle("Xonix - Main Menu");
    const int windowWidth = 800;
    const int windowHeight = 600;
    window.setSize(Vector2u(windowWidth, windowHeight));

    Text title("Xonix Game", font, 30);
    title.setFillColor(Color::White);
    title.setPosition(300, 50);
    title.setOutlineColor(Color::Black);
    title.setOutlineThickness(1);

    RectangleShape buttons[3];
    Text buttonTexts[3];
    const char* buttonLabels[3] = { "Login", "Register", "Exit" };
    for (int i = 0; i < 3; ++i) {
        buttons[i] = createButton(320, 200 + i * 60, 160, 40, Color(50, 150, 50));
        buttonTexts[i].setFont(font);
        buttonTexts[i].setString(buttonLabels[i]);
        buttonTexts[i].setCharacterSize(22);
        buttonTexts[i].setFillColor(Color::White);
        buttonTexts[i].setPosition(350, 205 + i * 60);
    }

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }

            if (event.type == Event::MouseButtonPressed) {
                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

                if (buttonTexts[0].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    displayLoginPage(pl, window, font, currentUser, inventory);
                    if (!currentUser.empty()) {
                        displayMenu(pl, window, font, currentUser, inventory);
                        currentUser.clear();
                    }
                }

                if (buttonTexts[1].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    displayRegistrationPage(pl, window, font, currentUser, inventory);
                    if (!currentUser.empty()) {
                        displayMenu(pl, window, font, currentUser, inventory);
                        currentUser.clear();
                    }
                }

                if (buttonTexts[2].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    window.close();
                }
            }
        }

        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
        for (int i = 0; i < 3; ++i) {
            buttons[i].setFillColor(buttonTexts[i].getGlobalBounds().contains(mousePos.x, mousePos.y) ? Color(70, 180, 70) : Color(50, 150, 50));
        }

        window.clear();
        inventory.drawBackground(window);
        window.draw(title);
        for (int i = 0; i < 3; ++i) {
            window.draw(buttons[i]);
            window.draw(buttonTexts[i]);
        }
        window.display();
    }
}

void displayLoginPage(PlayerList& pl, RenderWindow& window, Font& font, string& currentUser, Inventory& inventory) {
    window.setTitle("Xonix - Login");
    const int windowWidth = 800;
    const int windowHeight = 600;
    window.setSize(Vector2u(windowWidth, windowHeight));

    inventory.setSound(1);
    inventory.setBackground(1);

    Text usernameText("Username:", font, 20);
    usernameText.setFillColor(Color::White);
    usernameText.setPosition(200, 200);
    usernameText.setOutlineColor(Color::Black);
    usernameText.setOutlineThickness(1);

    Text passwordText("Password:", font, 20);
    passwordText.setFillColor(Color::White);
    passwordText.setPosition(200, 300);
    passwordText.setOutlineColor(Color::Black);
    passwordText.setOutlineThickness(1);

    RectangleShape loginButton = createButton(320, 400, 160, 40, Color(50, 150, 50));
    Text loginButtonText("Login", font, 22);
    loginButtonText.setFillColor(Color::White);
    loginButtonText.setPosition(350, 405);

    RectangleShape unhideButton = createButton(550, 300, 100, 30, Color(50, 150, 50));
    Text unhideButtonText("Unhide", font, 15);
    unhideButtonText.setFillColor(Color::White);
    unhideButtonText.setPosition(570, 305);

    Text feedbackMessage("", font, 20);
    feedbackMessage.setFillColor(Color::Red);
    feedbackMessage.setPosition(200, 450);
    feedbackMessage.setOutlineColor(Color::Black);
    feedbackMessage.setOutlineThickness(1);
    bool showFeedback = false;
    Clock feedbackClock;

    string usernameInput, passwordInput;
    Text usernameField("", font, 20);
    usernameField.setFillColor(Color::White);
    usernameField.setPosition(400, 200);
    usernameField.setOutlineColor(Color::Black);
    usernameField.setOutlineThickness(1);

    Text passwordField("", font, 20);
    passwordField.setFillColor(Color::White);
    passwordField.setPosition(400, 300);
    passwordField.setOutlineColor(Color::Black);
    passwordField.setOutlineThickness(1);

    bool showPassword = false;
    int activeField = 0;

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }

            if (event.type == Event::TextEntered) {
                char enteredChar = static_cast<char>(event.text.unicode);
                if (enteredChar == '\b') {
                    if (activeField == 0 && !usernameInput.empty()) usernameInput.pop_back();
                    if (activeField == 1 && !passwordInput.empty()) passwordInput.pop_back();
                }
                else if (enteredChar >= 32 && enteredChar <= 126 && ((activeField == 0 && usernameInput.length() < 49) || (activeField == 1 && passwordInput.length() < 49))) {
                    if (activeField == 0) usernameInput += enteredChar;
                    if (activeField == 1) passwordInput += enteredChar;
                }
            }

            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Enter) {
                    if (activeField == 1) {
                        if (pl.login(usernameInput, passwordInput)) {
                            currentUser = usernameInput;
                            Player* player = pl.getPlayerByUsername(currentUser);
                            if (player && player->preferredThemeID > 0) {
                                inventory.setBackground(player->preferredThemeID);
                            }
                            if (player && player->preferredSoundID > 0) {
                                inventory.setSound(player->preferredSoundID);
                            }
                            feedbackMessage.setString("Login successful! Welcome.");
                            feedbackMessage.setFillColor(Color::Green);
                            showFeedback = true;
                            feedbackClock.restart();
                            return;
                        }
                        else {
                            feedbackMessage.setString("Error: Invalid username or password.");
                            feedbackMessage.setFillColor(Color::Red);
                            showFeedback = true;
                            feedbackClock.restart();
                        }
                    }
                    activeField = (activeField + 1) % 2;
                }
            }

            if (event.type == Event::MouseButtonPressed) {
                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

                if (loginButtonText.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    if (pl.login(usernameInput, passwordInput)) {
                        currentUser = usernameInput;
                        Player* player = pl.getPlayerByUsername(currentUser);
                        if (player && player->preferredThemeID > 0) {
                            inventory.setBackground(player->preferredThemeID);
                        }
                        if (player && player->preferredSoundID > 0) {
                            inventory.setSound(player->preferredSoundID);
                        }
                        feedbackMessage.setString("Login successful! Welcome.");
                        feedbackMessage.setFillColor(Color::Green);
                        showFeedback = true;
                        feedbackClock.restart();
                        return;
                    }
                    else {
                        feedbackMessage.setString("Error: Invalid username or password.");
                        feedbackMessage.setFillColor(Color::Red);
                        showFeedback = true;
                        feedbackClock.restart();
                    }
                }

                if (unhideButtonText.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    showPassword = !showPassword;
                }
            }
        }

        usernameField.setString(usernameInput);
        passwordField.setString(showPassword ? passwordInput : string(passwordInput.size(), '*'));

        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
        loginButton.setFillColor(loginButtonText.getGlobalBounds().contains(mousePos.x, mousePos.y) ? Color(70, 180, 70) : Color(50, 150, 50));
        unhideButton.setFillColor(unhideButtonText.getGlobalBounds().contains(mousePos.x, mousePos.y) ? Color(70, 180, 70) : Color(50, 150, 50));

        window.clear();
        inventory.drawBackground(window);
        window.draw(usernameText);
        window.draw(passwordText);
        window.draw(loginButton);
        window.draw(loginButtonText);
        window.draw(unhideButton);
        window.draw(unhideButtonText);
        window.draw(usernameField);
        window.draw(passwordField);
        if (showFeedback && feedbackClock.getElapsedTime().asSeconds() < 2) {
            window.draw(feedbackMessage);
        }
        else {
            showFeedback = false;
        }
        window.display();
    }
}

void displayRegistrationPage(PlayerList& pl, RenderWindow& window, Font& font, string& currentUser, Inventory& inventory) {
    window.setTitle("Xonix - Registration");
    const int windowWidth = 800;
    const int windowHeight = 600;
    window.setSize(Vector2u(windowWidth, windowHeight));

    Text usernameText("Username:", font, 20);
    usernameText.setFillColor(Color::White);
    usernameText.setPosition(200, 150);
    usernameText.setOutlineColor(Color::Black);
    usernameText.setOutlineThickness(1);

    Text passwordText("Password:", font, 20);
    passwordText.setFillColor(Color::White);
    passwordText.setPosition(200, 200);
    passwordText.setOutlineColor(Color::Black);
    passwordText.setOutlineThickness(1);

    Text nicknameText("Nickname:", font, 20);
    nicknameText.setFillColor(Color::White);
    nicknameText.setPosition(200, 250);
    nicknameText.setOutlineColor(Color::Black);
    nicknameText.setOutlineThickness(1);

    Text emailText("Email:", font, 20);
    emailText.setFillColor(Color::White);
    emailText.setPosition(200, 300);
    emailText.setOutlineColor(Color::Black);
    emailText.setOutlineThickness(1);

    RectangleShape registerButton = createButton(320, 400, 160, 40, Color(50, 150, 50));
    Text registerButtonText("Register", font, 22);
    registerButtonText.setFillColor(Color::White);
    registerButtonText.setPosition(350, 405);

    RectangleShape unhideButton = createButton(550, 200, 100, 30, Color(50, 150, 50));
    Text unhideButtonText("Unhide", font, 15);
    unhideButtonText.setFillColor(Color::White);
    unhideButtonText.setPosition(570, 205);

    Text feedbackMessage("", font, 20);
    feedbackMessage.setFillColor(Color::Red);
    feedbackMessage.setPosition(200, 450);
    feedbackMessage.setOutlineColor(Color::Black);
    feedbackMessage.setOutlineThickness(1);
    bool showFeedback = false;
    Clock feedbackClock;

    string usernameInput, passwordInput, nicknameInput, emailInput;
    Text usernameField("", font, 20);
    usernameField.setFillColor(Color::White);
    usernameField.setPosition(400, 150);
    usernameField.setOutlineColor(Color::Black);
    usernameField.setOutlineThickness(1);

    Text passwordField("", font, 20);
    passwordField.setFillColor(Color::White);
    passwordField.setPosition(400, 200);
    passwordField.setOutlineColor(Color::Black);
    passwordField.setOutlineThickness(1);

    Text nicknameField("", font, 20);
    nicknameField.setFillColor(Color::White);
    nicknameField.setPosition(400, 250);
    nicknameField.setOutlineColor(Color::Black);
    nicknameField.setOutlineThickness(1);

    Text emailField("", font, 20);
    emailField.setFillColor(Color::White);
    emailField.setPosition(400, 300);
    emailField.setOutlineColor(Color::Black);
    emailField.setOutlineThickness(1);

    int activeField = 0;
    bool showPassword = false;

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }

            if (event.type == Event::TextEntered) {
                char enteredChar = static_cast<char>(event.text.unicode);
                if (enteredChar == '\b') {
                    if (activeField == 0 && !usernameInput.empty()) usernameInput.pop_back();
                    if (activeField == 1 && !passwordInput.empty()) passwordInput.pop_back();
                    if (activeField == 2 && !nicknameInput.empty()) nicknameInput.pop_back();
                    if (activeField == 3 && !emailInput.empty()) emailInput.pop_back();
                }
                else if (enteredChar >= 32 && enteredChar <= 126) {
                    if (activeField == 0 && usernameInput.length() < 49) usernameInput += enteredChar;
                    if (activeField == 1 && passwordInput.length() < 49) passwordInput += enteredChar;
                    if (activeField == 2 && nicknameInput.length() < 49) nicknameInput += enteredChar;
                    if (activeField == 3 && emailInput.length() < 49) emailInput += enteredChar;
                }
            }

            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Enter) {
                    if (activeField == 3) {
                        if (!usernameInput.empty() && !passwordInput.empty() && !nicknameInput.empty() && !emailInput.empty()) {
                            if (pl.usernameExists(usernameInput)) {
                                feedbackMessage.setString("Error: Username already taken.");
                                feedbackMessage.setFillColor(Color::Red);
                                showFeedback = true;
                                feedbackClock.restart();
                            }
                            else {
                                pl.registerPlayer(usernameInput, passwordInput, nicknameInput, emailInput);
                                currentUser = usernameInput;
                                feedbackMessage.setString("Registration successful!");
                                feedbackMessage.setFillColor(Color::Green);
                                showFeedback = true;
                                feedbackClock.restart();
                                return;
                            }
                        }
                        else {
                            feedbackMessage.setString("Error: All fields must be filled out.");
                            feedbackMessage.setFillColor(Color::Red);
                            showFeedback = true;
                            feedbackClock.restart();
                        }
                    }
                    activeField = (activeField + 1) % 4;
                }
            }

            if (event.type == Event::MouseButtonPressed) {
                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

                if (registerButtonText.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    if (!usernameInput.empty() && !passwordInput.empty() && !nicknameInput.empty() && !emailInput.empty()) {
                        if (pl.usernameExists(usernameInput)) {
                            feedbackMessage.setString("Error: Username already taken.");
                            feedbackMessage.setFillColor(Color::Red);
                            showFeedback = true;
                            feedbackClock.restart();
                        }
                        else {
                            pl.registerPlayer(usernameInput, passwordInput, nicknameInput, emailInput);
                            currentUser = usernameInput;
                            feedbackMessage.setString("Registration successful!");
                            feedbackMessage.setFillColor(Color::Green);
                            showFeedback = true;
                            feedbackClock.restart();
                            return;
                        }
                    }
                    else {
                        feedbackMessage.setString("Error: All fields must be filled out.");
                        feedbackMessage.setFillColor(Color::Red);
                        showFeedback = true;
                        feedbackClock.restart();
                    }
                }

                if (unhideButtonText.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    showPassword = !showPassword;
                }
            }
        }

        usernameField.setString(usernameInput);
        passwordField.setString(showPassword ? passwordInput : string(passwordInput.size(), '*'));
        nicknameField.setString(nicknameInput);
        emailField.setString(emailInput);

        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
        registerButton.setFillColor(registerButtonText.getGlobalBounds().contains(mousePos.x, mousePos.y) ? Color(70, 180, 70) : Color(50, 150, 50));
        unhideButton.setFillColor(unhideButtonText.getGlobalBounds().contains(mousePos.x, mousePos.y) ? Color(70, 180, 70) : Color(50, 150, 50));

        window.clear();
        inventory.drawBackground(window);
        window.draw(usernameText);
        window.draw(passwordText);
        window.draw(nicknameText);
        window.draw(emailText);
        window.draw(registerButton);
        window.draw(registerButtonText);
        window.draw(unhideButton);
        window.draw(unhideButtonText);
        window.draw(usernameField);
        window.draw(passwordField);
        window.draw(nicknameField);
        window.draw(emailField);
        if (showFeedback && feedbackClock.getElapsedTime().asSeconds() < 2) {
            window.draw(feedbackMessage);
        }
        else {
            showFeedback = false;
        }
        window.display();
    }
}

void displayMenu(PlayerList& pl, RenderWindow& window, Font& font, const string& currentUser, Inventory& inventory) {
    window.setTitle("Xonix - Game Menu");
    const int windowWidth = 800;
    const int windowHeight = 600;
    window.setSize(Vector2u(windowWidth, windowHeight));

    Text title("Game Menu", font, 30);
    title.setFillColor(Color::White);
    title.setPosition(320, 50);
    title.setOutlineColor(Color::Black);
    title.setOutlineThickness(1);

    RectangleShape buttons[6];
    Text buttonTexts[6];
    const char* buttonLabels[6] = { "New Game", "Resume Game", "Multiplayer", "Profile", "Leaderboard", "Exit" };
    bool hasSaveFile = saveFileExists(currentUser.c_str());
    for (int i = 0; i < 6; ++i) {
        buttons[i] = createButton(320, 150 + i * 60, 160, 40, Color(50, 150, 50));
        buttonTexts[i].setFont(font);
        buttonTexts[i].setString(buttonLabels[i]);
        buttonTexts[i].setCharacterSize(22);
        buttonTexts[i].setFillColor(Color::White);
        buttonTexts[i].setPosition(350, 155 + i * 60);
    }

    Text feedbackMessage("", font, 20);
    feedbackMessage.setFillColor(Color::Red);
    feedbackMessage.setPosition(200, 500);
    feedbackMessage.setOutlineColor(Color::Black);
    feedbackMessage.setOutlineThickness(1);
    bool showFeedback = false;
    Clock feedbackClock;

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }

            if (event.type == Event::MouseButtonPressed) {
                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

                if (buttonTexts[0].getGlobalBounds().contains(mousePos.x, mousePos.y)) { // New Game
                    playXonixGame(pl, window, font, currentUser, inventory, nullptr);
                    hasSaveFile = saveFileExists(currentUser.c_str());
                }
                else if (buttonTexts[1].getGlobalBounds().contains(mousePos.x, mousePos.y) && hasSaveFile) { // Resume Game
                    SaveGameState state;
                    if (loadGame(currentUser, state)) {
                        playXonixGame(pl, window, font, currentUser, inventory, &state);
                        hasSaveFile = saveFileExists(currentUser.c_str());
                    }
                    else {
                        feedbackMessage.setString("Invalid Save File");
                        feedbackMessage.setFillColor(Color::Red);
                        showFeedback = true;
                        feedbackClock.restart();
                    }
                }
                else if (buttonTexts[2].getGlobalBounds().contains(mousePos.x, mousePos.y)) { // Multiplayer
                    string player2;
                    if (displayMultiplayerLogin(pl, window, font, currentUser, player2, inventory)) {
                        playMultiplayerXonixGame(pl, window, font, currentUser, player2, inventory);
                    }
                }
                else if (buttonTexts[3].getGlobalBounds().contains(mousePos.x, mousePos.y)) { // Profile
                    displayProfilePage(pl, window, font, currentUser, inventory);
                }
                else if (buttonTexts[4].getGlobalBounds().contains(mousePos.x, mousePos.y)) { // Leaderboard
                    displayLeaderboardPage(pl, window, font, currentUser, inventory);
                }
                else if (buttonTexts[5].getGlobalBounds().contains(mousePos.x, mousePos.y)) { // Exit
                    return;
                }
            }
        }

        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
        for (int i = 0; i < 6; ++i) {
            if (i == 1 && !hasSaveFile) {
                buttons[i].setFillColor(Color(100, 100, 100)); // Grayed out
                buttonTexts[i].setFillColor(Color(150, 150, 150));
            }
            else {
                buttons[i].setFillColor(buttonTexts[i].getGlobalBounds().contains(mousePos.x, mousePos.y) ? Color(70, 180, 70) : Color(50, 150, 50));
                buttonTexts[i].setFillColor(Color::White);
            }
        }

        window.clear();
        inventory.drawBackground(window);
        window.draw(title);
        for (int i = 0; i < 6; ++i) {
            window.draw(buttons[i]);
            window.draw(buttonTexts[i]);
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

void displayPlaceholderPage(PlayerList& pl, RenderWindow& window, Font& font, const string& currentUser, Inventory& inventory) {
    window.setTitle("Feature Not Implemented");
    const int windowWidth = 800;
    const int windowHeight = 600;
    window.setSize(Vector2u(windowWidth, windowHeight));

    Text title("Feature Not Implemented", font, 30);
    title.setFillColor(Color::White);
    title.setPosition(250, 200);
    title.setOutlineColor(Color::Black);
    title.setOutlineThickness(1);

    RectangleShape backButton = createButton(320, 400, 160, 40, Color(50, 150, 50));
    Text backButtonText("Back", font, 22);
    backButtonText.setFillColor(Color::White);
    backButtonText.setPosition(350, 405);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }

            if (event.type == Event::MouseButtonPressed) {
                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

                if (backButtonText.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    return;
                }
            }
        }

        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
        backButton.setFillColor(backButtonText.getGlobalBounds().contains(mousePos.x, mousePos.y) ? Color(70, 180, 70) : Color(50, 150, 50));

        window.clear();
        inventory.drawBackground(window);
        window.draw(title);
        window.draw(backButton);
        window.draw(backButtonText);
        window.display();
    }
}

void displayProfilePage(PlayerList& pl, RenderWindow& window, Font& font, const string& currentUser, Inventory& inventory) {
    window.setTitle("Xonix - Profile");
    const int windowWidth = 800;
    const int windowHeight = 600;
    window.setSize(Vector2u(windowWidth, windowHeight));

    Text title("Player Profile", font, 30);
    title.setFillColor(Color::White);
    title.setPosition(300, 50);
    title.setOutlineColor(Color::Black);
    title.setOutlineThickness(1);

    Text statsText(pl.getPlayerStats(currentUser), font, 20);
    statsText.setFillColor(Color::White);
    statsText.setPosition(200, 150);
    statsText.setOutlineColor(Color::Black);
    statsText.setOutlineThickness(1);

    RectangleShape backButton = createButton(320, 400, 160, 40, Color(50, 150, 50));
    Text backButtonText("Back", font, 22);
    backButtonText.setFillColor(Color::White);
    backButtonText.setPosition(350, 405);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }

            if (event.type == Event::MouseButtonPressed) {
                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

                if (backButtonText.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    return;
                }
            }
        }

        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
        backButton.setFillColor(backButtonText.getGlobalBounds().contains(mousePos.x, mousePos.y) ? Color(70, 180, 70) : Color(50, 150, 50));

        window.clear();
        inventory.drawBackground(window);
        window.draw(title);
        window.draw(statsText);
        window.draw(backButton);
        window.draw(backButtonText);
        window.display();
    }
}

void displayLeaderboardPage(PlayerList& pl, RenderWindow& window, Font& font, const string& currentUser, Inventory& inventory) {
    window.setTitle("Xonix - Leaderboard");
    const int windowWidth = 800;
    const int windowHeight = 600;
    window.setSize(Vector2u(windowWidth, windowHeight));

    Text title("Leaderboard", font, 30);
    title.setFillColor(Color::White);
    title.setPosition(300, 50);
    title.setOutlineColor(Color::Black);
    title.setOutlineThickness(1);

    Text leaderboardText(pl.getLeaderboard(), font, 20);
    leaderboardText.setFillColor(Color::White);
    leaderboardText.setPosition(200, 150);
    leaderboardText.setOutlineColor(Color::Black);
    leaderboardText.setOutlineThickness(1);

    RectangleShape backButton = createButton(320, 400, 160, 40, Color(50, 150, 50));
    Text backButtonText("Back", font, 22);
    backButtonText.setFillColor(Color::White);
    backButtonText.setPosition(350, 405);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }

            if (event.type == Event::MouseButtonPressed) {
                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

                if (backButtonText.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    return;
                }
            }
        }

        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
        backButton.setFillColor(backButtonText.getGlobalBounds().contains(mousePos.x, mousePos.y) ? Color(70, 180, 70) : Color(50, 150, 50));

        window.clear();
        inventory.drawBackground(window);
        window.draw(title);
        window.draw(leaderboardText);
        window.draw(backButton);
        window.draw(backButtonText);
        window.display();
    }
}

bool displayMultiplayerLogin(PlayerList& pl, RenderWindow& window, Font& font, const string& player1, string& player2, Inventory& inventory) {
    window.setTitle("Xonix - Multiplayer Login");
    const int windowWidth = 800;
    const int windowHeight = 600;
    window.setSize(Vector2u(windowWidth, windowHeight));

    Text title("Enter Player 2 Username", font, 30);
    title.setFillColor(Color::White);
    title.setPosition(250, 50);
    title.setOutlineColor(Color::Black);
    title.setOutlineThickness(1);

    Text usernameText("Username:", font, 20);
    usernameText.setFillColor(Color::White);
    usernameText.setPosition(200, 200);
    usernameText.setOutlineColor(Color::Black);
    usernameText.setOutlineThickness(1);

    RectangleShape loginButton = createButton(320, 400, 160, 40, Color(50, 150, 50));
    Text loginButtonText("Start Game", font, 22);
    loginButtonText.setFillColor(Color::White);
    loginButtonText.setPosition(350, 405);

    RectangleShape backButton = createButton(320, 450, 160, 40, Color(50, 150, 50));
    Text backButtonText("Back", font, 22);
    backButtonText.setFillColor(Color::White);
    backButtonText.setPosition(350, 455);

    Text feedbackMessage("", font, 20);
    feedbackMessage.setFillColor(Color::Red);
    feedbackMessage.setPosition(200, 350);
    feedbackMessage.setOutlineColor(Color::Black);
    feedbackMessage.setOutlineThickness(1);
    bool showFeedback = false;
    Clock feedbackClock;

    string usernameInput;
    Text usernameField("", font, 20);
    usernameField.setFillColor(Color::White);
    usernameField.setPosition(400, 200);
    usernameField.setOutlineColor(Color::Black);
    usernameField.setOutlineThickness(1);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
                return false;
            }

            if (event.type == Event::TextEntered) {
                char enteredChar = static_cast<char>(event.text.unicode);
                if (enteredChar == '\b' && !usernameInput.empty()) {
                    usernameInput.pop_back();
                }
                else if (enteredChar >= 32 && enteredChar <= 126 && usernameInput.length() < 49) {
                    usernameInput += enteredChar;
                }
            }

            if (event.type == Event::MouseButtonPressed) {
                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

                if (loginButtonText.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    if (usernameInput.empty()) {
                        feedbackMessage.setString("Error: Username cannot be empty.");
                        feedbackMessage.setFillColor(Color::Red);
                        showFeedback = true;
                        feedbackClock.restart();
                    }
                    else if (!pl.usernameExists(usernameInput)) {
                        feedbackMessage.setString("Error: Username not found.");
                        feedbackMessage.setFillColor(Color::Red);
                        showFeedback = true;
                        feedbackClock.restart();
                    }
                    else if (usernameInput == player1) {
                        feedbackMessage.setString("Error: Cannot play against yourself.");
                        feedbackMessage.setFillColor(Color::Red);
                        showFeedback = true;
                        feedbackClock.restart();
                    }
                    else {
                        player2 = usernameInput;
                        return true;
                    }
                }

                if (backButtonText.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    return false;
                }
            }
        }

        usernameField.setString(usernameInput);

        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
        loginButton.setFillColor(loginButtonText.getGlobalBounds().contains(mousePos.x, mousePos.y) ? Color(70, 180, 70) : Color(50, 150, 50));
        backButton.setFillColor(backButtonText.getGlobalBounds().contains(mousePos.x, mousePos.y) ? Color(70, 180, 70) : Color(50, 150, 50));

        window.clear();
        inventory.drawBackground(window);
        window.draw(title);
        window.draw(usernameText);
        window.draw(usernameField);
        window.draw(loginButton);
        window.draw(loginButtonText);
        window.draw(backButton);
        window.draw(backButtonText);
        if (showFeedback && feedbackClock.getElapsedTime().asSeconds() < 2) {
            window.draw(feedbackMessage);
        }
        else {
            showFeedback = false;
        }
        window.display();
    }
    return false;
}