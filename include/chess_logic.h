// chess_logic

#ifndef CHESS_LOGIC_H
#define CHESS_LOGIC_H

#include <stdint.h> // For uint64_t

// Enumeration for piece types
typedef enum {
    EMPTY,
    PAWN,
    ROOK,
    KNIGHT,
    BISHOP,
    QUEEN,
    KING
} PieceType;

// Enumeration for piece colours

typedef enum {
    NONE, // For empty sqaures
    WHITE,
    BLACK
} Colour;

// Structure to represent a single piece

typedef struct {
    PieceType type;
    Colour color;
} Piece;

typedef struct {
    int from_row;
    int from_col;
    int to_row;
    int to_col;
} Move;

// Enumeration for game status
typedef enum {
    IN_PROGRESS,
    CHECKMATE,
    STALEMATE,
    DRAW_FIFTY_MOVE,
    DRAW_REPETITION,
    DRAW_INSUFFICIENT_MATERIAL,
    DRAW_AGREEMENT
} GameStatus;

// Define a max moves for threefold repetition history
#define MAX_GAME_MOVES 1024

// Structure to represent the chessboard and game state
typedef struct {
    Piece board[8][8]; // 8x8 grid-board
    Colour current_turn;

    //Castling rights (1 means moved, 0 means not moved)
    int white_king_moved;
    int white_kingside_rook_moved;
    int white_queenside_rook_moved;
    int black_king_moved;
    int black_kingside_rook_moved;
    int black_queenside_rook_moved;

    // En passant
    int en_passant_target_row;
    int en_passant_target_col;

    // 50 move draw clock
    int halfmove_clock;

    // Threefold Repetition
    uint64_t position_history[MAX_GAME_MOVES]; // e.g., #define MAX_GAME_MOVES 1024
    int move_count;

    // Game status
    GameStatus status;

    // Draw offer tracking
    Colour draw_offer_by;
} GameState;


// Created prototypes
void initialize_board(GameState* state);
void print_board(const GameState* state);
void make_move(GameState* state, const Move* move);
//More functions for move validation, etc., will go here

#endif // CHESS_LOGIC_H
