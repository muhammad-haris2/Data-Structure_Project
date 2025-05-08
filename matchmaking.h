#ifndef MATCHMAKING_H
#define MATCHMAKING_H
#include <string>

class MatchmakingQueue {
private:
    struct QueueNode {
        int playerID;
        int score;
        std::string username;
        QueueNode(int id, int s, const std::string& uname) : playerID(id), score(s), username(uname) {}
        bool operator<(const QueueNode& other) const { return score < other.score; }
        bool operator>(const QueueNode& other) const { return score > other.score; }
    };

    static const int MAX_SIZE = 100;
    QueueNode* heap[MAX_SIZE];
    int size;

    void heapifyUp(int index);
    void heapifyDown(int index);
    int getParent(int index) const { return (index - 1) / 2; }
    int getLeftChild(int index) const { return 2 * index + 1; }
    int getRightChild(int index) const { return 2 * index + 2; }
    void saveQueue() const;
    void loadQueue();

public:
    MatchmakingQueue();
    ~MatchmakingQueue();
    bool enqueue(int playerID, int score, const std::string& username);
    bool dequeue(std::string& player1, std::string& player2);

    bool hasEnoughPlayers() const { return size >= 2; }
    int getSize() const { return size; }
    std::string* getQueueContents(int& outSize) const;
    bool isPlayerInQueue(const std::string& username) const;
    void debugPrintQueue() const;
};

#endif