#include "includes.hpp"

void PUCTTree::select(Path &path, chess::Board &board) {
    path.clear();
    uint32_t curr = 0;

    while(true) {
        path.add(curr);

        // Leaf node, so we return
        if(tree[curr].visits == 0) break;

        // If this is a terminal node
        if(board.isGameOver().first != chess::GameResultReason::NONE) break;

        // We only expand on the second visit to save space and policy network calls
        if(tree[curr].visits == 1) expand(curr, board);

        // If there are no children, this is a terminal node, so we break
        if(tree[curr].num_children == 0) break;

        if(tree[curr].isSolved()) break;

        uint32_t best_child = -1;
        float best_child_value = -__FLT_MAX__;

        for(int i = tree[curr].first_child; i < tree[curr].first_child + tree[curr].num_children; i++) {
            float child_value;
            float child_exploration;


            // If we have not explored the child, we use the parent's value as exploration
            // Unless this is the root node, in which case we immediately select it
            if(tree[i].visits == 0) {
                if(curr == 0) child_value = 9999;
                else child_value = (tree[curr].score / tree[curr].visits);
            } else {
                child_value = -(tree[i].score / tree[i].visits);
            }

            child_exploration = tree[i].prior * sqrt(tree[curr].visits) / tree[i].visits;
            if(curr == 0) child_exploration *= RootCPUCT;
            else child_exploration *= CPUCT;

            if((child_exploration + child_value) > best_child_value) {
                best_child_value = child_exploration + child_value;
                best_child = i;
            }
        }

        curr = best_child;
        board.makeMove(tree[curr].move);
    }
}

void PUCTTree::expand(uint32_t node_to_expand, chess::Board &board) {
    chess::Movelist moves;
    chess::movegen::legalmoves(moves, board);

    for(int i = 0; i < moves.size(); i++) {
        tree[head + i].move = moves[i].move();
        tree[head + i].prior = (1.f / moves.size());
        
        tree[head + i].visits = 0;
        tree[head + i].num_children = 0;
        tree[head + i].score = 0;
        tree[head + i].first_child = 0;
        tree[head + i].flags = 0;
    }

    tree[node_to_expand].first_child = head;
    tree[node_to_expand].num_children = moves.size();
    head += moves.size();
}

void PUCTTree::backup(Path &path, float value) {
    // Handle the leaf node seperately, as we always update solution if necessary there
    tree[path[path.size - 1]].score += value;
    tree[path[path.size - 1]].visits++;
    if(value == 1 || value == -1) {
        tree[path[path.size - 1]].setSolution(value);
    }
    value = -value;

    for(int i = path.size - 2; i >= 0; i--) {
        tree[path[i]].score += value;
        tree[path[i]].visits++;
        value = -value;

        if(path[i] == 0) continue;

        // We can set this automatically, without checking siblings
        if(tree[path[i + 1]].isSolved() && tree[path[i + 1]].getSolution() == 1) {
            tree[path[i]].setSolution(-1);
        }

        // We need to check that all moves are a loss to know that this node is a loss
        if(tree[path[i + 1]].isSolved() && tree[path[i + 1]].getSolution() == -1) {
            bool canSet = true;
            for(int j = tree[path[i]].first_child; j < tree[path[i]].first_child + tree[path[i]].num_children; j++) {
                if(!tree[j].isSolved() || tree[j].getSolution() != -1) canSet = false;
            }
            if(canSet) tree[path[i]].setSolution(1);
        }
    }
}

void PUCTTree::getPV(Path &path, int threshold) {
    path.clear();
    int curr = 0;

    do {
        uint32_t best_node = tree[curr].first_child;
        for(int i = tree[curr].first_child + 1; i < tree[curr].first_child + tree[curr].num_children; i++) {
            // If there are no visits other than the root node, do policy
            if(tree[i].visits == 0 && tree[best_node].visits == 0) {
                if(tree[i].prior > tree[best_node].prior) best_node = i;
            }

            else if(tree[i].visits > tree[best_node].visits) best_node = i;

            // Otherwise, do the highest value move
            else if((tree[i].score / tree[i].visits) < (tree[best_node].score / tree[best_node].visits)) best_node = i;
        }
        curr = best_node;
        path.add(curr);
    } while(tree[curr].visits > threshold && tree[curr].num_children != 0);
}

void PUCTTree::print(int threshold, uint32_t node, int distance) {
    if(distance != 0) {
        for(int i = 0; i < distance; i++) std::cout << "  ";
        std::cout << std::setprecision(4) << 
            chess::uci::moveToUci(tree[node].move) << "(P: " << tree[node].prior <<
            ", V: " << tree[node].visits << 
            ", Q: " << (tree[node].visits != 0 ? tree[node].score / tree[node].visits : 0) << ")" << std::endl;
    }
    else {
        std::cout << std::setprecision(4) << 
            "V: " << tree[node].visits << 
            ", Q: " << (tree[node].visits != 0 ? tree[node].score / tree[node].visits : 0) << ")" << std::endl;
    }

    for(uint32_t i = tree[node].first_child; i < tree[node].first_child + tree[node].num_children; i++) {
        if(tree[i].visits > threshold || node == 0) print(threshold, i, distance + 1);
    }
}

chess::Move PUCTTree::sample(float temperature, xorshift &rng) {
    int num_children = tree[0].num_children;
    
    if (num_children == 1) {
        // Only one choice
        return tree[tree[0].first_child].move;
    }
    
    // Calculate Q values and find maximum for numerical stability
    float max_q = -std::numeric_limits<float>::infinity();
    for (int i = tree[0].first_child; i < tree[0].first_child + num_children; i++) {
        float q = -tree[i].score / tree[i].visits;
        if (q > max_q) max_q = q;
    }
    
    // Calculate softmax probabilities
    float sum = 0.0f;
    std::vector<float> probs(num_children);
    for (int i = 0; i < num_children; i++) {
        int node_idx = tree[0].first_child + i;
        float q = -tree[node_idx].score / tree[node_idx].visits;
        probs[i] = std::exp((q - max_q) / temperature);
        sum += probs[i];
    }
    
    // Normalize probabilities
    for (int i = 0; i < num_children; i++) {
        probs[i] /= sum;
    }
    
    // Sample using cumulative distribution
    float r = (float)rng.rand() / (float)rng.max();
    float cumulative = 0.0f;
    for (int i = 0; i < num_children; i++) {
        cumulative += probs[i];
        if (r <= cumulative) {
            return tree[tree[0].first_child + i].move;
        }
    }
    
    // Fallback (should not reach here due to floating point precision)
    return tree[tree[0].first_child + num_children - 1].move;
}