#pragma once
#include "includes.hpp"

struct ValuePackedBoard {
    chess::Bitboard occupied = 0;
    uint8_t pieces[16] = {0};
    uint8_t king_square = 0;
    
    // 0 if nstm win, 1 if draw, 2 if stm win
    uint8_t result = 0;
};

// Not thread safe
class DataWriter {
   private:
    std::ofstream outFile;

   public:
    uint64_t positionsWritten = 0;

    DataWriter(std::string fileName) {
        outFile = std::ofstream(fileName, std::ios::out | std::ios::binary);
    }

    void writeData(std::vector<ValuePackedBoard> &vec) {
        for(int i = 0; i < vec.size(); i++) {
            outFile.write(reinterpret_cast<char*>(&vec[i]), sizeof(vec[i]));
            positionsWritten++;
        }
    }
};