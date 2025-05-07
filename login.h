#ifndef LOGIN_H
#define LOGIN_H

#include <SFML/Graphics.hpp>
#include "player.h"

void displayMainMenu(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, std::string& currentUser);
void displayLoginPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, std::string& currentUser);
void displayRegistrationPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, std::string& currentUser);
void displayPlaceholderPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& currentUser);
void displayProfilePage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& currentUser);
void displayLeaderboardPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& currentUser); // New
bool displayMultiplayerLogin(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& player1, std::string& player2);

#endif