#ifndef INVENTORY_H
#define INVENTORY_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Inventory {
private:
    std::vector<sf::Texture> backgroundTextures;
    std::vector<std::string> backgroundNames;
    sf::Sprite backgroundSprite;
    int currentBackgroundIndex;
    static const std::string BACKGROUND_PATH;

public:
    Inventory();
    bool loadBackgrounds();
    void setBackground(int index);
    sf::Sprite& getBackgroundSprite();
    void drawBackground(sf::RenderWindow& window);
    const std::vector<std::string>& getBackgroundNames() const;
    int getCurrentBackgroundIndex() const;
    int getBackgroundCount() const;
};

#endif