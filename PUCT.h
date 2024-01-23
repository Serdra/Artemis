#pragma once
#include "includes.h"

float cPUCT = 2.72;
float rootCPUCT = 6.50;
float FPU = -0.42;
float PST = 1.47;

// 20 bytes
struct PUCTNode {
    uint32_t visits = 0;

    float score = 0;
    float prior = 0;

    uint32_t firstChild;

    uint16_t move;
    uint8_t numChildren;
    PUCTNode(){};
};

struct PUCTTree {
    PUCTNode *tree;
    int head = 0;
    uint32_t size;
};

// We always assume head + 218 is less than max
// Takes a node and expands it, adding the children to tree
void expand(uint32_t toExpand, chess::Board &board, PUCTTree &tree, POLICY_NNUE::Accumulator &acc);

// Selects a node to be analyzed
int selectAndBackupNode(PUCTTree &tree, chess::Board &board, POLICY_NNUE::Accumulator &pAcc, VALUE_NNUE::Accumulator &vAcc);

// Returns the best move of the node
uint32_t bestMove(PUCTTree &tree, uint32_t node);

// Prints the tree
void print(PUCTTree &tree, uint32_t *node=0, int threshold=1000, int distance=0);