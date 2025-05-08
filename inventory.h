#ifndef INVENTORY_H
#define INVENTORY_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>

class Inventory {
private:
    // Theme struct for background management
    struct Theme {
        int id;                  // Unique ID for the theme
        std::string name;        // Name of the theme (e.g., "normal")
        sf::Texture texture;     // SFML texture for the background
        Theme(int id, const std::string& name) : id(id), name(name) {}
    };

    // Sound struct for sound management
    struct Sound {
        int id;                  // Unique ID for the sound
        std::string name;        // Name of the sound (e.g., "Sound 1")
        sf::Music music;         // SFML music for the sound
        Sound(int id, const std::string& name) : id(id), name(name) {}
    };

    // AVL Node for themes
    struct AVLNode {
        Theme* theme;            // Theme data
        AVLNode* left;           // Left child
        AVLNode* right;          // Right child
        int height;              // Height of the node for balancing
        AVLNode(Theme* t) : theme(t), left(nullptr), right(nullptr), height(1) {}
    };

    // AVL Node for sounds
    struct AVLSoundNode {
        Sound* sound;            // Sound data
        AVLSoundNode* left;      // Left child
        AVLSoundNode* right;     // Right child
        int height;              // Height of the node for balancing
        AVLSoundNode(Sound* s) : sound(s), left(nullptr), right(nullptr), height(1) {}
    };

    AVLNode* root;               // Root of the theme AVL tree
    AVLSoundNode* soundRoot;     // Root of the sound AVL tree
    sf::Sprite backgroundSprite; // Sprite for rendering the current background
    Theme* currentTheme;         // Pointer to the current theme
    Sound* currentSound;         // Pointer to the current sound
    int themeCount;              // Number of themes in the tree
    int soundCount;              // Number of sounds in the tree

    // AVL Tree Helper Functions for Themes
    int getHeight(AVLNode* node) const;
    int getBalance(AVLNode* node) const;
    AVLNode* rightRotate(AVLNode* y);
    AVLNode* leftRotate(AVLNode* x);
    AVLNode* insertNode(AVLNode* node, Theme* theme);
    AVLNode* findMin(AVLNode* node) const;
    AVLNode* deleteNode(AVLNode* node, int id);
    AVLNode* searchByID(AVLNode* node, int id) const;
    AVLNode* searchByName(AVLNode* node, const std::string& name) const;
    void inOrderTraversal(AVLNode* node, std::string& result) const;
    void destroyTree(AVLNode* node);
    Theme* createTheme(int id, const std::string& name, const std::string& filePath);

    // AVL Tree Helper Functions for Sounds
    int getSoundHeight(AVLSoundNode* node) const;
    int getSoundBalance(AVLSoundNode* node) const;
    AVLSoundNode* rightSoundRotate(AVLSoundNode* y);
    AVLSoundNode* leftSoundRotate(AVLSoundNode* x);
    AVLSoundNode* insertSoundNode(AVLSoundNode* node, Sound* sound);
    AVLSoundNode* findSoundMin(AVLSoundNode* node) const;
    AVLSoundNode* deleteSoundNode(AVLSoundNode* node, int id);
    AVLSoundNode* searchSoundByID(AVLSoundNode* node, int id) const;
    AVLSoundNode* searchSoundByName(AVLSoundNode* node, const std::string& name) const;
    void inOrderSoundTraversal(AVLSoundNode* node, std::string& result) const;
    void destroySoundTree(AVLSoundNode* node);
    Sound* createSound(int id, const std::string& name, const std::string& filePath);

public:
    Inventory();
    ~Inventory();
    bool loadBackgrounds();
    bool loadSounds();
    void setBackground(int id);
    void setBackgroundByName(const std::string& name);
    void setSound(int id);
    void setSoundByName(const std::string& name);
    void stopSound(); // New: Stops the current sound
    void playDefaultSound(); // New: Plays the default sound (1.mp3)
    sf::Sprite& getBackgroundSprite();
    sf::Music& getCurrentSound();
    void drawBackground(sf::RenderWindow& window);
    std::string getBackgroundNames() const;
    std::string getSoundNames() const;
    int getCurrentBackgroundID() const;
    int getCurrentSoundID() const;
    int getBackgroundCount() const;
    int getSoundCount() const;
    bool addBackground(const std::string& name, const std::string& filePath);
    bool addSound(const std::string& name, const std::string& filePath);
    bool removeBackground(int id);
    bool removeSound(int id);
    bool updateBackgroundName(int id, const std::string& newName);
    bool updateSoundName(int id, const std::string& newName);
    Theme* getCurrentTheme() const;
    Sound* getCurrentSoundPtr() const;
    static const std::string BACKGROUND_PATH;
    static const std::string SOUND_PATH;
};

#endif