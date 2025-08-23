#pragma once
#include "includes.hpp"

namespace VALUE_NN {
    const int INPUT_SIZE = 768;
    const int HIDDEN_SIZE = 32;
    const int Q1 = 181;
    const int Q2 = 255;

    int16_t hiddenWeights[INPUT_SIZE][HIDDEN_SIZE];
    int16_t hiddenBias[HIDDEN_SIZE];

    int16_t outputWeights[HIDDEN_SIZE];
    float outputBias;

    struct Accumulator {
        int16_t data[HIDDEN_SIZE];
    };

    void init(std::string fileName);

    int16_t screlu(int16_t x) {
        return std::clamp(x, (int16_t)0, (int16_t)Q1) * std::clamp(x, (int16_t)0, (int16_t)Q1);
    }

    float eval(chess::Board &pos, Accumulator &accumulator);
};

namespace POLICY_NN {
    const int INPUT_SIZE = 768;
    const int HIDDEN_SIZE = 128;
    const int Q1 = 181;
    const int Q2 = 255;

    int16_t hiddenWeights[INPUT_SIZE][HIDDEN_SIZE];
    int16_t hiddenBias[HIDDEN_SIZE];

    int16_t outputWeights[1858][HIDDEN_SIZE];
    float outputBias[1858];

    struct Accumulator {
        int16_t data[HIDDEN_SIZE];
    };

    void init(std::string fileName);

    int16_t screlu(int16_t x) {
        return std::clamp(x, (int16_t)0, (int16_t)Q1) * std::clamp(x, (int16_t)0, (int16_t)Q1);
    }

    void CalcAccumulator(chess::Board &pos, Accumulator &accumulator);
    float eval(Accumulator &acc, int move_idx);
}