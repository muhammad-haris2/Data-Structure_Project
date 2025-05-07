#include <SFML/Graphics.hpp>
#include "login.h"
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

    PlayerList pl;
    string currentUser;

    displayMainMenu(pl, window, font, currentUser);

    return 0;
}