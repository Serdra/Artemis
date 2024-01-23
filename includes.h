#pragma once

#include <algorithm>
#include <iostream>
#include <random>
#include <unistd.h>
#include <vector>
#include <cmath>
#include <future>
#include <fstream>
#include "string.h"

#include "Chess/chess.hpp"

enum Results {
    NotGameOver,
    WhiteWin,
    BlackWin, 
    Draw
};

Results getGameResult(chess::Board &b) {
    chess::Movelist moves;
    chess::movegen::legalmoves<chess::movegen::MoveGenType::ALL>(moves, b);

    if(b.isRepetition(2)) return Results::Draw;
    if(b.halfMoveClock() > 99) return Results::Draw;
    if(moves.size() != 0) return Results::NotGameOver;
    if(b.isAttacked(b.kingSq(chess::Color::WHITE), chess::Color::BLACK)) return Results::BlackWin;
    if(b.isAttacked(b.kingSq(chess::Color::BLACK), chess::Color::WHITE)) return Results::WhiteWin;
    return Results::Draw;
}

#include "NNUE.h"
#include "NNUE.cpp"

#include "PUCT.h"
#include "PUCT.cpp"

#include "Search.h"
#include "Search.cpp"

#include "UCI.cpp"