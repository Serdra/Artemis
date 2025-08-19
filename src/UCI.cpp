#include "includes.hpp"

std::vector<std::string> splitString(const std::string &string, const char &delimiter) {
    std::stringstream string_stream(string);
    std::string segment;
    std::vector<std::string> seglist;

    while (std::getline(string_stream, segment, delimiter)) seglist.emplace_back(segment);

    return seglist;
}

std::string lower(const std::string& str) {
    std::string result = str;
    for (char &c : result) {
        if (c >= 'A' && c <= 'Z') {
            c = c - 'A' + 'a';
        }
    }
    return result;
}

void UCI() {
    uint32_t hash = 64;
    bool chess_960 = false;

    std::string nnue_path = "master.nnue";

    std::string command;

    std::cout << "id name Random" << std::endl;
    std::cout << "id author Serdra" << std::endl;
    std::cout << "option name Hash type spin default 64 min 16 max 16384" << std::endl;
    std::cout << "option name UCI_Chess960 type check default false" << std::endl;
    std::cout << "option name nnue_path type string default master.nnue" << std::endl;
    std::cout << "uciok" << std::endl;

    VALUE_NN::init(nnue_path);

    chess::Board board;
    board.set960(chess_960);
    PUCTTree tree((1024 * 1024 * hash) / sizeof(PUCTNode));

    while(true) {
        if(!getline(std::cin, command)) return;
        std::vector<std::string> split = splitString(command, ' ');
        if(split.size() == 0) continue;

        if(split[0] == "isready") {
            std::cout << "readyok\n";
        }

        else if (split[0] == "setoption") {
            if(split.size() != 5) continue; 
            if(split[1] != "name") continue;
            if(split[3] != "value") continue;

            // Control options here
            if(lower(split[2]) == "hash") {
                hash = std::stoi(split[4]);
                tree.resize((1024 * 1024 * hash) / sizeof(PUCTNode));
            }

            if(lower(split[2]) == "UCI_Chess960") {
                chess_960 = lower(split[4]) == "true";
            }

            if(split[2] == "nnue_path") {
                nnue_path = split[4];
                VALUE_NN::init(nnue_path);
            }
        }

        else if(split[0] == "position") {
            int i = 1;
            if(split[i] == "fen") {
                std::string fen;
                i = 2;
                while(split.size() > i && split[i] != "moves") {
                    fen.append(split[i]);
                    fen.append(" ");
                    i++;
                }
                board = chess::Board(fen, false);
            }
            else if(split[i] == "startpos") {
                board = chess::Board();
                i = 2;
            }
            if(split.size() > i && split[i] == "moves") {
                i++;
                while(split.size() > i) {
                    board.makeMove(chess::uci::uciToMove(board, split[i]));
                    i++;
                }
            }
        }

        // Not in the UGI spec but two very useful tools for debugging nonetheless
        else if(split[0] == "display") {
            std::cout << board << std::endl;
        }
        else if(split[0] == "tree") {
            if(split.size() == 1) tree.print(tree[0].visits);
            else tree.print(std::stoi(split[1]));
        }

        else if(split[0] == "go") {
            int time_remaining = -1;
            int inc_time = -1;
            int m_time = -1;
            int depth = -1;
            int nodes = -1;
            StopType stop_type = StopType::Infinite;

            int i = 1;
            while(split.size() > i) {
                if(split[i] == "wtime" && board.sideToMove() == chess::Color::WHITE) {
                    time_remaining = std::stoi(split[i + 1]);
                    stop_type = StopType::Time;
                }
                else if(split[i] == "btime" && board.sideToMove() == chess::Color::BLACK) {
                    time_remaining = std::stoi(split[i + 1]);
                    stop_type = StopType::Time;
                }
                else if(split[i] == "winc" && board.sideToMove() == chess::Color::WHITE) {
                    inc_time = std::stoi(split[i + 1]);
                    stop_type = StopType::Time;
                }
                else if(split[i] == "binc" && board.sideToMove() == chess::Color::BLACK) {
                    inc_time = std::stoi(split[i + 1]);
                    stop_type = StopType::Time;
                }

                else if(split[i] == "nodes") {
                    nodes = std::stoi(split[i + 1]);
                    stop_type = StopType::Nodes;
                }

                else if(split[i] == "time" || split[i] == "movetime") {
                    m_time = std::stoi(split[i + 1]);
                    stop_type = StopType::Time;
                }

                else if(split[i] == "depth") {
                    depth = std::stoi(split[i + 1]);
                    stop_type = StopType::Depth;
                }
                i++;
            }

            chess::Move best_move;

            if(stop_type == StopType::Infinite) best_move = PUCTSearch(board, tree, stop_type, 0);
            if(stop_type == StopType::Time) {
                if(m_time == -1) {
                    best_move = PUCTSearch(board, tree, stop_type, std::min(time_remaining / 2, (time_remaining / 30) + inc_time));
                } else {
                    best_move = PUCTSearch(board, tree, stop_type, m_time);
                }
            }
            if(stop_type == StopType::Nodes) best_move = PUCTSearch(board, tree, stop_type, nodes);
            if(stop_type == StopType::Depth) best_move = PUCTSearch(board, tree, stop_type, depth);

            std::cout << "bestmove " << chess::uci::moveToUci(best_move, chess_960) << std::endl;
        }

        else if(split[0] == "quit") {
            break;
        }
    }
}