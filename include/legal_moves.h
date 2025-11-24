#ifndef LEGAL_MOVES_H
#define LEGAL_MOVES_H

#include "chess_logic.h"

// Function prototypes for move validation
int is_legal_move(const GameState* state, const Move* move, int verbose);
int is_pawn_move_legal(const GameState* state, const Move* move);
int is_bishop_move_legal(const GameState* state, const Move* move);
int is_knight_move_legal(const GameState* state, const Move* move);
int is_rook_move_legal(const GameState* state, const Move* move);
int is_queen_move_legal(const GameState* state, const Move* move);
int is_king_move_legal(const GameState* state, const Move* move);
int is_checkmate_or_stalemate(const GameState* state, Colour color);
int is_in_check(const GameState* state, Colour color);
int is_square_attacked(const GameState* state, int row, int col, Colour by_color);


#endif // LEGAL_MOVES_H
