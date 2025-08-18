#pragma once
#include "includes.hpp"

enum StopType {
    Infinite,
    Time,
    Nodes,
    Depth
};

bool doPrinting = true;
uint32_t printInterval = 4096*32;
uint32_t checkInterval = 512;

float depthDecay = 0.999;

// The primary search loop
chess::Move PUCTSearch(chess::Board pos, PUCTTree &tree, StopType stop_type, uint32_t stop_value);