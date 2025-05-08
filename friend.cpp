#include "friend.h"

using namespace std;

bool displayFriendPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const string& currentUser, Inventory& inventory) {
    window.setTitle("Friend System");

    sf::Text title("Friend Management", font, 30);
    title.setPosition(300, 50);

    sf::Text sendRequestText("Send Friend Request:", font, 20);
    sendRequestText.setPosition(200, 150);

    sf::Text usernameField("", font, 20);
    usernameField.setPosition(400, 150);

    sf::Text sendButton("Send", font, 20);
    sendButton.setPosition(600, 150);

    sf::Text pendingText("Pending Requests:", font, 20);
    pendingText.setPosition(200, 200);

    const int MAX_REQUESTS = 50;
    sf::Text pendingRequestTexts[MAX_REQUESTS];
    int numPendingRequests = 0;
    string selectedPending;

    sf::Text acceptButton("Accept", font, 20);
    acceptButton.setPosition(200, 350);
    sf::Text rejectButton("Reject", font, 20);
    rejectButton.setPosition(300, 350);

    sf::Text backButton("Back", font, 20);
    backButton.setPosition(400, 350);

    sf::Text friendsText("Friends List:", font, 20);
    friendsText.setPosition(200, 400);

    sf::Text friendsList("", font, 15);
    friendsList.setPosition(200, 430);

    sf::Text errorMessage("", font, 20);
    errorMessage.setFillColor(sf::Color::Red);
    errorMessage.setPosition(200, 550);

    string inputUsername;

    auto updatePendingRequests = [&]() {
        numPendingRequests = 0;
        string pending = pl.getPendingRequests(currentUser);
        size_t pos = 0;
        int y = 230;
        while ((pos = pending.find('\n')) != string::npos && numPendingRequests < MAX_REQUESTS) {
            string line = pending.substr(0, pos);
            if (line.find("Pending requests for") == string::npos && line != "No pending requests.") {
                pendingRequestTexts[numPendingRequests].setFont(font);
                pendingRequestTexts[numPendingRequests].setString(line);
                pendingRequestTexts[numPendingRequests].setCharacterSize(15);
                pendingRequestTexts[numPendingRequests].setPosition(200, static_cast<float>(y));
                numPendingRequests++;
                y += 20;
            }
            pending = pending.substr(pos + 1);

        }
        };

    updatePendingRequests();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return false;
            }

            if (event.type == sf::Event::TextEntered) {
                char enteredChar = static_cast<char>(event.text.unicode);
                if (enteredChar == '\b' && !inputUsername.empty()) {
                    inputUsername.pop_back();
                }
                else if (enteredChar >= 32 && enteredChar <= 126) {
                    inputUsername += enteredChar;
                }
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (sendButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    if (inputUsername.empty()) {
                        errorMessage.setString("Error: Enter a username.");
                    }
                    else if (pl.sendFriendRequest(currentUser, inputUsername)) {
                        errorMessage.setString("Friend request sent!");
                        errorMessage.setFillColor(sf::Color::Green);
                        inputUsername.clear();
                        updatePendingRequests();
                    }
                    else {
                        errorMessage.setString("Error: Invalid request (user not found, already friends, or pending).");
                    }
                }

                if (acceptButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)) && !selectedPending.empty()) {
                    if (pl.acceptFriendRequest(currentUser, selectedPending)) {
                        errorMessage.setString("Friend request accepted!");
                        errorMessage.setFillColor(sf::Color::Green);
                        selectedPending.clear();
                        updatePendingRequests();
                        friendsList.setString(pl.getFriendsList(currentUser));
                    }
                    else {
                        errorMessage.setString("Error: Unable to accept request.");
                    }
                }

                if (rejectButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)) && !selectedPending.empty()) {
                    if (pl.rejectFriendRequest(currentUser, selectedPending)) {
                        errorMessage.setString("Friend request rejected.");
                        errorMessage.setFillColor(sf::Color::Red);
                        selectedPending.clear();
                        updatePendingRequests();
                    }
                    else {
                        errorMessage.setString("Error: Unable to reject request.");
                    }
                }

                if (backButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    return true;
                }

                for (int i = 0; i < numPendingRequests; i++) {
                    if (pendingRequestTexts[i].getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                        selectedPending = pendingRequestTexts[i].getString();
                        errorMessage.setString("Selected: " + selectedPending);
                        errorMessage.setFillColor(sf::Color::Blue);
                    }
                }
            }
        }

        usernameField.setString(inputUsername);
        friendsList.setString(pl.getFriendsList(currentUser));

        window.clear();
        inventory.drawBackground(window); // Draw background
        window.draw(title);
        window.draw(sendRequestText);
        window.draw(usernameField);
        window.draw(sendButton);
        window.draw(pendingText);
        for (int i = 0; i < numPendingRequests; i++) {
            window.draw(pendingRequestTexts[i]);
        }
        window.draw(acceptButton);
        window.draw(rejectButton);
        window.draw(backButton);
        window.draw(friendsText);
        window.draw(friendsList);
        window.draw(errorMessage);
        window.display();
    }
    return false;
}