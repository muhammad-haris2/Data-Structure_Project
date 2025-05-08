#include <SFML/Graphics.hpp>
#include "login.h"
#include "inventory.h"
#include <iostream>

using namespace std;
using namespace sf;

int main() {
    RenderWindow window(VideoMode(800, 600), "Login and Registration System");
    window.setFramerateLimit(60);

    Font font;
    if (!font.loadFromFile("fonts/arial.ttf")) {
        cout << "Error: Could not load font.\n";
        return 1;
    }

    // Initialize Inventory
    Inventory inventory;

    if (!inventory.loadBackgrounds()) {
        cout << "Error: Failed to load backgrounds.\n";
        return 1;
    }

    PlayerList pl;
    string currentUser;

    // Start with main menu, passing Inventory
    displayMainMenu(pl, window, font, currentUser, inventory);

    return 0;
}