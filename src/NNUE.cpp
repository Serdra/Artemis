#include "includes.hpp"

void VALUE_NN::init(std::string fileName) {
    float temp;
    std::ifstream inFile(fileName, std::ios::in | std::ios::binary);

    for(int i = 0; i < INPUT_SIZE; i++) {
        for(int j = 0; j < HIDDEN_SIZE; j++) {
            inFile.read((char*) &temp, sizeof(temp));
            hiddenWeights[i][j] = std::round(Q1 * temp);
        }
    }

    for(int i = 0; i < HIDDEN_SIZE; i++) {
        inFile.read((char*) &temp, sizeof(temp));
        hiddenBias[i] = std::round(Q1 * temp);
    }

    for(int i = 0; i < HIDDEN_SIZE; i++) {
        inFile.read((char*) &temp, sizeof(temp));
        outputWeights[i] = std::round(Q2 * temp);
    }

    inFile.read((char*) &temp, sizeof(temp));
    outputBias = std::round(Q1 * Q1 * Q2 * temp);
}

float VALUE_NN::eval(chess::Board &pos, Accumulator &accumulator) {
    std::pair<chess::GameResultReason, chess::GameResult> result = pos.isGameOver();
    if(result.second != chess::GameResult::NONE) {
        if(result.second == chess::GameResult::DRAW) return 0;
        if(result.second == chess::GameResult::WIN) return 1;
        return -1;
    }

    chess::Bitboard occ = pos.occ();
    float output = outputBias;

    // Applies hidden bias
    memcpy(accumulator.data, hiddenBias, sizeof(int16_t(0)) * HIDDEN_SIZE);

    // Calculates the first layer
    while(occ) {
        int sq = occ.pop();
        chess::Piece piece = pos.at(sq);

        if(pos.sideToMove() == chess::Color::BLACK) {
            sq ^= 56;
            piece = chess::Piece(piece.type(), ~piece.color());
        }

        if((pos.kingSq(pos.sideToMove()).index() % 8) < 4) sq ^= 7;

        int feature = ((int)piece * 64) + sq;

        for(int i = 0; i < HIDDEN_SIZE; i++) {
            accumulator.data[i] += hiddenWeights[feature][i];
        }
    }

    // Applies screlu
    for(int i = 0; i < HIDDEN_SIZE; i++) {
        accumulator.data[i] = screlu(accumulator.data[i]);
    }

    // Calculates final layer
    for(int i = 0; i < HIDDEN_SIZE; i++) {
        output += accumulator.data[i] * outputWeights[i];
    }

    return tanh(output / (float)(Q1 * Q1 * Q2));
}