#include "inventory.h"
#include <iostream>

const std::string Inventory::BACKGROUND_PATH = "D:/part2/images/backgrounds/";

Inventory::Inventory() : currentBackgroundIndex(0) {
    backgroundSprite.setPosition(0, 0);
}

bool Inventory::loadBackgrounds() {
    std::vector<std::string> filenames = {
        "normal.jpg", // Default background
        "light.jpg",
        "cool.jpg",
        "city.jpg",
        "robotic.jpg"
    };
    backgroundNames = filenames;

    for (const auto& filename : filenames) {
        sf::Texture texture;
        if (!texture.loadFromFile(BACKGROUND_PATH + filename)) {
            std::cout << "Error: Could not load background " << filename << "\n";
            return false;
        }
        backgroundTextures.push_back(texture);
    }

    // Set default background (normal.jpg)
    if (!backgroundTextures.empty()) {
        backgroundSprite.setTexture(backgroundTextures[0]);
        // Scale sprite to fit window (assuming 800x600 window)
        backgroundSprite.setScale(
            800.0f / backgroundTextures[0].getSize().x,
            600.0f / backgroundTextures[0].getSize().y
        );
    }
    return true;
}

void Inventory::setBackground(int index) {
    if (index >= 0 && index < static_cast<int>(backgroundTextures.size())) {
        currentBackgroundIndex = index;
        backgroundSprite.setTexture(backgroundTextures[index]);
        // Reapply scaling
        backgroundSprite.setScale(
            800.0f / backgroundTextures[index].getSize().x,
            600.0f / backgroundTextures[index].getSize().y
        );
    }
}

sf::Sprite& Inventory::getBackgroundSprite() {
    return backgroundSprite;
}

void Inventory::drawBackground(sf::RenderWindow& window) {
    window.draw(backgroundSprite);
}

const std::vector<std::string>& Inventory::getBackgroundNames() const {
    return backgroundNames;
}

int Inventory::getCurrentBackgroundIndex() const {
    return currentBackgroundIndex;
}

int Inventory::getBackgroundCount() const {
    return static_cast<int>(backgroundTextures.size());
}