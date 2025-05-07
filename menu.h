#ifndef MENU_H
#define MENU_H
#include "player.h"
#include "matchmaking.h"
#include <SFML/Graphics.hpp>

void displayMenu(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, std::string& currentUser);
bool displayMatchmakingPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& currentUser, MatchmakingQueue& queue);
bool displayMatchTransitionPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& player1, const std::string& player2);
bool displayManualMatchPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& currentUser);
bool displayQueuePage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& currentUser, MatchmakingQueue& queue);

#endif