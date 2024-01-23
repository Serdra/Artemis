#pragma once
#include "includes.h"

int printInterval = 4096*32;
int checkInterval = 4096;

float depthDecay = 0.9999;

enum Stop {
    Infinite,
    Time,
    Nodes
};

chess::Move PUCTSearch(chess::Board pos, PUCTTree &tree, Stop StopType, int stopValue);