#include "includes.h"

void VALUE_NNUE::init() {
    for(int i = 0; i < 768; i++) {
        for(int j = 0; j < VALUE_NNUE::size; j++) {
            hiddenWeights[i][j] = 20 - (rand() % 41);
        }
    }

    for(int i = 0; i < VALUE_NNUE::size; i++) {
        hiddenBias[i] = 20 - (rand() % 41);
        outputWeights[i] = 20 - (rand() % 41);
    }
    outputBias = 20 - (rand() % 41);
}

void VALUE_NNUE::init(std::string fileName) {
    float temp;
    std::ifstream inFile(fileName, std::ios::in | std::ios::binary);

    for(int i = 0; i < 384; i++) {
        inFile.read((char*) &temp, sizeof(temp));
        PST[i] = 128 * 128 * temp;
    }
    // Creates this for easy indexing into opponent's features
    for(int i = 384; i < 768; i++) {
        int square = i % 64;
        int piece = (i - 384 - square) / 64;
        square ^= 56;
        int feature = (piece * 64) + square;
        PST[i] = -PST[feature];
    }

    for(int i = 0; i < VALUE_NNUE::size; i++) {
        inFile.read((char*) &temp, sizeof(temp));
        hiddenBias[i] = std::round(128 * temp);
    }

    for(int i = 0; i < 768; i++) {
        for(int j = 0; j < VALUE_NNUE::size; j++) {
            inFile.read((char*) &temp, sizeof(temp));
            hiddenWeights[i][j] = std::round(128 * temp);
        }
    }

    inFile.read((char*) &temp, sizeof(temp));
    outputBias = 128 * 128 * temp;

    for(int i = 0; i < VALUE_NNUE::size; i++) {
        inFile.read((char*) &temp, sizeof(temp));
        outputWeights[i] = std::round(128 * temp);
    }
}

int16_t VALUE_NNUE::relu(int16_t x) {
    return std::max(x, (int16_t)0);
}

float VALUE_NNUE::eval(chess::Board &pos, VALUE_NNUE::Accumulator &acc) {
    chess::Bitboard occ = pos.occ();
    float output = outputBias;

    // Applies the hidden bias
    memcpy(acc.data, hiddenBias, sizeof(int16_t(0)) * size);

    // Calculates the first layer
    while(occ) {
        int sq = occ.pop();
        chess::Piece piece = pos.at(sq);

        // Flip position and color if necessary
        if(pos.sideToMove() == chess::Color::BLACK) {
            sq ^= 56;
            piece = chess::Piece(piece.type(), ~piece.color());
        }
        // Calculates the active feature
        int feature = ((int)piece * 64) + sq;

        // Adds feature
        for(int i = 0; i < VALUE_NNUE::size; i++) {
            acc.data[i] += hiddenWeights[feature][i];
        }

        output += PST[feature];
    }

    // Applies relu
    for(int i = 0; i < VALUE_NNUE::size; i++) {
        acc.data[i] = relu(acc.data[i]);
    }

    for(int i = 0; i < VALUE_NNUE::size; i++) {
        output += acc.data[i] * outputWeights[i];
    }
    return tanh(output / (200.f * 128.f * 128.f));
}

//
// ####################################################################################################################
//

void POLICY_NNUE::init() {
    for (int i = 0; i < 768; i++) {
        for (int j = 0; j < size; j++) {
            hiddenWeights[i][j] = 20 - (rand() % 41);
        }
    }

    for (int i = 0; i < size; i++) {
        hiddenBias[i] = 20 - (rand() % 41);
    }

    for (int i = 0; i < 384; i++) {
        for (int j = 0; j < size; j++) {
            outputWeights[i][j] = 20 - (rand() % 41);
        }
    }

    for (int i = 0; i < 384; i++) {
        outputBias[i] = 20 - (rand() % 41);
    }
}

void POLICY_NNUE::init(std::string fileName) {
    float temp;
    std::ifstream inFile(fileName, std::ios::in | std::ios::binary);

    for(int i = 0; i < POLICY_NNUE::size; i++) {
        inFile.read((char*) &temp, sizeof(temp));
        hiddenBias[i] = std::round(128 * temp);
    }

    for(int i = 0; i < 768; i++) {
        for(int j = 0; j < POLICY_NNUE::size; j++) {
            inFile.read((char*) &temp, sizeof(temp));
            hiddenWeights[i][j] = std::round(128 * temp);
        }
    }

    for(int i = 0; i < 384; i++) {
        inFile.read((char*) &temp, sizeof(temp));
        outputBias[i] = temp * 128 * 128;
    }

    for(int i = 0; i < 384; i++) {
        for(int j = 0; j < POLICY_NNUE::size; j++) {
            inFile.read((char*) &temp, sizeof(temp));
            outputWeights[i][j] = std::round(128 * temp);
        }
    }
}

int16_t POLICY_NNUE::relu(int16_t x) {
    return std::max(x, (int16_t)0);
}

void POLICY_NNUE::calcAccumulator(chess::Board &pos, POLICY_NNUE::Accumulator &acc) {
    chess::Bitboard occ = pos.occ();

    // Resets and applies hidden bias
    memcpy(acc.data, hiddenBias, sizeof(int16_t) * size);

    // Calculates the first layer
    while(occ) {
        int sq = occ.pop();
        chess::Piece piece = pos.at(sq);

        // Flip position and color if necessary
        if(pos.sideToMove() == chess::Color::BLACK) {
            sq ^= 56;
            piece = chess::Piece(piece.type(), ~piece.color());
        }
        // Calculates the active feature
        int feature = ((int)piece * 64) + sq;

        // Adds feature
        for(int i = 0; i < POLICY_NNUE::size; i++) {
            acc.data[i] += hiddenWeights[feature][i];
        }
    }

    for(int i = 0; i < POLICY_NNUE::size; i++) {
        acc.data[i] = relu(acc.data[i]);
    }
}

float POLICY_NNUE::eval(chess::Board &pos, chess::Move move, Accumulator &acc) {
    int idx = moveToPolicy(pos, move);
    float output = outputBias[idx];
    for(int i = 0; i < POLICY_NNUE::size; i++) {
        output += acc.data[i] * outputWeights[idx][i];
    }
    return output / (128 * 128);
}