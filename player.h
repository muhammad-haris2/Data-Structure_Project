#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <fstream>
#include <ctime>

using namespace std;

// Struct for Friend Node
struct FriendNode {
    string friendID;
    FriendNode* next;
    FriendNode(const string& id);
};

// Struct for each Player
struct Player {
    string username;
    string password;
    string nickname;
    string email;
    string timestamp;
    int playerID;
    int totalScore;
    int totalPowerUps;
    FriendNode* friends;
    FriendNode* pendingRequests;
    Player* next;
    Player();
};

// Custom Dynamic Array (ArrayList)
class ArrayList {
    Player** players;
    int capacity;
    int size;

public:
    ArrayList(int initialCapacity = 10);
    void addPlayer(Player* p);
    Player* getPlayer(int index) const;
    int getSize() const;
    ~ArrayList();
};

// Hash Table
class HashTable {
    struct HashNode {
        string username;
        int index;
        HashNode* next;
        HashNode(const string& uname, int idx);
    };

    HashNode** table;
    int tableSize;

    int hashFunction(const string& key) const;

public:
    HashTable(int size = 100);
    void insert(const string& username, int index);
    int find(const string& username) const;
    ~HashTable();
};

// Leaderboard using Min-Heap (array-based)
class Leaderboard {
    struct HeapNode {
        int playerID;
        int score;
        HeapNode(int id = 0, int s = 0) : playerID(id), score(s) {}
    };

    HeapNode heap[10];
    int size;
    const int maxSize = 10;

    void heapifyUp(int index);
    void heapifyDown(int index);
    int getParent(int index) const { return (index - 1) / 2; }
    int getLeftChild(int index) const { return 2 * index + 1; }
    int getRightChild(int index) const { return 2 * index + 2; }

public:
    Leaderboard();
    void update(int playerID, int score);
    string getTopPlayers(const ArrayList& players) const;
    void saveToFile() const;
    void loadFromFile(const ArrayList& players);
};

// PlayerList class
class PlayerList {
    Player* head;
    int idCounter;
    ArrayList playerArray;
    HashTable playerHash;
    Leaderboard leaderboard;

public:
    PlayerList();
    string getCurrentTimestamp() const;
    bool usernameExists(const string& uname) const;
    bool login(const string& uname, const string& pass) const;
    void registerPlayer(const string& uname, const string& pass, const string& nick, const string& email);
    void saveToFile(Player* p);
    void loadFromFile();
    bool sendFriendRequest(const string& sender, const string& receiver);
    bool acceptFriendRequest(const string& acceptor, const string& requester);
    bool rejectFriendRequest(const string& rejector, const string& requester);
    string getFriendsList(const string& username) const;
    string getPendingRequests(const string& username) const;
    void saveFriend(const string& username, const string& friendID);
    void savePendingRequest(const string& username, const string& requesterID);
    void removePendingRequest(const string& username, const string& requesterID);
    void loadFriendsAndRequests();
    void savePlayerStats(const string& username, int score, int powerUps);
    string getPlayerStats(const string& username) const;
    void updateLeaderboard(int playerID, int score);
    string getLeaderboard() const;
    int getPlayerScore(const std::string& username) const;
    Player* getPlayerByUsername(const std::string& username) const;
    ~PlayerList();
};

#endif