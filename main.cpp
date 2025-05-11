#include "game.h"
#include "menu.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include "login.h"

using namespace std;
using namespace sf;

int main() {
    // Initialize SFML window
    RenderWindow window(VideoMode(800, 600), "Xonix");
    window.setFramerateLimit(60); // Smooth rendering, consistent with game.cpp

    // Load font
    Font font;
    if (!font.loadFromFile("fonts/arial.ttf")) {
        cerr << "Error: Could not load font 'arial.ttf'\n";
        return 1;
    }

    // Initialize game components
    PlayerList pl; // Manages player data (login, stats)
    Inventory inv; // Manages backgrounds and sounds
    string currentUser; // Tracks logged-in user

    // Start the main menu (login/registration)
    displayMainMenu(pl, window, font, currentUser, inv);

    return 0;
}