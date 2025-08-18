#include "includes.hpp"

chess::Move PUCTSearch(chess::Board pos, PUCTTree &tree, StopType stop_type, uint32_t stop_value) {
    VALUE_NN::Accumulator acc;
    tree.clear();

    auto start = std::chrono::high_resolution_clock::now();

    std::chrono::time_point<std::chrono::high_resolution_clock> end_time;
    end_time = start + std::chrono::milliseconds(stop_value);

    Path path;

    uint32_t nodes = 0;
    uint32_t max_depth = 0;
    float depth = 1;

    while(true) {
        nodes++;

        chess::Board new_pos = pos;

        // Gets path
        tree.select(path, new_pos);

        // Evaluates node
        float value = tanh(VALUE_NN::eval(new_pos, acc));

        // Backs up node
        tree.backup(path, value);

        // Updates depth
        depth *= depthDecay;
        depth += float(path.size) * (1 - depthDecay);

        max_depth = std::max(max_depth, path.size);

        if(nodes % printInterval == 0 && doPrinting) {
            auto elapsed = std::chrono::high_resolution_clock::now() - start;
            float ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

            std::cout << "info depth " << std::round(depth);
            std::cout << " seldepth " << max_depth;
            std::cout << " score cp " << (int)(200 * std::atanh(std::clamp(tree[0].score / tree[0].visits, -0.995f, 0.995f)));
            std::cout << " nodes " << nodes;
            if(ms > 2) std::cout << " nps " << (int)std::round(nodes / (ms/1000));
            std::cout << " tree " << tree.head;

            tree.getPV(path, std::round(std::pow(nodes, 0.35)));
            std::cout << " pv";
            for(int i = 0; i < path.size; i++) {
                std::cout << " " << chess::uci::moveToUci(tree[path[i]].move);
            }
            std::cout << std::endl;
        }
        
        if(nodes % checkInterval == 0) {
            if(stop_type == StopType::Time && std::chrono::high_resolution_clock::now() > end_time) break;
            if(stop_type == StopType::Depth && std::round(depth) >= stop_value) break;
        }
        if(stop_type == StopType::Nodes && nodes == stop_value) break;

        if((tree.head + 218) >= tree.size) break;
    }

    if(doPrinting) {
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        float ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

        std::cout << "info depth " << std::round(depth);
        std::cout << " seldepth " << max_depth;
        std::cout << " score cp " << (int)(200 * std::atanh(std::clamp(tree[0].score / tree[0].visits, -0.995f, 0.995f)));
        std::cout << " nodes " << nodes;
        if(ms > 2) std::cout << " nps " << (int)std::round(nodes / (ms/1000));
        std::cout << " tree " << tree.head;

        tree.getPV(path, std::round(std::pow(nodes, 0.35)));
        std::cout << " pv";
        for(int i = 0; i < path.size; i++) {
            std::cout << " " << chess::uci::moveToUci(tree[path[i]].move);
        }
        std::cout << std::endl;
    }

    tree.getPV(path);

    return tree[path[0]].move;
}