#pragma once
#include "includes.hpp"

// There's a 50% chance of an additional ply
const int NUM_STARTING_PLY = 6;
const int NODES = 10000;

const float TEMPERATURE = 0.125;
const float TEMPERATURE_PLY = 12;

struct WDL {
    uint64_t whiteWins = 0;
    uint64_t blackWins = 0;
    uint64_t draws = 0;
};

// Flips a bitboard vertically
chess::Bitboard flipVertical(chess::Bitboard x) {
    return  ( (x << 56)                           ) |
            ( (x << 40) & (0x00ff000000000000ull) ) |
            ( (x << 24) & (0x0000ff0000000000ull) ) |
            ( (x <<  8) & (0x000000ff00000000ull) ) |
            ( (x >>  8) & (0x00000000ff000000ull) ) |
            ( (x >> 24) & (0x0000000000ff0000ull) ) |
            ( (x >> 40) & (0x000000000000ff00ull) ) |
            ( (x >> 56) );
}
// Packs a board into a compressed format
ValuePackedBoard packBoard(chess::Board &board);

// Generates a random starting position. In the future this will incorporate frc and dfrc positions
chess::Board generateStartingPosition(xorshift &rng);

// The main data generation function. This can be spawned as many times as desired
void generateData(DataWriter &writer, std::mutex &mtx, int &interval, WDL &wdl, xorshift rng);