#include "includes.h"


void expand(uint32_t toExpand, chess::Board &board, PUCTTree &tree, POLICY_NNUE::Accumulator &acc) {
    // Gets all the children of the board
    chess::Movelist moves;
    chess::movegen::legalmoves<chess::movegen::MoveGenType::ALL>(moves, board);

    // Only needs a single calculation of the policy head
    POLICY_NNUE::calcAccumulator(board, acc);

    float totalPrior = 0;

    for(int i = 0; i < moves.size(); i++) {
        float prior = POLICY_NNUE::eval(board, moves[i], acc);
        prior /= PST;
        tree.tree[tree.head + i].move = moves[i].move();
        tree.tree[tree.head + i].prior = prior;

        tree.tree[tree.head + i].visits = 0;
        tree.tree[tree.head + i].numChildren = 0;
        tree.tree[tree.head + i].score = 0;
        tree.tree[tree.head + i].firstChild = -1;

        totalPrior += exp(prior);
    }
    for(int i = 0; i < moves.size(); i++) {
        tree.tree[tree.head + i].prior = exp(tree.tree[tree.head + i].prior) / totalPrior;
    }

    tree.tree[toExpand].firstChild = tree.head;
    tree.tree[toExpand].numChildren = moves.size();

    tree.head += moves.size();
}

int selectAndBackupNode(PUCTTree &tree, chess::Board &board, POLICY_NNUE::Accumulator &pAcc, VALUE_NNUE::Accumulator &vAcc) {
    uint32_t path[256];
    int idx = 0;

    uint32_t curr = 0;
    
    // Selects a node
    while(true) {
        path[idx] = curr;
        idx++;

        if(tree.tree[curr].visits == 0) break;
        if(tree.tree[curr].visits == 1) expand(curr, board, tree, pAcc);

        // If this is a terminal node, no sense in looking for children so just return it
        Results res = getGameResult(board);
        if(res != Results::NotGameOver) break;

        // This for loop is equivalent to argmax
        uint32_t bestChild = -1;
        float bestChildValue = -__FLT_MAX__;

        for(int i = tree.tree[curr].firstChild; i < tree.tree[curr].firstChild + tree.tree[curr].numChildren; i++) {
            
            float childValue;
            float childExploration;

            // If visits = 0, then FPU kicks in
            if(tree.tree[i].visits == 0) {
                if(tree.tree[curr].move == chess::Move::NULL_MOVE) childValue = 999999;
                else childValue = FPU;
            } else {
                childValue = -(tree.tree[i].score / tree.tree[i].visits);
            }

            childExploration = (tree.tree[curr].move == chess::Move::NULL_MOVE ? rootCPUCT : cPUCT) * tree.tree[i].prior * (sqrt(tree.tree[curr].visits) / (tree.tree[i].visits + 1));

            if((childValue + childExploration) > bestChildValue) {
                bestChildValue = childValue + childExploration;
                bestChild = i;
            }
        }
        board.makeMove(tree.tree[bestChild].move);
        curr = bestChild;
    }

    float score;

    // Analyzes the node
    std::pair<chess::GameResultReason, chess::GameResult> result = board.isGameOver();
    if(result.second == chess::GameResult::NONE) score = VALUE_NNUE::eval(board, vAcc);
    else if(result.second == chess::GameResult::DRAW) score = 0;
    else if(result.second == chess::GameResult::WIN) score = 1;
    else if(result.second == chess::GameResult::LOSE) score = -1;

    for(int i = idx-1; i >= 0; i--) {
        tree.tree[path[i]].score += score;
        tree.tree[path[i]].visits++;
        score *= -1;
    }
    return idx;
}

uint32_t bestMove(PUCTTree &tree, uint32_t node) {
    uint32_t bestMoveIdx = 0;
    float bestMoveValue = __FLT_MAX__;
    uint32_t firstChild = tree.tree[node].firstChild;
    for(int i = 0; i < tree.tree[node].numChildren; i++) {
        float value = (tree.tree[firstChild + i].score / tree.tree[firstChild + i].visits);
        if(value < bestMoveValue) {
            bestMoveValue = value;
            bestMoveIdx = i;
        }
    }
    return firstChild + bestMoveIdx;
}

void print(PUCTTree &tree, PUCTNode node, int threshold, int distance=0) {
    if(distance != 0) {
        for(int i = 0; i < distance; i++) std::cout << "  ";
        std::cout << chess::uci::moveToUci(node.move) << " (P: " << node.prior << ", V: " << node.visits << ", S: " << (node.score / node.visits) << ")" << std::endl;
    }
    else {
        std::cout << "P: " << node.prior << ", V: " << node.visits << ", S: " << (node.score / node.visits) << std::endl;
    }
    for(int i = 0; i < node.numChildren; i++) {
        if(tree.tree[node.firstChild + i].visits >= threshold) print(tree, tree.tree[node.firstChild + i], threshold, distance + 1);
    }
}