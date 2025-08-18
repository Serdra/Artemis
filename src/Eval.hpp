#pragma once
#include "includes.hpp"

float HCE(chess::Board &board) {
    std::pair<chess::GameResultReason, chess::GameResult> result = board.isGameOver();
    if(result.second != chess::GameResult::NONE) {
        if(result.second == chess::GameResult::DRAW) return 0;
        if(result.second == chess::GameResult::WIN) return 1;
        return -1;
    }

    int eval = 0;

    eval += 100 * board.pieces(chess::PieceType::PAWN, board.sideToMove()).count();
    eval += 300 * board.pieces(chess::PieceType::KNIGHT, board.sideToMove()).count();
    eval += 320 * board.pieces(chess::PieceType::BISHOP, board.sideToMove()).count();
    eval += 500 * board.pieces(chess::PieceType::ROOK, board.sideToMove()).count();
    eval += 900 * board.pieces(chess::PieceType::QUEEN, board.sideToMove()).count();

    eval -= 100 * board.pieces(chess::PieceType::PAWN, ~board.sideToMove()).count();
    eval -= 300 * board.pieces(chess::PieceType::KNIGHT, ~board.sideToMove()).count();
    eval -= 320 * board.pieces(chess::PieceType::BISHOP, ~board.sideToMove()).count();
    eval -= 500 * board.pieces(chess::PieceType::ROOK, ~board.sideToMove()).count();
    eval -= 900 * board.pieces(chess::PieceType::QUEEN, ~board.sideToMove()).count();

    return tanh(eval / 200);
}