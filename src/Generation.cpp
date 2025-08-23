#include "includes.hpp"

ValuePackedBoard packValueBoard(chess::Board &board) {
    ValuePackedBoard result;

    chess::Bitboard occ = board.all();

    result.king_square = board.kingSq(board.sideToMove()).index();
    if(board.sideToMove() == chess::Color::BLACK) {
        occ = flipVertical(occ);
        result.result = 1;
        result.king_square ^= 56;
    }

    result.occupied = occ;

    int idx = 0;

    while(occ) {
        int sq = occ.pop();
        chess::Piece pc = board.at(sq);

        if(board.sideToMove() == chess::Color::BLACK) {
            sq ^= 56;
            pc = board.at(sq);
            pc = chess::Piece(!pc.color(), pc.type());
        }

        if(idx % 2 == 0) result.pieces[idx /2] = (int)pc.internal();
        else result.pieces[idx / 2] += (int)pc.internal() << 4;
        idx++;
    }
    return result;
}

PolicyPackedBoard packPolicyBoard(chess::Board &board) {
    PolicyPackedBoard result;

    chess::Bitboard occ = board.all();

    result.king_square = board.kingSq(board.sideToMove()).index();
    if(board.sideToMove() == chess::Color::BLACK) {
        occ = flipVertical(occ);
        result.king_square ^= 56;
    }

    result.occupied = occ;

    int idx = 0;

    while(occ) {
        int sq = occ.pop();
        chess::Piece pc = board.at(sq);

        if(board.sideToMove() == chess::Color::BLACK) {
            sq ^= 56;
            pc = board.at(sq);
            pc = chess::Piece(!pc.color(), pc.type());
        }

        if(idx % 2 == 0) result.pieces[idx /2] = (int)pc.internal();
        else result.pieces[idx / 2] += (int)pc.internal() << 4;
        idx++;
    }
    return result;
}

chess::Board generateStartingPosition(xorshift &rng) {
    while(true) {
        chess::Board board;

        int num_moves = NUM_STARTING_PLY + rng.rand() % 2;

        for(int i = 0; i < num_moves && board.isGameOver().first == chess::GameResultReason::NONE; i++) {
            chess::Movelist moves;
            chess::movegen::legalmoves(moves, board);
            board.makeMove(moves[rng.rand() % moves.size()]);
        }
        if(board.isGameOver().first == chess::GameResultReason::NONE && !board.inCheck()) {
            if(rng.rand() % 2 == 0) board.makeNullMove();
            return board;
        }
    }
}

void generateValueData(DataWriter &writer, std::mutex &mtx, int &interval, WDL &wdl, xorshift rng) {
    PUCTTree tree(NODES * 20);

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<ValuePackedBoard> gameData;

    chess::Board board = generateStartingPosition(rng);
    int num_ply = 0;

    while(true) {
        if(board.isGameOver().first != chess::GameResultReason::NONE) {
            uint8_t result = 1;
            if(board.isGameOver().second == chess::GameResult::WIN  && board.sideToMove() == chess::Color::WHITE) result = 2;
            if(board.isGameOver().second == chess::GameResult::LOSE && board.sideToMove() == chess::Color::BLACK) result = 2;
            if(board.isGameOver().second == chess::GameResult::LOSE && board.sideToMove() == chess::Color::WHITE) result = 0;
            if(board.isGameOver().second == chess::GameResult::WIN  && board.sideToMove() == chess::Color::BLACK) result = 0;

            for(int i = 0; i < gameData.size(); i++) {
                if(gameData[i].result == 1) gameData[i].result = 2 - result;
                else gameData[i].result = result;
            }

            mtx.lock();
            if(result == 2) wdl.whiteWins++;
            else if(result == 0) wdl.blackWins++;
            else wdl.draws++;

            writer.writeData(gameData);

            if(writer.positionsWritten >= (interval * 50000)) {
                auto elapsed = std::chrono::high_resolution_clock::now() - start;
                float u = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

                std::cout << "Positions generated: " << writer.positionsWritten << std::endl;
                std::cout << "Time Taken: " << u/1000 << "s" << std::endl;
                std::cout << "Speed: " << (writer.positionsWritten) / (u/1000) << "p/s" << std::endl;
                std::cout << "White wins: " << wdl.whiteWins << std::endl;
                std::cout << "Black wins: " << wdl.blackWins << std::endl;
                std::cout << "Draws: " << wdl.draws << std::endl << std::endl;
                interval++;
            }
            mtx.unlock();

            gameData.clear();
            num_ply = 0;

            board = generateStartingPosition(rng);
        }
        
        ValuePackedBoard pb = packValueBoard(board);
        gameData.push_back(pb);

        chess::Move bestmove = PUCTSearch(board, tree, StopType::Nodes, NODES);
        if(num_ply < TEMPERATURE_PLY) {
            bestmove = tree.sample(TEMPERATURE, rng);
        }
        num_ply++;
        board.makeMove(bestmove);
    }
}

void generatePolicyData(DataWriter &writer, std::mutex &mtx, int &interval, uint64_t &rejected, xorshift rng) {
    PUCTTree tree(NODES * 20);

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<PolicyPackedBoard> gameData;

    chess::Board board = generateStartingPosition(rng);
    int num_ply = 0;

    while(true) {
        if(board.isGameOver().first != chess::GameResultReason::NONE) {
            mtx.lock();

            writer.writeData(gameData);

            if(writer.positionsWritten >= (interval * 50000)) {
                auto elapsed = std::chrono::high_resolution_clock::now() - start;
                float u = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

                std::cout << "Positions generated: " << writer.positionsWritten << std::endl;
                std::cout << "Time Taken: " << u/1000 << "s" << std::endl;
                std::cout << "Speed: " << (writer.positionsWritten) / (u/1000) << "p/s" << std::endl;
                std::cout << "Rejected positions: " << rejected << std::endl << std::endl;
                interval++;
            }
            mtx.unlock();

            gameData.clear();
            num_ply = 0;

            board = generateStartingPosition(rng);
        }
        chess::Move bestmove = PUCTSearch(board, tree, StopType::Nodes, NODES);
        if(num_ply < TEMPERATURE_PLY) {
            bestmove = tree.sample(TEMPERATURE, rng);
        }
        num_ply++;

        chess::Movelist moves;
        chess::movegen::legalmoves(moves, board);

        PolicyPackedBoard pb = packPolicyBoard(board);
        if(tree[0].num_children <= MAX_POLICY_MOVES) {
            for(int i = 0; i < tree[0].num_children; i++) {
                int move_idx = mapping::getMoveIndex(
                    moves[i], 
                    board.sideToMove() == chess::Color::BLACK, 
                    (board.kingSq(board.sideToMove()).index() % 8) < 4
                );


                pb.policy[i].write(move_idx, tree[1 + i].visits);
            }
        } else {
            mtx.lock();
            rejected++;
            mtx.unlock();
        }
        gameData.push_back(pb);

        board.makeMove(bestmove);
    }
}