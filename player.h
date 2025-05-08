#ifndef PLAYER_H
#define PLAYER_H

#include <string>

struct FriendNode {
    std::string friendID;
    FriendNode* next;
    FriendNode(const std::string& id);
};

struct Player {
    std::string username;
    std::string password;
    std::string nickname;
    std::string email;
    std::string timestamp;
    int playerID;
    int totalScore;
    int totalPowerUps;
    int preferredThemeID;
    int preferredSoundID;
    FriendNode* friends;
    FriendNode* pendingRequests;
    Player* next;
    Player();
};

class ArrayList {
private:
    Player** players;
    int capacity;
    int size;
public:
    ArrayList(int initialCapacity = 10);
    ~ArrayList();
    void addPlayer(Player* p);
    Player* getPlayer(int index) const;
    int getSize() const;
};

class HashTable {
private:
    struct HashNode {
        std::string username;
        int index;
        HashNode* next;
        HashNode(const std::string& uname, int idx);
    };
    HashNode** table;
    int tableSize;
    int hashFunction(const std::string& key) const;
public:
    HashTable(int size = 100);
    ~HashTable();
    void insert(const std::string& username, int index);
    int find(const std::string& username) const;
};

class Leaderboard {
private:
    static const int maxSize = 10;
    struct HeapNode {
        int playerID;
        int score;
        HeapNode(int pid = 0, int s = 0) : playerID(pid), score(s) {}
    };
    HeapNode heap[maxSize];
    int size;
    int getParent(int index) const { return (index - 1) / 2; }
    int getLeftChild(int index) const { return 2 * index + 1; }
    int getRightChild(int index) const { return 2 * index + 2; }
    void heapifyUp(int index);
    void heapifyDown(int index);
    void saveToFile() const;
public:
    Leaderboard();
    void update(int playerID, int score);
    std::string getTopPlayers(const ArrayList& players) const;
    void loadFromFile(const ArrayList& players);
};

class PlayerList {
private:
    Player* head;
    ArrayList playerArray;
    HashTable playerHash;
    Leaderboard leaderboard;
    int idCounter;
    std::string getCurrentTimestamp() const;
    void saveFriend(const std::string& username, const std::string& friendID);
    void savePendingRequest(const std::string& username, const std::string& requesterID);
    void removePendingRequest(const std::string& username, const std::string& requesterID);
public:
    PlayerList();
    ~PlayerList();
    bool usernameExists(const std::string& uname) const;
    bool login(const std::string& uname, const std::string& pass) const;
    void registerPlayer(const std::string& uname, const std::string& pass, const std::string& nick, const std::string& email);
    void saveToFile(Player* p);
    void loadFromFile();
    bool sendFriendRequest(const std::string& sender, const std::string& receiver);
    bool acceptFriendRequest(const std::string& acceptor, const std::string& requester);
    bool rejectFriendRequest(const std::string& rejector, const std::string& requester);
    std::string getFriendsList(const std::string& username) const;
    std::string getPendingRequests(const std::string& username) const;
    void loadFriendsAndRequests();
    void savePlayerStats(const std::string& username, int score, int powerUps, int preferredThemeID, int preferredSoundID);
    std::string getPlayerStats(const std::string& username) const;
    void updateLeaderboard(int playerID, int score);
    std::string getLeaderboard() const;
    int getPlayerScore(const std::string& username) const;
    Player* getPlayerByUsername(const std::string& username) const;
};

#endif