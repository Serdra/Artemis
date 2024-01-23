#pragma once
#include "includes.h"

int moveToPolicy(chess::Board &board, chess::Move move) {
    return ((int)board.at<chess::PieceType>(move.from()) * 64) + move.to().index();
}

// Credit to Zuppa D. Cipolle for letting me steal his NNUE code
// https://github.com/PGG106/Alexandria/blob/master/src/nnue.h
// He also credits Disservin and Lucex, who are both pretty cool

namespace VALUE_NNUE {
    const int size = 256;

    struct Accumulator {
        int16_t data[size] = {0};
    };

    float PST[768] = {0};
    int16_t hiddenWeights[768][size] = {0};
    int16_t hiddenBias[size] = {0};
    int16_t outputWeights[size] = {0};
    float outputBias = 0;

    void init();
    void init(std::string fileName);

    int16_t relu(int16_t x);
    
    float eval(chess::Board &pos, Accumulator &acc);
};

namespace POLICY_NNUE {
    const int size = 768;

    struct Accumulator {
        int16_t data[size] = {0};
    };

    int16_t hiddenWeights[768][size] = {0};
    int16_t hiddenBias[size] = {0};
    // It would make more sense for the following 2 indices to be inverted but it's faster this way so here we are
    int16_t outputWeights[384][size] = {0};
    float outputBias[384] = {0};

    void init();
    void init(std::string fileName);

    int16_t relu(int16_t x);
    
    void calcAccumulator(chess::Board &pos, Accumulator &acc);
    float eval(chess::Board &pos, chess::Move move, Accumulator &acc);
};