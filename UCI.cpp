#include "includes.h"

void lower(std::string &str)
{
    for (int i = 0; i < str.length(); i++)
    {
        str[i] = std::tolower(str[i]);
    }
}
void getToken(std::istringstream &uip, std::string &token)
{
    // std::ofstream out("test.txt", std::ios_base::app);
    token.clear();
    uip >> token;
    // out << token;
    // out << " ";
    // out.close();
}

void UCI()
{
    std::string command;
    std::string token;

    int fixedDepth = -1;
    int fixedMoveTime = -1;
    uint64_t memory = 256;

    std::string valuePath;
    std::string policyPath;

    // Author//Engine info
    std::cout << "id name Artemis" << std::endl;
    std::cout << "id author Serdra" << std::endl;
    std::cout << "option name Memory type spin default 256 min 1 max 81920" << std::endl;
    std::cout << "option name ValuePath type string" << std::endl;
    std::cout << "option name PolicyPath type string" << std::endl;
    std::cout << "option name CPUCT type spin default 272" << std::endl;
    std::cout << "option name RootCPUCT type spin default 650" << std::endl;
    std::cout << "option name FPU type spin default -42" << std::endl;
    std::cout << "option name PST type spin default 147" << std::endl;
    std::cout << "uciok" << std::endl;

    // TODO: Add search and eval options
    while (true)
    {
        if (!getline(std::cin, command))
            return;

        std::istringstream uip(command, std::ios::in);
        getToken(uip, token);

        if (token == "isready")
        {
            if (valuePath == "")
            {
                VALUE_NNUE::init();
            }
            else
            {
                VALUE_NNUE::init(valuePath);
            }

            if (policyPath == "")
            {
                POLICY_NNUE::init();
            }
            else
            {
                POLICY_NNUE::init(policyPath);
            }
            //init_tables();
            // std::cout << VALUE_NNUE::eval(chess::Board("1nb1kbn1/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQ - 0 1")) << std::endl;
            std::cout << "readyok" << std::endl;
            break;
        }

        else if (token == "setoption")
        {
            getToken(uip, token);

            if (token != "name")
                return;

            getToken(uip, token);

            if (token == "Memory")
            {
                getToken(uip, token);

                if (token != "value")
                    return;

                getToken(uip, token);

                memory = std::stoi(token);
            }

            if (token == "ValuePath")
            {
                getToken(uip, token);

                if (token != "value")
                    return;

                getToken(uip, token);

                valuePath = token;
            }

            if (token == "PolicyPath")
            {
                getToken(uip, token);

                if (token != "value")
                    return;

                getToken(uip, token);

                policyPath = token;
            }

            if (token == "CPUCT")
            {
                getToken(uip, token);

                if (token != "value")
                    return;

                getToken(uip, token);

                cPUCT = std::stof(token) / 100;
            }

            if (token == "RootCPUCT")
            {
                getToken(uip, token);
                lower(token);

                if (token != "value")
                    return;

                getToken(uip, token);

                rootCPUCT = std::stof(token) / 100;
            }

            if (token == "FPU")
            {
                getToken(uip, token);
                lower(token);

                if (token != "value")
                    return;

                getToken(uip, token);

                FPU = std::stof(token) / 100;
            }

            if (token == "PST")
            {
                getToken(uip, token);
                lower(token);

                if (token != "value")
                    return;

                getToken(uip, token);

                PST = std::stof(token) / 100;
            }
        }

        else if (token == "quit")
            return;
    }

    VALUE_NNUE::Accumulator acc;
    chess::Board tmp("1nbqkbn1/2pppppr/pp5p/8/3PP3/5NPB/PPP2P1P/RNBQ1RK1 w - - 0 1");
    std::cout << VALUE_NNUE::eval(tmp, acc) << std::endl;

    PUCTTree tree;
    tree.size = std::round((1024 * 1024 * memory) / 20);
    tree.tree = new PUCTNode[tree.size];
    chess::Board pos;
    pos.set960(true);

    while (true)
    {
        if (!getline(std::cin, command))
            break;

        std::istringstream uip(command, std::ios::in);
        getToken(uip, token);
        lower(token);

        if (token == "isready")
        {
            std::cout << "readyok\n";
        }

        else if (token == "position")
        {
            getToken(uip, token);
            lower(token);
            if (token == "startpos")
            {
                pos = chess::Board();
                getToken(uip, token);
                lower(token);
            }
            if (token == "fen")
            {
                std::string fen;
                getToken(uip, token);
                // lower(token);
                while (token != "moves" && !uip.eof())
                {
                    fen.append(token);
                    fen.append(" ");
                    getToken(uip, token);
                    // lower(token);
                }
                std::cout << fen << std::endl;
                pos = chess::Board(fen);
            }
            if (token == "moves")
            {
                while (!uip.eof())
                {
                    getToken(uip, token);
                    lower(token);
                    if (!token.empty())
                    {
                        pos.makeMove(chess::uci::uciToMove(pos, token));
                    }
                }
                //std::cout << pos << std::endl;
            }
            // std::cout << pos << std::endl;
        }

        else if (token == "go")
        {
            int timeRemaining = -1;
            int incTime = -1;
            int mTime = -1;
            int nodes = -1;
            Stop stopType = Stop::Infinite;

            while (!uip.eof()) {
                getToken(uip, token);
                lower(token);
                if (token == "btime" && pos.sideToMove() == chess::Color::BLACK) {
                    getToken(uip, token);
                    timeRemaining = std::stoi(token);
                    stopType = Stop::Time;
                }
                if (token == "wtime" && pos.sideToMove() == chess::Color::WHITE) {
                    getToken(uip, token);
                    timeRemaining = std::stoi(token);
                    stopType = Stop::Time;
                }
                if (token == "binc" && pos.sideToMove() == chess::Color::BLACK) {
                    getToken(uip, token);
                    incTime = std::stoi(token);
                    stopType = Stop::Time;
                }
                if (token == "winc" && pos.sideToMove() == chess::Color::WHITE) {
                    getToken(uip, token);
                    incTime = std::stoi(token);
                    stopType = Stop::Time;
                }
                if (token == "nodes")  {
                    getToken(uip, token);
                    nodes = std::stoi(token);
                    stopType = Stop::Nodes;
                }
                if (token == "time" || token == "movetime")  {
                    getToken(uip, token);
                    mTime = std::stoi(token);
                    stopType = Stop::Time;
                }
            }

            chess::Move bestMove;

            if (stopType == Stop::Nodes)
                bestMove = PUCTSearch(pos, tree, stopType, nodes);
            if (stopType == Stop::Infinite)
                bestMove = PUCTSearch(pos, tree, stopType, 0);
            if (stopType == Stop::Time) {
                if (mTime == -1) {
                    bestMove = PUCTSearch(pos, tree, stopType, std::min(timeRemaining / 2, (timeRemaining / 30) + incTime));
                }
                else {
                    bestMove = PUCTSearch(pos, tree, stopType, mTime);
                }
            }
            std::cout << "bestmove " << chess::uci::moveToUci(bestMove) << std::endl;
        }

        else if (token == "quit") {
            break;
        }
    }
}