#ifndef FRIEND_H
#define FRIEND_H

#include <SFML/Graphics.hpp>
#include "player.h"
#include "inventory.h"

bool displayFriendPage(PlayerList& pl, sf::RenderWindow& window, sf::Font& font, const std::string& currentUser, Inventory& inventory);

#endif