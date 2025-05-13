#include "matchmaking.h"
#include <iostream>
#include <fstream>
#include <sstream>

MatchmakingQueue::MatchmakingQueue() : size(0) {
    for (int i = 0; i < MAX_SIZE; i++) heap[i] = nullptr;
    loadQueue(); // Load queue from file on construction
}

MatchmakingQueue::~MatchmakingQueue() {
    saveQueue(); // Save queue to file on destruction
    for (int i = 0; i < size; i++) delete heap[i];
}

void MatchmakingQueue::heapifyUp(int index) {
    while (index > 0) {
        int parent = getParent(index);
        if (*heap[index] < *heap[parent]) break; // Max-heap: higher score is higher priority

        // Swap without using std::swap
        QueueNode* temp = heap[index];
        heap[index] = heap[parent];
        heap[parent] = temp;

        index = parent;
    }
}

void MatchmakingQueue::heapifyDown(int index) {
    while (true) {
        int largest = index;
        int left = getLeftChild(index);
        int right = getRightChild(index);

        if (left < size && *heap[left] > *heap[largest]) largest = left;
        if (right < size && *heap[right] > *heap[largest]) largest = right;

        if (largest != index) {
            // Swap without using std::swap
            QueueNode* temp = heap[index];
            heap[index] = heap[largest];
            heap[largest] = temp;

            index = largest;
        }
        else {
            break;
        }
    }
}

bool MatchmakingQueue::enqueue(int playerID, int score, const std::string& username) {
    if (size >= MAX_SIZE || isPlayerInQueue(username)) return false;
    heap[size] = new QueueNode(playerID, score, username);
    heapifyUp(size);
    size++;
    std::cout << "Enqueued: " << username << " (Score: " << score << "), Size: " << size << std::endl;
    debugPrintQueue();
    saveQueue(); // Save queue after enqueuing
    return true;
}

bool MatchmakingQueue::dequeue(std::string& player1, std::string& player2) {
    if (size < 2) {
        std::cout << "Dequeue failed: Not enough players (Size: " << size << ")" << std::endl;
        return false;
    }

    // Remove first player
    player1 = heap[0]->username;
    delete heap[0];
    heap[0] = heap[size - 1];
    heap[size - 1] = nullptr;
    size--;
    if (size > 0) heapifyDown(0); // Re-heapify after first removal

    // Remove second player
    if (size > 0) {
        player2 = heap[0]->username;
        delete heap[0];
        heap[0] = heap[size - 1];
        heap[size - 1] = nullptr;
        size--;
        if (size > 0) heapifyDown(0); // Re-heapify after second removal
    }
    else {
        std::cout << "Dequeue failed: Only one player left after first dequeue" << std::endl;
        player2 = "";
        return false;
    }

    std::cout << "Dequeued: " << player1 << " vs " << player2 << ", Remaining Size: " << size << std::endl;
    debugPrintQueue();
    saveQueue(); // Save queue after dequeuing
    return player1 != player2;
}

std::string* MatchmakingQueue::getQueueContents(int& outSize) const {
    outSize = size;
    std::string* contents = new std::string[MAX_SIZE];
    for (int i = 0; i < MAX_SIZE; i++) {
        if (i < size && heap[i]) {
            contents[i] = heap[i]->username;
        }
        else {
            contents[i] = "";
        }
    }
    return contents;
}

bool MatchmakingQueue::isPlayerInQueue(const std::string& username) const {
    for (int i = 0; i < size; i++) {
        if (heap[i] && heap[i]->username == username) {
            return true;
        }
    }
    return false;
}

void MatchmakingQueue::debugPrintQueue() const {
    std::cout << "Queue Contents (Size: " << size << "): ";
    for (int i = 0; i < size; i++) {
        if (heap[i]) {
            std::cout << heap[i]->username << " (Score: " << heap[i]->score << ")";
            if (i < size - 1) std::cout << ", ";
        }
    }
    std::cout << std::endl;
}

void MatchmakingQueue::saveQueue() const {
    std::ofstream outFile("queue.dat");
    if (!outFile) {
        std::cerr << "Error: Could not open queue.dat for writing" << std::endl;
        return;
    }
    for (int i = 0; i < size; i++) {
        if (heap[i]) {
            // Escape spaces in username to handle multi-word usernames
            std::string escapedUsername = heap[i]->username;
            // Replace spaces with underscores without using std::replace
            for (size_t j = 0; j < escapedUsername.length(); j++) {
                if (escapedUsername[j] == ' ') {
                    escapedUsername[j] = '_';
                }
            }
            outFile << heap[i]->playerID << " " << heap[i]->score << " " << escapedUsername << "\n";
        }
    }
    outFile.close();
    std::cout << "Queue saved to queue.dat" << std::endl;
}

void MatchmakingQueue::loadQueue() {
    std::ifstream inFile("queue.dat");
    if (!inFile) {
        std::cout << "No existing queue.dat found, starting with empty queue" << std::endl;
        return;
    }

    // Clear current queue
    for (int i = 0; i < size; i++) {
        delete heap[i];
        heap[i] = nullptr;
    }
    size = 0;

    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        int playerID, score;
        std::string username;
        if (iss >> playerID >> score >> username) {
            // Replace underscores with spaces in username
            // Without using std::replace
            for (size_t j = 0; j < username.length(); j++) {
                if (username[j] == '_') {
                    username[j] = ' ';
                }
            }
            if (size < MAX_SIZE && !isPlayerInQueue(username)) {
                heap[size] = new QueueNode(playerID, score, username);
                heapifyUp(size);
                size++;
            }
        }
    }
    inFile.close();
    std::cout << "Queue loaded from queue.dat, Size: " << size << std::endl;
    debugPrintQueue();
}