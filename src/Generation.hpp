#pragma once
#include "includes.hpp"

// There's a 50% chance of an additional ply
const int NUM_STARTING_PLY = 5;
const int NODES = 7000;

const float TEMPERATURE = 0.125;
const float TEMPERATURE_PLY = 6;

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
ValuePackedBoard packValueBoard(chess::Board &board);

PolicyPackedBoard packPolicyBoard(chess::Board &board);

// The main data generation functions. This can be spawned as many times as desired
void generateValueData(DataWriter &writer, std::mutex &mtx, int &interval, WDL &wdl, xorshift rng);

void generatePolicyData(DataWriter &writer, std::mutex &mtx, int &interval, uint64_t &rejected, xorshift rng);