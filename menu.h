#ifndef MENU_H
#define MENU_H
#include "player.h"
#include "matchmaking.h"
#include "inventory.h"
#include <SFML/Graphics.hpp>

void displayMenu(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, std::string& currentUser, Inventory& inventory);
bool displayMatchmakingPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& currentUser, MatchmakingQueue& queue, Inventory& inventory);
bool displayMatchTransitionPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& player1, const std::string& player2, Inventory& inventory);
bool displayManualMatchPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& currentUser, Inventory& inventory);
bool displayQueuePage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& currentUser, MatchmakingQueue& queue, Inventory& inventory);
bool displayInventoryPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& currentUser, Inventory& inventory);

#endif