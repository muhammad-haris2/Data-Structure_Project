#include "player.h"
#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

// FriendNode constructor
FriendNode::FriendNode(const string& id) : friendID(id), next(nullptr) {}

// Player constructor
Player::Player() : playerID(0), totalScore(0), totalPowerUps(0), preferredThemeID(1), preferredSoundID(1), // Default to theme ID 1 and sound ID 1
friends(nullptr), pendingRequests(nullptr), next(nullptr) {
}

// ArrayList implementation
ArrayList::ArrayList(int initialCapacity) : capacity(initialCapacity > 0 ? initialCapacity : 10), size(0) {
    players = new Player * [capacity]();
}

void ArrayList::addPlayer(Player* p) {
    if (p == nullptr) return;
    if (size >= capacity) {
        int newCapacity = capacity * 2;
        Player** newPlayers = new Player * [newCapacity]();
        for (int i = 0; i < size; i++) {
            newPlayers[i] = players[i];
        }
        delete[] players;
        players = newPlayers;
        capacity = newCapacity;
    }
    players[size++] = p;
}

Player* ArrayList::getPlayer(int index) const {
    if (index >= 0 && index < size) return players[index];
    return nullptr;
}

int ArrayList::getSize() const { return size; }

ArrayList::~ArrayList() {
    delete[] players;
}

// HashTable implementation
HashTable::HashNode::HashNode(const string& uname, int idx) : username(uname), index(idx), next(nullptr) {}

int HashTable::hashFunction(const string& key) const {
    unsigned long hash = 0;
    for (char c : key) hash = hash * 31 + c;
    return hash % tableSize;
}

HashTable::HashTable(int size) : tableSize(size) {
    table = new HashNode * [tableSize]();
    for (int i = 0; i < tableSize; i++) table[i] = nullptr;
}

void HashTable::insert(const string& username, int index) {
    int hash = hashFunction(username);
    HashNode* newNode = new HashNode(username, index);
    newNode->next = table[hash];
    table[hash] = newNode;
}

int HashTable::find(const string& username) const {
    int hash = hashFunction(username);
    HashNode* current = table[hash];
    while (current) {
        if (current->username == username) return current->index;
        current = current->next;
    }
    return -1;
}

HashTable::~HashTable() {
    for (int i = 0; i < tableSize; i++) {
        HashNode* current = table[i];
        while (current) {
            HashNode* temp = current;
            current = current->next;
            delete temp;
        }
    }
    delete[] table;
}

// Leaderboard implementation
Leaderboard::Leaderboard() : size(0) {
    for (int i = 0; i < maxSize; i++) heap[i] = HeapNode();
}

void Leaderboard::heapifyUp(int index) {
    while (index > 0) {
        int parent = getParent(index);
        if (heap[index].score < heap[parent].score) {
            // Swap
            HeapNode temp = heap[index];
            heap[index] = heap[parent];
            heap[parent] = temp;
            index = parent;
        }
        else break;
    }
}

void Leaderboard::heapifyDown(int index) {
    while (true) {
        int smallest = index;
        int left = getLeftChild(index);
        int right = getRightChild(index);

        if (left < size && heap[left].score < heap[smallest].score)
            smallest = left;
        if (right < size && heap[right].score < heap[smallest].score)
            smallest = right;

        if (smallest != index) {
            // Swap
            HeapNode temp = heap[index];
            heap[index] = heap[smallest];
            heap[smallest] = temp;
            index = smallest;
        }
        else break;
    }
}

void Leaderboard::update(int playerID, int score) {
    // Check if player is already in heap
    for (int i = 0; i < size; i++) {
        if (heap[i].playerID == playerID) {
            if (score > heap[i].score) {
                heap[i].score = score;
                heapifyDown(i);
                heapifyUp(i);
            }
            saveToFile();
            return;
        }
    }

    // If heap has space, insert new player
    if (size < maxSize) {
        heap[size] = HeapNode(playerID, score);
        heapifyUp(size);
        size++;
    }
    // If heap is full and score is higher than minimum, replace root
    else if (size > 0 && score > heap[0].score) {
        heap[0] = HeapNode(playerID, score);
        heapifyDown(0);
    }
    saveToFile();
}

string Leaderboard::getTopPlayers(const ArrayList& players) const {
    if (size == 0) return "Leaderboard is empty.";

    // Copy heap for sorting (manual descending sort)
    HeapNode sorted[10];
    for (int i = 0; i < size; i++) sorted[i] = heap[i];

    // Bubble sort in descending order
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (sorted[j].score < sorted[j + 1].score) {
                HeapNode temp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = temp;
            }
        }
    }

    string result = "Leaderboard (Top 10):\n";
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < players.getSize(); j++) {
            Player* p = players.getPlayer(j);
            if (p && p->playerID == sorted[i].playerID) {
                result += "Rank " + to_string(i + 1) + ": " + p->username +
                    " (ID: " + to_string(sorted[i].playerID) +
                    ", Score: " + to_string(sorted[i].score) + ")\n";
                break;
            }
        }
    }
    return result;
}

void Leaderboard::saveToFile() const {
    ofstream fout("leaderboard.txt");
    if (!fout) {
        cout << "Error: Unable to open leaderboard.txt for writing.\n";
        return;
    }
    for (int i = 0; i < size; i++) {
        fout << heap[i].playerID << "|" << heap[i].score << endl;
    }
    fout.close();
}

void Leaderboard::loadFromFile(const ArrayList& players) {
    ifstream fin("leaderboard.txt");
    if (!fin) return;

    size = 0;
    string line;
    while (getline(fin, line) && size < maxSize) {
        size_t pos = line.find('|');
        if (pos == string::npos) continue;
        int playerID = stoi(line.substr(0, pos));
        int score = stoi(line.substr(pos + 1));
        // Verify player exists
        bool valid = false;
        for (int i = 0; i < players.getSize(); i++) {
            if (players.getPlayer(i) && players.getPlayer(i)->playerID == playerID) {
                valid = true;
                break;
            }
        }
        if (valid) {
            heap[size] = HeapNode(playerID, score);
            heapifyUp(size);
            size++;
        }
    }
    fin.close();
}

// PlayerList implementation
PlayerList::PlayerList() : head(nullptr), idCounter(1), playerArray(10), playerHash(100) {
    loadFromFile();
}

string PlayerList::getCurrentTimestamp() const {
    time_t now = time(0);
    char buffer[26];
    ctime_s(buffer, sizeof(buffer), &now);
    string timestamp(buffer);
    timestamp.pop_back();
    return timestamp;
}

bool PlayerList::usernameExists(const string& uname) const {
    return playerHash.find(uname) != -1;
}

bool PlayerList::login(const string& uname, const string& pass) const {
    Player* temp = head;
    while (temp) {
        if (temp->username == uname && temp->password == pass) return true;
        temp = temp->next;
    }
    return false;
}

void PlayerList::registerPlayer(const string& uname, const string& pass, const string& nick, const string& email) {
    if (usernameExists(uname)) {
        cout << "Error: Username already taken.\n";
        return;
    }

    Player* newPlayer = new Player;
    newPlayer->username = uname;
    newPlayer->password = pass;
    newPlayer->nickname = nick;
    newPlayer->email = email;
    newPlayer->timestamp = getCurrentTimestamp();
    newPlayer->playerID = idCounter++;
    newPlayer->totalScore = 0;
    newPlayer->totalPowerUps = 0;
    newPlayer->preferredThemeID = 1; // Default theme (normal.jpg)
    newPlayer->preferredSoundID = 1; // Default sound (1.mp3)
    newPlayer->next = head;
    head = newPlayer;

    playerArray.addPlayer(newPlayer);
    playerHash.insert(uname, playerArray.getSize() - 1);

    saveToFile(newPlayer);
    cout << "Registration successful!\n";
}

void PlayerList::saveToFile(Player* p) {
    if (!p) {
        cout << "Error: Null player pointer.\n";
        return;
    }
    ofstream fout("players.txt", ios::app);
    if (!fout) {
        cout << "Error: Unable to open players.txt for saving.\n";
        return;
    }
    fout << p->username << "|" << p->password << "|" << p->nickname << "|" << p->email
        << "|" << p->timestamp << "|" << p->playerID << "|" << p->totalScore
        << "|" << p->totalPowerUps << "|" << p->preferredThemeID << "|"
        << p->preferredSoundID << endl;
    fout.close();
}

void PlayerList::loadFromFile() {
    ifstream fin("players.txt");
    if (!fin) return;

    string line;
    while (getline(fin, line)) {
        size_t pos = 0;
        string token;
        Player* p = new Player;
        int field = 0;
        while ((pos = line.find('|')) != string::npos) {
            token = line.substr(0, pos);
            switch (field++) {
            case 0: p->username = token; break;
            case 1: p->password = token; break;
            case 2: p->nickname = token; break;
            case 3: p->email = token; break;
            case 4: p->timestamp = token; break;
            case 5: p->playerID = stoi(token); break;
            case 6: p->totalScore = stoi(token); break;
            case 7: p->totalPowerUps = stoi(token); break;
            case 8: p->preferredThemeID = stoi(token); break;
            case 9: p->preferredSoundID = stoi(token); break;
            }
            line.erase(0, pos + 1);
        }
        // Handle backward compatibility
        try {
            if (field == 9) {
                p->preferredSoundID = stoi(line);
            }
            else if (field == 8) {
                p->preferredThemeID = stoi(line);
                p->preferredSoundID = 1; // Default sound
            }
            else if (field == 7) {
                p->totalPowerUps = stoi(line);
                p->preferredThemeID = 1;
                p->preferredSoundID = 1;
            }
            else if (field == 6) {
                p->totalScore = stoi(line);
                p->totalPowerUps = 0;
                p->preferredThemeID = 1;
                p->preferredSoundID = 1;
            }
            else if (field == 5) {
                p->playerID = stoi(line);
                p->totalScore = 0;
                p->totalPowerUps = 0;
                p->preferredThemeID = 1;
                p->preferredSoundID = 1;
            }
        }
        catch (const std::exception& e) {
            cout << "Error parsing line in players.txt: " << line << endl;
            delete p;
            continue;
        }
        p->next = head;
        head = p;
        playerArray.addPlayer(p);
        playerHash.insert(p->username, playerArray.getSize() - 1);
        idCounter = max(idCounter, p->playerID + 1);
    }
    fin.close();
    leaderboard.loadFromFile(playerArray);
    loadFriendsAndRequests();
}

bool PlayerList::sendFriendRequest(const string& sender, const string& receiver) {
    int senderIdx = playerHash.find(sender);
    int receiverIdx = playerHash.find(receiver);
    if (senderIdx == -1 || receiverIdx == -1 || sender == receiver) {
        cout << "Friend request failed: Invalid sender or receiver.\n";
        return false;
    }

    Player* senderPlayer = playerArray.getPlayer(senderIdx);
    Player* receiverPlayer = playerArray.getPlayer(receiverIdx);
    if (!senderPlayer || !receiverPlayer) {
        cout << "Friend request failed: Player data not found.\n";
        return false;
    }

    FriendNode* current = senderPlayer->friends;
    while (current) {
        if (current->friendID == receiver) {
            cout << "Friend request failed: Already friends.\n";
            return false;
        }
        current = current->next;
    }

    current = receiverPlayer->pendingRequests;
    while (current) {
        if (current->friendID == sender) {
            cout << "Friend request failed: Request already pending.\n";
            return false;
        }
        current = current->next;
    }

    FriendNode* newRequest = new FriendNode(sender);
    newRequest->next = receiverPlayer->pendingRequests;
    receiverPlayer->pendingRequests = newRequest;
    savePendingRequest(receiver, sender);
    cout << "Friend request sent.\n";
    return true;
}

bool PlayerList::acceptFriendRequest(const string& acceptor, const string& requester) {
    int acceptorIdx = playerHash.find(acceptor);
    int requesterIdx = playerHash.find(requester);
    if (acceptorIdx == -1 || requesterIdx == -1) {
        cout << "Accept failed: User(s) not found.\n";
        return false;
    }

    Player* acceptorPlayer = playerArray.getPlayer(acceptorIdx);
    Player* requesterPlayer = playerArray.getPlayer(requesterIdx);
    if (!acceptorPlayer || !requesterPlayer) {
        cout << "Accept failed: Player data not found.\n";
        return false;
    }

    FriendNode* current = acceptorPlayer->pendingRequests;
    FriendNode* prev = nullptr;
    while (current && current->friendID != requester) {
        prev = current;
        current = current->next;
    }
    if (!current) {
        cout << "Accept failed: No pending request.\n";
        return false;
    }

    if (prev) prev->next = current->next;
    else acceptorPlayer->pendingRequests = current->next;
    delete current;

    FriendNode* newFriend1 = new FriendNode(requester);
    newFriend1->next = acceptorPlayer->friends;
    acceptorPlayer->friends = newFriend1;

    FriendNode* newFriend2 = new FriendNode(acceptor);
    newFriend2->next = requesterPlayer->friends;
    requesterPlayer->friends = newFriend2;

    saveFriend(acceptor, requester);
    saveFriend(requester, acceptor);
    removePendingRequest(acceptor, requester);
    cout << "Friend request accepted.\n";
    return true;
}

bool PlayerList::rejectFriendRequest(const string& rejector, const string& requester) {
    int rejectorIdx = playerHash.find(rejector);
    if (rejectorIdx == -1) {
        cout << "Reject failed: Rejector not found.\n";
        return false;
    }

    Player* rejectorPlayer = playerArray.getPlayer(rejectorIdx);
    if (!rejectorPlayer) {
        cout << "Reject failed: Rejector data not found.\n";
        return false;
    }

    FriendNode* current = rejectorPlayer->pendingRequests;
    FriendNode* prev = nullptr;
    while (current && current->friendID != requester) {
        prev = current;
        current = current->next;
    }
    if (!current) {
        cout << "Reject failed: No pending request.\n";
        return false;
    }

    if (prev) prev->next = current->next;
    else rejectorPlayer->pendingRequests = current->next;
    delete current;

    removePendingRequest(rejector, requester);
    cout << "Friend request rejected.\n";
    return true;
}

string PlayerList::getFriendsList(const string& username) const {
    int idx = playerHash.find(username);
    if (idx == -1) return "User not found.";

    Player* player = playerArray.getPlayer(idx);
    if (!player) return "Player data not found.";

    string result = "Friends of " + username + ":\n";
    FriendNode* current = player->friends;
    if (!current) result += "No friends.";
    while (current) {
        result += current->friendID + "\n";
        current = current->next;
    }
    return result;
}

string PlayerList::getPendingRequests(const string& username) const {
    int idx = playerHash.find(username);
    if (idx == -1) return "User not found.";

    Player* player = playerArray.getPlayer(idx);
    if (!player) return "Player data not found.";

    string result = "Pending requests for " + username + ":\n";
    FriendNode* current = player->pendingRequests;
    if (!current) result += "No pending requests.";
    while (current) {
        result += current->friendID + "\n";
        current = current->next;
    }
    return result;
}

void PlayerList::saveFriend(const string& username, const string& friendID) {
    ofstream fout("friends.txt", ios::app);
    if (!fout) {
        cout << "Error: Unable to open friends.txt.\n";
        return;
    }
    fout << username << "|" << friendID << endl;
    fout.close();
}

void PlayerList::savePendingRequest(const string& username, const string& requesterID) {
    ofstream fout("pending_requests.txt", ios::app);
    if (!fout) {
        cout << "Error: Unable to open pending_requests.txt.\n";
        return;
    }
    fout << username << "|" << requesterID << endl;
    fout.close();
}

void PlayerList::removePendingRequest(const string& username, const string& requesterID) {
    ifstream fin("pending_requests.txt");
    if (!fin) {
        cout << "Error: Cannot open pending_requests.txt.\n";
        return;
    }

    ofstream temp("temp.txt");
    if (!temp) {
        cout << "Error: Cannot open temp.txt.\n";
        fin.close();
        return;
    }

    string line;
    bool found = false;
    while (getline(fin, line)) {
        size_t pos = line.find('|');
        string user = line.substr(0, pos);
        string requester = line.substr(pos + 1);
        if (!(user == username && requester == requesterID)) {
            temp << line << endl;
        }
        else {
            found = true;
        }
    }
    fin.close();
    temp.close();

    if (!found) {
        cout << "Warning: No pending request found.\n";
    }

    if (remove("pending_requests.txt") != 0) {
        cout << "Error: Failed to delete pending_requests.txt.\n";
        return;
    }
    if (rename("temp.txt", "pending_requests.txt") != 0) {
        cout << "Error: Failed to rename temp.txt.\n";
    }
}

void PlayerList::loadFriendsAndRequests() {
    ifstream fin("friends.txt");
    if (fin) {
        string line;
        while (getline(fin, line)) {
            size_t pos = line.find('|');
            string username = line.substr(0, pos);
            string friendID = line.substr(pos + 1);
            int userIdx = playerHash.find(username);
            int friendIdx = playerHash.find(friendID);
            if (userIdx != -1 && friendIdx != -1) {
                Player* user = playerArray.getPlayer(userIdx);
                if (user) {
                    FriendNode* newFriend = new FriendNode(friendID);
                    newFriend->next = user->friends;
                    user->friends = newFriend;
                }
            }
        }
        fin.close();
    }

    fin.open("pending_requests.txt");
    if (fin) {
        string line;
        while (getline(fin, line)) {
            size_t pos = line.find('|');
            string username = line.substr(0, pos);
            string requesterID = line.substr(pos + 1);
            int userIdx = playerHash.find(username);
            if (userIdx != -1) {
                Player* user = playerArray.getPlayer(userIdx);
                if (user) {
                    FriendNode* newRequest = new FriendNode(requesterID);
                    newRequest->next = user->pendingRequests;
                    user->pendingRequests = newRequest;
                }
            }
        }
        fin.close();
    }
}

void PlayerList::savePlayerStats(const string& username, int score, int powerUps, int preferredThemeID, int preferredSoundID) {
    int idx = playerHash.find(username);
    if (idx == -1) {
        cout << "Error: User " << username << " not found.\n";
        return;
    }

    Player* player = playerArray.getPlayer(idx);
    if (!player) {
        cout << "Error: Player data not found for " << username << ".\n";
        return;
    }

    player->totalScore += score;
    player->totalPowerUps += powerUps;
    player->preferredThemeID = preferredThemeID;
    player->preferredSoundID = preferredSoundID;

    // Update leaderboard
    updateLeaderboard(player->playerID, player->totalScore);

    // Update players.txt
    ifstream fin("players.txt");
    if (!fin) {
        cout << "Error: Cannot open players.txt.\n";
        return;
    }

    ofstream temp("temp.txt");
    if (!temp) {
        cout << "Error: Cannot open temp.txt.\n";
        fin.close();
        return;
    }

    string line;
    while (getline(fin, line)) {
        size_t pos = line.find('|');
        string user = line.substr(0, pos);
        if (user == username) {
            temp << user << "|" << player->password << "|" << player->nickname << "|" << player->email
                << "|" << player->timestamp << "|" << player->playerID << "|"
                << player->totalScore << "|" << player->totalPowerUps << "|"
                << player->preferredThemeID << "|" << player->preferredSoundID << endl;
        }
        else {
            temp << line << endl;
        }
    }
    fin.close();
    temp.close();

    if (remove("players.txt") != 0) {
        cout << "Error: Failed to delete players.txt.\n";
        return;
    }
    if (rename("temp.txt", "players.txt") != 0) {
        cout << "Error: Failed to rename temp.txt.\n";
    }
}

string PlayerList::getPlayerStats(const string& username) const {
    int idx = playerHash.find(username);
    if (idx == -1) return "User not found.";

    Player* player = playerArray.getPlayer(idx);
    if (!player) return "Player data not found.";

    return "Username: " + player->username + "\nNickname: " + player->nickname +
        "\nEmail: " + player->email + "\nRegistered: " + player->timestamp +
        "\nTotal Score: " + to_string(player->totalScore) +
        "\nTotal Power-Ups: " + to_string(player->totalPowerUps) +
        "\nPreferred Theme ID: " + to_string(player->preferredThemeID) +
        "\nPreferred Sound ID: " + to_string(player->preferredSoundID);
}

void PlayerList::updateLeaderboard(int playerID, int score) {
    leaderboard.update(playerID, score);
}

string PlayerList::getLeaderboard() const {
    return leaderboard.getTopPlayers(playerArray);
}

int PlayerList::getPlayerScore(const string& username) const {
    int idx = playerHash.find(username);
    if (idx == -1) return 0;
    Player* player = playerArray.getPlayer(idx);
    return player ? player->totalScore : 0;
}

Player* PlayerList::getPlayerByUsername(const string& username) const {
    int idx = playerHash.find(username);
    if (idx == -1) return nullptr;
    Player* player = playerArray.getPlayer(idx);
    return player;
}

PlayerList::~PlayerList() {
    Player* temp;
    while (head) {
        temp = head;
        head = head->next;
        FriendNode* current = temp->friends;
        while (current) {
            FriendNode* f = current;
            current = current->next;
            delete f;
        }
        current = temp->pendingRequests;
        while (current) {
            FriendNode* f = current;
            current = current->next;
            delete f;
        }
        delete temp;
    }
}