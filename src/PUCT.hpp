#pragma once
#include "includes.hpp"

float CPUCT = 2.76f;
float RootCPUCT = 7.50f;
float RootPST = 1.74f;
float PST = 1.06f;

float MATE_VALUE = 2.0f;

// 20 bytes
struct PUCTNode {
    uint32_t visits = 0;

    float score = 0;
    float prior = 0;

    uint32_t first_child;

    uint16_t move;
    uint8_t num_children;

    uint8_t flags = 0; // Unused
    PUCTNode(){};

    bool isSolved() {
        return (flags & 0b11) != 0;
    }

    float getSolution() {
        // Returns 1.0f for win, -1.0f for loss
        return (flags & 1) ? MATE_VALUE : -MATE_VALUE;
    }

    void setSolution(float value) {
        flags &= 0b11111100;  // Clear solution bits
        if (value == MATE_VALUE) {
            flags |= 1;  // Set bit 0 for win
        } else if (value == -MATE_VALUE) {
            flags |= 2;  // Set bit 1 for loss
        }
    }
};

// Used for PV and such
struct Path {
    uint32_t path[256] = {0};
    uint32_t size = 0;

    Path() {}

    void add(uint32_t node) {
        if(size >= 255) {
            throw std::out_of_range("Index out of bounds");
        }
        path[size] = node;
        size++;
    }

    void clear() {
        size = 0;
    }

    // Const index operator - returns value
    uint32_t operator[](uint32_t index) const {
        if (index >= size || index >= 255) {
            throw std::out_of_range("Index out of bounds");
        }
        return path[index];
    }

    // Non-const index operator - returns reference
    uint32_t& operator[](uint32_t index) {
        if (index >= size || index >= 255) {
            throw std::out_of_range("Index out of bounds");
        }
        return path[index];
    }
};


struct PUCTTree {
    PUCTNode *tree = nullptr;
    int head = 1;
    uint32_t size = 0;

    // Standard constructor
    PUCTTree(uint32_t initialSize) : size(initialSize) {
        tree = new PUCTNode[size];
        head = 1;
    }

    PUCTTree() : PUCTTree(1024) {} // Default size of 1024 nodes

    // Destructor
    ~PUCTTree() {
        if(tree != nullptr) {
            delete[] tree;
            tree = nullptr;
        }
    }

    void resize(uint32_t newSize) {
        // Delete the old tree first to prevent OOM
        if(tree != nullptr) {
            delete[] tree;
            tree = nullptr;
        }
        
        // Allocate new tree with new size
        size = newSize;
        tree = new PUCTNode[size];
        head = 1; // Reset head pointer
    }

    // Const index operator - returns value
    PUCTNode operator[](uint32_t index) const {
        if (index >= size) {
            throw std::out_of_range("Index out of bounds");
        }
        return tree[index];
    }

    // Non-const index operator - returns reference
    PUCTNode& operator[](uint32_t index) {
        if (index >= size) {
            throw std::out_of_range("Index out of bounds");
        }
        return tree[index];
    }

    void clear() {
        head = 1;
        tree[0].first_child = 0;
        tree[0].move = chess::Move::NO_MOVE;
        tree[0].num_children = 0;
        tree[0].prior = 0;
        tree[0].score = 0;
        tree[0].visits = 0;
        tree[0].flags = 0;
    }

    // The selection step for MCTS
    void select(Path &path, chess::Board &board);

    // The expansion step for MCTS
    // In the future, this will be integrated with the policy network
    void expand(uint32_t node_to_expand, chess::Board &board);

    // The backup step for MCTS
    void backup(Path &path, float value);

    // UB if there are no root visits
    void getPV(Path &path, int threshold=0);

    // Prints debugging information for the tree
    void print(int threshold=999999, uint32_t node=0, int distance=0);

    chess::Move sample(float temperature, xorshift &rng);
};