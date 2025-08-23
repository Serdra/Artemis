#pragma once
#include "includes.hpp"

const int MAX_POLICY_MOVES = 64;

struct ValuePackedBoard {
    chess::Bitboard occupied = 0;
    uint8_t pieces[16] = {0};
    uint8_t king_square = 0;
    
    // 0 if nstm win, 1 if draw, 2 if stm win
    uint8_t result = 0;
};

struct PolicyPackedMove {
    uint16_t v1 = 0;
    uint8_t v2 = 0;

    void write(uint16_t index, uint16_t visits) {
        uint32_t value = visits * 1858 + index;
        v1 = value >> 8;
        v2 = value & 0xff;
    }
} __attribute__((__packed__));

struct PolicyPackedBoard {
    chess::Bitboard occupied = 0;

    PolicyPackedMove policy[MAX_POLICY_MOVES];
    uint8_t pieces[16] = {0};
    uint8_t king_square = 0;
}; // 224 bytes

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

    void writeData(std::vector<PolicyPackedBoard> &vec) {
        for(int i = 0; i < vec.size(); i++) {
            outFile.write(reinterpret_cast<char*>(&vec[i]), sizeof(vec[i]));
            positionsWritten++;
        }
    }
};