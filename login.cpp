#include "login.h"
#include "menu.h"
#include <iostream>

using namespace std;

void displayMainMenu(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, string& currentUser, Inventory& inventory) {
    window.setTitle("Login and Registration System");

    sf::Text title("Login and Registration System", font, 30);
    title.setPosition(200, 50);

    sf::Text loginButton("Login", font, 20);
    loginButton.setPosition(350, 200);

    sf::Text registerButton("Register", font, 20);
    registerButton.setPosition(350, 300);

    sf::Text exitButton("Exit", font, 20);
    exitButton.setPosition(350, 400);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (loginButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    displayLoginPage(pl, window, font, currentUser, inventory);
                    if (!currentUser.empty()) {
                        displayMenu(pl, window, font, currentUser, inventory);
                        currentUser.clear();
                    }
                }

                if (registerButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    displayRegistrationPage(pl, window, font, currentUser, inventory);
                    if (!currentUser.empty()) {
                        displayMenu(pl, window, font, currentUser, inventory);
                        currentUser.clear();
                    }
                }

                if (exitButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    window.close();
                }
            }
        }

        window.clear();
        inventory.drawBackground(window); // Draw background
        window.draw(title);
        window.draw(loginButton);
        window.draw(registerButton);
        window.draw(exitButton);
        window.display();
    }
}

void displayLoginPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, string& currentUser, Inventory& inventory) {
    window.setTitle("Login and Registration System");

    sf::Text usernameText("Username:", font, 20);
    usernameText.setPosition(200, 200);

    sf::Text passwordText("Password:", font, 20);
    passwordText.setPosition(200, 300);

    sf::Text loginButton("Login", font, 20);
    loginButton.setPosition(350, 400);

    sf::Text unhideButton("Unhide", font, 15);
    unhideButton.setPosition(600, 300);

    sf::Text errorMessage("", font, 20);
    errorMessage.setFillColor(sf::Color::Red);
    errorMessage.setPosition(200, 450);

    string usernameInput, passwordInput;
    sf::Text usernameField("", font, 20);
    usernameField.setPosition(400, 200);

    sf::Text passwordField("", font, 20);
    passwordField.setPosition(400, 300);

    bool showPassword = false;
    int activeField = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::TextEntered) {
                char enteredChar = static_cast<char>(event.text.unicode);
                if (enteredChar == '\b') {
                    if (activeField == 0 && !usernameInput.empty()) usernameInput.pop_back();
                    if (activeField == 1 && !passwordInput.empty()) passwordInput.pop_back();
                }
                else if (enteredChar >= 32 && enteredChar <= 126) {
                    if (activeField == 0) usernameInput += enteredChar;
                    if (activeField == 1) passwordInput += enteredChar;
                }
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Enter) {
                    activeField = (activeField + 1) % 2;
                }
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (loginButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    if (pl.login(usernameInput, passwordInput)) {
                        currentUser = usernameInput;
                        errorMessage.setString("Login successful! Welcome.");
                        errorMessage.setFillColor(sf::Color::Green);
                        return;
                    }
                    else {
                        errorMessage.setString("Error: Invalid username or password.");
                    }
                }

                if (unhideButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    showPassword = !showPassword;
                }
            }
        }

        usernameField.setString(usernameInput);
        passwordField.setString(showPassword ? passwordInput : string(passwordInput.size(), '*'));

        window.clear();
        inventory.drawBackground(window); // Draw background
        window.draw(usernameText);
        window.draw(passwordText);
        window.draw(loginButton);
        window.draw(unhideButton);
        window.draw(usernameField);
        window.draw(passwordField);
        window.draw(errorMessage);
        window.display();
    }
}

void displayRegistrationPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, string& currentUser, Inventory& inventory) {
    window.setTitle("Login and Registration System");

    sf::Text usernameText("Username:", font, 20);
    usernameText.setPosition(200, 150);

    sf::Text passwordText("Password:", font, 20);
    passwordText.setPosition(200, 200);

    sf::Text nicknameText("Nickname:", font, 20);
    nicknameText.setPosition(200, 250);

    sf::Text emailText("Email:", font, 20);
    emailText.setPosition(200, 300);

    sf::Text registerButton("Register", font, 20);
    registerButton.setPosition(350, 400);

    sf::Text unhideButton("Unhide", font, 15);
    unhideButton.setPosition(600, 200);

    sf::Text errorMessage("", font, 20);
    errorMessage.setFillColor(sf::Color::Red);
    errorMessage.setPosition(200, 450);

    string usernameInput, passwordInput, nicknameInput, emailInput;
    sf::Text usernameField("", font, 20);
    usernameField.setPosition(400, 150);

    sf::Text passwordField("", font, 20);
    passwordField.setPosition(400, 200);

    sf::Text nicknameField("", font, 20);
    nicknameField.setPosition(400, 250);

    sf::Text emailField("", font, 20);
    emailField.setPosition(400, 300);

    int activeField = 0;
    bool showPassword = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::TextEntered) {
                char enteredChar = static_cast<char>(event.text.unicode);
                if (enteredChar == '\b') {
                    if (activeField == 0 && !usernameInput.empty()) usernameInput.pop_back();
                    if (activeField == 1 && !passwordInput.empty()) passwordInput.pop_back();
                    if (activeField == 2 && !nicknameInput.empty()) nicknameInput.pop_back();
                    if (activeField == 3 && !emailInput.empty()) emailInput.pop_back();
                }
                else if (enteredChar >= 32 && enteredChar <= 126) {
                    if (activeField == 0) usernameInput += enteredChar;
                    if (activeField == 1) passwordInput += enteredChar;
                    if (activeField == 2) nicknameInput += enteredChar;
                    if (activeField == 3) emailInput += enteredChar;
                }
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Enter) {
                    activeField = (activeField + 1) % 4;
                }
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (registerButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    if (!usernameInput.empty() && !passwordInput.empty() && !nicknameInput.empty() && !emailInput.empty()) {
                        if (pl.usernameExists(usernameInput)) {
                            errorMessage.setString("Error: Username already taken.");
                        }
                        else {
                            pl.registerPlayer(usernameInput, passwordInput, nicknameInput, emailInput);
                            currentUser = usernameInput;
                            errorMessage.setString("Registration successful!");
                            errorMessage.setFillColor(sf::Color::Green);
                            return;
                        }
                    }
                    else {
                        errorMessage.setString("Error: All fields must be filled out.");
                    }
                }

                if (unhideButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    showPassword = !showPassword;
                }
            }
        }

        usernameField.setString(usernameInput);
        passwordField.setString(showPassword ? passwordInput : string(passwordInput.size(), '*'));
        nicknameField.setString(nicknameInput);
        emailField.setString(emailInput);

        window.clear();
        inventory.drawBackground(window); // Draw background
        window.draw(usernameText);
        window.draw(passwordText);
        window.draw(nicknameText);
        window.draw(emailText);
        window.draw(registerButton);
        window.draw(unhideButton);
        window.draw(usernameField);
        window.draw(passwordField);
        window.draw(nicknameField);
        window.draw(emailField);
        window.draw(errorMessage);
        window.display();
    }
}

void displayPlaceholderPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const string& currentUser, Inventory& inventory) {
    window.setTitle("Feature Not Implemented");

    sf::Text title("Feature Not Implemented", font, 30);
    title.setPosition(250, 200);

    sf::Text backButton("Back", font, 20);
    backButton.setPosition(350, 400);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (backButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    return;
                }
            }
        }

        window.clear();
        inventory.drawBackground(window); // Draw background
        window.draw(title);
        window.draw(backButton);
        window.display();
    }
}

void displayProfilePage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const string& currentUser, Inventory& inventory) {
    window.setTitle("Profile");

    sf::Text title("Player Profile", font, 30);
    title.setPosition(300, 50);

    sf::Text statsText(pl.getPlayerStats(currentUser), font, 20);
    statsText.setPosition(200, 150);

    sf::Text backButton("Back", font, 20);
    backButton.setPosition(350, 400);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (backButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    return;
                }
            }
        }

        window.clear();
        inventory.drawBackground(window); // Draw background
        window.draw(title);
        window.draw(statsText);
        window.draw(backButton);
        window.display();
    }
}

void displayLeaderboardPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const string& currentUser, Inventory& inventory) {
    window.setTitle("Leaderboard");

    sf::Text title("Leaderboard", font, 30);
    title.setPosition(300, 50);

    sf::Text leaderboardText(pl.getLeaderboard(), font, 20);
    leaderboardText.setPosition(200, 150);

    sf::Text backButton("Back", font, 20);
    backButton.setPosition(350, 400);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (backButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    return;
                }
            }
        }

        window.clear();
        inventory.drawBackground(window); // Draw background
        window.draw(title);
        window.draw(leaderboardText);
        window.draw(backButton);
        window.display();
    }
}

bool displayMultiplayerLogin(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const string& player1, string& player2, Inventory& inventory) {
    window.setTitle("Multiplayer Login");

    sf::Text title("Enter Player 2 Username", font, 30);
    title.setPosition(250, 50);

    sf::Text usernameText("Username:", font, 20);
    usernameText.setPosition(200, 200);

    sf::Text loginButton("Start Game", font, 20);
    loginButton.setPosition(350, 400);

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

                if (loginButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    if (usernameInput.empty()) {
                        errorMessage.setString("Error: Username cannot be empty.");
                    }
                    else if (!pl.usernameExists(usernameInput)) {
                        errorMessage.setString("Error: Username not found.");
                    }
                    else if (usernameInput == player1) {
                        errorMessage.setString("Error: Cannot play against yourself.");
                    }
                    else {
                        player2 = usernameInput;
                        return true;
                    }
                }

                if (backButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    return false;
                }
            }
        }

        usernameField.setString(usernameInput);

        window.clear();
        inventory.drawBackground(window); // Draw background
        window.draw(title);
        window.draw(usernameText);
        window.draw(usernameField);
        window.draw(loginButton);
        window.draw(backButton);
        window.draw(errorMessage);
        window.display();
    }
    return false;
}