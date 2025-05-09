#include "inventory.h"
#include <iostream>
#include <fstream>

const std::string Inventory::BACKGROUND_PATH = "images/backgrounds/";
const std::string Inventory::SOUND_PATH = "music/";

Inventory::Inventory() : root(nullptr), soundRoot(nullptr), currentTheme(nullptr), currentSound(nullptr), themeCount(0), soundCount(0) {
    backgroundSprite.setScale(800.0f / 1920.0f, 600.0f / 1080.0f); // Scale to 800x600 window
    loadBackgrounds();
    loadSounds();
}

Inventory::~Inventory() {
    destroyTree(root);
    destroySoundTree(soundRoot);
    root = nullptr;
    soundRoot = nullptr;
}

// Theme AVL Tree Implementations
int Inventory::getHeight(AVLNode* node) const {
    return node ? node->height : 0;
}

int Inventory::getBalance(AVLNode* node) const {
    return node ? getHeight(node->left) - getHeight(node->right) : 0;
}

Inventory::AVLNode* Inventory::rightRotate(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = std::max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = std::max(getHeight(x->left), getHeight(x->right)) + 1;
    return x;
}

Inventory::AVLNode* Inventory::leftRotate(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = std::max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = std::max(getHeight(y->left), getHeight(y->right)) + 1;
    return y;
}

Inventory::AVLNode* Inventory::insertNode(AVLNode* node, Theme* theme) {
    if (!node) {
        themeCount++;
        return new AVLNode(theme);
    }
    if (theme->id < node->theme->id)
        node->left = insertNode(node->left, theme);
    else if (theme->id > node->theme->id)
        node->right = insertNode(node->right, theme);
    else
        return node; // Duplicate IDs not allowed

    node->height = std::max(getHeight(node->left), getHeight(node->right)) + 1;
    int balance = getBalance(node);

    // Left Left
    if (balance > 1 && theme->id < node->left->theme->id)
        return rightRotate(node);
    // Right Right
    if (balance < -1 && theme->id > node->right->theme->id)
        return leftRotate(node);
    // Left Right
    if (balance > 1 && theme->id > node->left->theme->id) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    // Right Left
    if (balance < -1 && theme->id < node->right->theme->id) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }
    return node;
}

Inventory::AVLNode* Inventory::findMin(AVLNode* node) const {
    while (node && node->left)
        node = node->left;
    return node;
}

Inventory::AVLNode* Inventory::deleteNode(AVLNode* node, int id) {
    if (!node)
        return node;
    if (id < node->theme->id)
        node->left = deleteNode(node->left, id);
    else if (id > node->theme->id)
        node->right = deleteNode(node->right, id);
    else {
        if (!node->left || !node->right) {
            AVLNode* temp = node->left ? node->left : node->right;
            if (!temp) {
                temp = node;
                node = nullptr;
            }
            else {
                *node = *temp;
            }
            delete temp->theme;
            delete temp;
            themeCount--;
        }
        else {
            AVLNode* temp = findMin(node->right);
            node->theme = temp->theme;
            node->right = deleteNode(node->right, temp->theme->id);
        }
    }
    if (!node)
        return node;

    node->height = std::max(getHeight(node->left), getHeight(node->right)) + 1;
    int balance = getBalance(node);

    // Left Left
    if (balance > 1 && getBalance(node->left) >= 0)
        return rightRotate(node);
    // Left Right
    if (balance > 1 && getBalance(node->left) < 0) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    // Right Right
    if (balance < -1 && getBalance(node->right) <= 0)
        return leftRotate(node);
    // Right Left
    if (balance < -1 && getBalance(node->right) > 0) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }
    return node;
}

Inventory::AVLNode* Inventory::searchByID(AVLNode* node, int id) const {
    if (!node || node->theme->id == id)
        return node;
    if (id < node->theme->id)
        return searchByID(node->left, id);
    return searchByID(node->right, id);
}

Inventory::AVLNode* Inventory::searchByName(AVLNode* node, const std::string& name) const {
    if (!node)
        return nullptr;
    if (node->theme->name == name)
        return node;
    AVLNode* leftResult = searchByName(node->left, name);
    if (leftResult)
        return leftResult;
    return searchByName(node->right, name);
}

void Inventory::inOrderTraversal(AVLNode* node, std::string& result) const {
    if (node) {
        inOrderTraversal(node->left, result);
        result += node->theme->name + "\n";
        inOrderTraversal(node->right, result);
    }
}

void Inventory::destroyTree(AVLNode* node) {
    if (node) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node->theme;
        delete node;
    }
}

Inventory::Theme* Inventory::createTheme(int id, const std::string& name, const std::string& filePath) {
    Theme* theme = new Theme(id, name);
    if (!theme->texture.loadFromFile(filePath)) {
        std::cerr << "Failed to load background: " << filePath << std::endl;
        delete theme;
        return nullptr;
    }
    return theme;
}

// Sound AVL Tree Implementations
int Inventory::getSoundHeight(AVLSoundNode* node) const {
    return node ? node->height : 0;
}

int Inventory::getSoundBalance(AVLSoundNode* node) const {
    return node ? getSoundHeight(node->left) - getSoundHeight(node->right) : 0;
}

Inventory::AVLSoundNode* Inventory::rightSoundRotate(AVLSoundNode* y) {
    AVLSoundNode* x = y->left;
    AVLSoundNode* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = std::max(getSoundHeight(y->left), getSoundHeight(y->right)) + 1;
    x->height = std::max(getSoundHeight(x->left), getSoundHeight(x->right)) + 1;
    return x;
}

Inventory::AVLSoundNode* Inventory::leftSoundRotate(AVLSoundNode* x) {
    AVLSoundNode* y = x->right;
    AVLSoundNode* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = std::max(getSoundHeight(x->left), getSoundHeight(x->right)) + 1;
    y->height = std::max(getSoundHeight(y->left), getSoundHeight(y->right)) + 1;
    return y;
}

Inventory::AVLSoundNode* Inventory::insertSoundNode(AVLSoundNode* node, Sound* sound) {
    if (!node) {
        soundCount++;
        return new AVLSoundNode(sound);
    }
    if (sound->id < node->sound->id)
        node->left = insertSoundNode(node->left, sound);
    else if (sound->id > node->sound->id)
        node->right = insertSoundNode(node->right, sound);
    else
        return node; // Duplicate IDs not allowed

    node->height = std::max(getSoundHeight(node->left), getSoundHeight(node->right)) + 1;
    int balance = getSoundBalance(node);

    // Left Left
    if (balance > 1 && sound->id < node->left->sound->id)
        return rightSoundRotate(node);
    // Right Right
    if (balance < -1 && sound->id > node->right->sound->id)
        return leftSoundRotate(node);
    // Left Right
    if (balance > 1 && sound->id > node->left->sound->id) {
        node->left = leftSoundRotate(node->left);
        return rightSoundRotate(node);
    }
    // Right Left
    if (balance < -1 && sound->id < node->right->sound->id) {
        node->right = rightSoundRotate(node->right);
        return leftSoundRotate(node);
    }
    return node;
}

Inventory::AVLSoundNode* Inventory::findSoundMin(AVLSoundNode* node) const {
    while (node && node->left)
        node = node->left;
    return node;
}

Inventory::AVLSoundNode* Inventory::deleteSoundNode(AVLSoundNode* node, int id) {
    if (!node)
        return node;
    if (id < node->sound->id)
        node->left = deleteSoundNode(node->left, id);
    else if (id > node->sound->id)
        node->right = deleteSoundNode(node->right, id);
    else {
        if (!node->left || !node->right) {
            AVLSoundNode* temp = node->left ? node->left : node->right;
            if (!temp) {
                temp = node;
                node = nullptr;
            }
            else {
                *node = *temp;
            }
            delete temp->sound;
            delete temp;
            soundCount--;
        }
        else {
            AVLSoundNode* temp = findSoundMin(node->right);
            node->sound = temp->sound;
            node->right = deleteSoundNode(node->right, temp->sound->id);
        }
    }
    if (!node)
        return node;

    node->height = std::max(getSoundHeight(node->left), getSoundHeight(node->right)) + 1;
    int balance = getSoundBalance(node);

    // Left Left
    if (balance > 1 && getSoundBalance(node->left) >= 0)
        return rightSoundRotate(node);
    // Left Right
    if (balance > 1 && getSoundBalance(node->left) < 0) {
        node->left = leftSoundRotate(node->left);
        return rightSoundRotate(node);
    }
    // Right Right
    if (balance < -1 && getSoundBalance(node->right) <= 0)
        return leftSoundRotate(node);
    // Right Left
    if (balance < -1 && getSoundBalance(node->right) > 0) {
        node->right = rightSoundRotate(node->right);
        return leftSoundRotate(node);
    }
    return node;
}

Inventory::AVLSoundNode* Inventory::searchSoundByID(AVLSoundNode* node, int id) const {
    if (!node || node->sound->id == id)
        return node;
    if (id < node->sound->id)
        return searchSoundByID(node->left, id);
    return searchSoundByID(node->right, id);
}

Inventory::AVLSoundNode* Inventory::searchSoundByName(AVLSoundNode* node, const std::string& name) const {
    if (!node)
        return nullptr;
    if (node->sound->name == name)
        return node;
    AVLSoundNode* leftResult = searchSoundByName(node->left, name);
    if (leftResult)
        return leftResult;
    return searchSoundByName(node->right, name);
}

void Inventory::inOrderSoundTraversal(AVLSoundNode* node, std::string& result) const {
    if (node) {
        inOrderSoundTraversal(node->left, result);
        result += node->sound->name + "\n";
        inOrderSoundTraversal(node->right, result);
    }
}

void Inventory::destroySoundTree(AVLSoundNode* node) {
    if (node) {
        destroySoundTree(node->left);
        destroySoundTree(node->right);
        delete node->sound;
        delete node;
    }
}

Inventory::Sound* Inventory::createSound(int id, const std::string& name, const std::string& filePath) {
    Sound* sound = new Sound(id, name);
    if (!sound->music.openFromFile(filePath)) {
        std::cerr << "Failed to load sound: " << filePath << std::endl;
        delete sound;
        return nullptr;
    }
    return sound;
}

bool Inventory::loadBackgrounds() {
    std::string themes[] = { "normal.jpg", "light.jpg", "dark.jpg", "space.jpg" };
    int id = 1;
    for (const auto& themeName : themes) {
        std::string filePath = BACKGROUND_PATH + themeName;
        Theme* theme = createTheme(id, themeName.substr(0, themeName.find('.')), filePath);
        if (theme) {
            root = insertNode(root, theme);
            if (id == 1) {
                currentTheme = theme;
                backgroundSprite.setTexture(currentTheme->texture);
            }
            id++;
        }
    }
    return themeCount > 0;
}

bool Inventory::loadSounds() {
    std::string sounds[] = { "1.mp3", "2.mp3", "3.mp3", "4.mp3" };
    int id = 1;
    for (const auto& soundFile : sounds) {
        std::string name = "Sound " + std::to_string(id);
        std::string filePath = SOUND_PATH + soundFile;
        Sound* sound = createSound(id, name, filePath);
        if (sound) {
            soundRoot = insertSoundNode(soundRoot, sound);
            if (id == 1) {
                currentSound = sound; // Set default sound to Sound 1
            }
            id++;
        }
    }
    if (soundCount > 0 && !currentSound) {
        // If no sound was set (e.g., all loads failed), try to set the first available sound
        AVLSoundNode* firstNode = findSoundMin(soundRoot);
        if (firstNode) {
            currentSound = firstNode->sound;
        }
    }
    return soundCount > 0;
}

void Inventory::setBackground(int id) {
    AVLNode* node = searchByID(root, id);
    if (node) {
        currentTheme = node->theme;
        backgroundSprite.setTexture(currentTheme->texture);
    }
}

void Inventory::setBackgroundByName(const std::string& name) {
    AVLNode* node = searchByName(root, name);
    if (node) {
        currentTheme = node->theme;
        backgroundSprite.setTexture(currentTheme->texture);
    }
}

void Inventory::setSound(int id) {
    if (currentSound) {
        currentSound->music.stop(); // Stop the current sound if playing
    }
    AVLSoundNode* node = searchSoundByID(soundRoot, id);
    if (node) {
        currentSound = node->sound;
    }
    else if (soundCount > 0 && !currentSound) {
        // Fallback to default sound (ID 1) if the requested ID is invalid
        AVLSoundNode* defaultNode = searchSoundByID(soundRoot, 1);
        if (defaultNode) {
            currentSound = defaultNode->sound;
        }
    }
}

void Inventory::setSoundByName(const std::string& name) {
    if (currentSound) {
        currentSound->music.stop(); // Stop the current sound if playing
    }
    AVLSoundNode* node = searchSoundByName(soundRoot, name);
    if (node) {
        currentSound = node->sound;
    }
    else if (soundCount > 0 && !currentSound) {
        // Fallback to default sound (ID 1) if the requested name is invalid
        AVLSoundNode* defaultNode = searchSoundByID(soundRoot, 1);
        if (defaultNode) {
            currentSound = defaultNode->sound;
        }
    }
}

sf::Sprite& Inventory::getBackgroundSprite() {
    return backgroundSprite;
}

sf::Music& Inventory::getCurrentSound() {
    if (!currentSound) {
        // Try to set default sound (ID 1) if currentSound is null
        AVLSoundNode* defaultNode = searchSoundByID(soundRoot, 1);
        if (defaultNode) {
            currentSound = defaultNode->sound;
        }
        else {
            throw std::runtime_error("No sound available in inventory");
        }
    }
    return currentSound->music;
}

void Inventory::drawBackground(sf::RenderWindow& window) {
    window.draw(backgroundSprite);
}

std::string Inventory::getBackgroundNames() const {
    std::string result;
    inOrderTraversal(root, result);
    return result;
}

std::string Inventory::getSoundNames() const {
    std::string result;
    inOrderSoundTraversal(soundRoot, result);
    return result;
}

int Inventory::getCurrentBackgroundID() const {
    return currentTheme ? currentTheme->id : -1;
}

int Inventory::getCurrentSoundID() const {
    return currentSound ? currentSound->id : -1;
}

int Inventory::getBackgroundCount() const {
    return themeCount;
}

int Inventory::getSoundCount() const {
    return soundCount;
}

bool Inventory::addBackground(const std::string& name, const std::string& filePath) {
    Theme* theme = createTheme(themeCount + 1, name, filePath);
    if (!theme)
        return false;
    root = insertNode(root, theme);
    return true;
}

bool Inventory::addSound(const std::string& name, const std::string& filePath) {
    Sound* sound = createSound(soundCount + 1, name, filePath);
    if (!sound)
        return false;
    soundRoot = insertSoundNode(soundRoot, sound);
    return true;
}

bool Inventory::removeBackground(int id) {
    if (!searchByID(root, id))
        return false;
    root = deleteNode(root, id);
    if (currentTheme && currentTheme->id == id) {
        AVLNode* firstNode = findMin(root);
        currentTheme = firstNode ? firstNode->theme : nullptr;
        if (currentTheme)
            backgroundSprite.setTexture(currentTheme->texture);
    }
    return true;
}

bool Inventory::removeSound(int id) {
    if (!searchSoundByID(soundRoot, id))
        return false;
    soundRoot = deleteSoundNode(soundRoot, id);
    if (currentSound && currentSound->id == id) {
        AVLSoundNode* firstNode = findSoundMin(soundRoot);
        currentSound = firstNode ? firstNode->sound : nullptr;
    }
    return true;
}

bool Inventory::updateBackgroundName(int id, const std::string& newName) {
    AVLNode* node = searchByID(root, id);
    if (!node)
        return false;
    node->theme->name = newName;
    return true;
}

bool Inventory::updateSoundName(int id, const std::string& newName) {
    AVLSoundNode* node = searchSoundByID(soundRoot, id);
    if (!node)
        return false;
    node->sound->name = newName;
    return true;
}

Inventory::Theme* Inventory::getCurrentTheme() const {
    return currentTheme;
}

Inventory::Sound* Inventory::getCurrentSoundPtr() const {
    return currentSound;
}