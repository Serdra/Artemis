#include "includes.hpp"

int main(int, char**){
    srand(time(0));
    std::string input;
    std::getline(std::cin, input);

    if (input == "uci") UCI();

    else if(input == "datagen") {
        VALUE_NN::init("master.nnue");

        int seed = rand();
        doPrinting = false;

        DataWriter writer("iteration3.bin");
        std::mutex mtx;
        int interval = 1;
        int num_threads = 8;
        WDL wdl;

        std::vector<std::thread> threads;

        // generateData(writer, mtx, interval, wdl, xorshift(seed));

        for(int i = 0; i < num_threads; i++) {
            threads.push_back(std::thread(generateData, std::ref(writer), std::ref(mtx), std::ref(interval), std::ref(wdl), xorshift(seed+i)));

            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(i * 2, &cpuset);

            int rc = pthread_setaffinity_np(threads[i].native_handle(), sizeof(cpu_set_t), &cpuset);
        }
        for(int i = 0; i < num_threads; i++) {
            threads[i].join();
        }
    }

    else {
        chess::Board b1("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        chess::Board b2("rnb1kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        chess::Board b3("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNB1KBNR w KQkq - 0 1");
        chess::Board b4("rnbqkbnr/pppppppp/8/8/2BPP3/5N2/PPP2PPP/RNBQR1K1 w kq - 0 1");

        VALUE_NN::Accumulator acc;

        std::cout << 200 * VALUE_NN::eval(b1, acc) << std::endl;
        std::cout << 200 * VALUE_NN::eval(b2, acc) << std::endl;
        std::cout << 200 * VALUE_NN::eval(b3, acc) << std::endl;
        std::cout << 200 * VALUE_NN::eval(b4, acc) << std::endl;
    }
}