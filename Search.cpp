#include "includes.h"

chess::Move PUCTSearch(chess::Board pos, PUCTTree &tree, Stop StopType, int stopValue) {
    // Resets the tree
    tree.head = 1;
    tree.tree[0].firstChild = 0;
    tree.tree[0].move = chess::Move::NULL_MOVE;
    tree.tree[0].numChildren = 0;
    tree.tree[0].prior = 0;
    tree.tree[0].score = 0;
    tree.tree[0].visits = 0;

    VALUE_NNUE::Accumulator vAcc;
    POLICY_NNUE::Accumulator pAcc;

    auto start = std::chrono::high_resolution_clock::now();

    std::chrono::time_point<std::chrono::high_resolution_clock> endTime;
    endTime = start + std::chrono::milliseconds(stopValue);

    int nodes = 0;
    chess::Board b2;

    float averageDepth = 0;
    int maxDepth = 0;

    while(true) {
        b2 = pos;
        int depth = selectAndBackupNode(tree, b2, pAcc, vAcc);
        averageDepth = (averageDepth * depthDecay) + ((1 - depthDecay) * depth);
        maxDepth = std::max(depth, maxDepth);
        if(tree.head + 218 >= tree.size) break;

        nodes++;

        if(nodes % checkInterval == 0) {
            if(StopType == Stop::Time && std::chrono::high_resolution_clock::now() > endTime) break;
            if(StopType == Stop::Nodes && nodes >= stopValue) break;
        }
        if(nodes % printInterval == 0) {
            auto elapsed = std::chrono::high_resolution_clock::now() - start;
            float u = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

            std::cout << "info depth " << std::round(averageDepth) << " seldepth " << maxDepth << " score cp " << (int)(atanh(tree.tree[0].score / tree.tree[0].visits) * 200.f);
            std::cout << " time " << (int)u << " nodes " << nodes;
            if(u >= 2) std::cout << " nps " << (int)(nodes / (u/1000));
            std::cout << " pv ";
            uint32_t pv = bestMove(tree, 0);
            while(true) {
                std::cout << chess::uci::moveToUci(tree.tree[pv].move) << " ";
                if(tree.tree[pv].numChildren == 0 || tree.tree[pv].visits < 10) break;
                pv = bestMove(tree, pv);
            }
            std::cout << std::endl;
        }
    }
    // print(tree, tree.tree[0], 50, 0);
    // std::cout << std::endl;
    return tree.tree[bestMove(tree, 0)].move;
}