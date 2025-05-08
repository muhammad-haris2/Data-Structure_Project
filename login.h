#ifndef LOGIN_H
#define LOGIN_H

#include <SFML/Graphics.hpp>
#include "player.h"
#include "inventory.h"

void displayMainMenu(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, std::string& currentUser, Inventory& inventory);
void displayLoginPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, std::string& currentUser, Inventory& inventory);
void displayRegistrationPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, std::string& currentUser, Inventory& inventory);
void displayPlaceholderPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& currentUser, Inventory& inventory);
void displayProfilePage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& currentUser, Inventory& inventory);
void displayLeaderboardPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& currentUser, Inventory& inventory);
bool displayMultiplayerLogin(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& player1, std::string& player2, Inventory& inventory);

#endif