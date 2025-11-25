#ifndef CHESS_LOGIC_H
#define CHESS_LOGIC_H

#include <stdint.h> // For uint64_t

typedef enum {
    EMPTY,
    PAWN,
    ROOK,
    KNIGHT,
    BISHOP,
    QUEEN,
    KING
} PieceType;

// Represents the color of a piece.
typedef enum {
    NONE, // For empty squares
    WHITE,
    BLACK
} Colour;

// Represents a single piece on the board.
typedef struct {
    PieceType type;
    Colour color;
} Piece;

typedef struct {
    int from_row;
    int from_col;
    int to_row;
    int to_col;
    PieceType promotion_piece; // Piece to promote to. EMPTY if not a promotion.
} Move;

// Represents the current status of the game.
typedef enum {
    IN_PROGRESS,
    CHECKMATE,
    STALEMATE,
    DRAW_FIFTY_MOVE,
    DRAW_REPETITION,
    DRAW_INSUFFICIENT_MATERIAL,
    DRAW_AGREEMENT
} GameStatus;

// Maximum number of moves to track for threefold repetition.
#define MAX_GAME_MOVES 1024

// Represents the entire state of the chess game.
typedef struct {
    Piece board[8][8];      // The 8x8 chess board.
    Colour current_turn;

    // Castling availability flags. 1 if moved, 0 otherwise.
    int white_king_moved;
    int white_kingside_rook_moved;
    int white_queenside_rook_moved;
    int black_king_moved;
    int black_kingside_rook_moved;
    int black_queenside_rook_moved;

    // En passant target square. (-1, -1) if no target.
    int en_passant_target_row;
    int en_passant_target_col;

    // Counter for the 50-move rule.
    int halfmove_clock;

    // History of board hashes for threefold repetition detection.
    uint64_t position_history[MAX_GAME_MOVES];
    int move_count;

    GameStatus status;

    // Tracks which player has offered a draw.
    Colour draw_offer_by;
} GameState;

// Function prototypes
void initialize_board(GameState* state);
void print_board(const GameState* state);
void make_move(GameState* state, const Move* move);

#endif // CHESS_LOGIC_H
