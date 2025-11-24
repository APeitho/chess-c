// chess_logic.h
#include <ctype.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "chess_logic.h"
#include "legal_moves.h"

uint64_t zobrist_keys[6][2][64];
uint64_t black_to_move_key;
uint64_t castling_keys[16]; // One key for each combination of castling rights
uint64_t en_passant_keys[8]; // One for each possible en passant file

void init_zobrist();
uint64_t compute_zobrist_hash(const GameState *game);

void initialize_board(GameState* state) {
    // Set all squares to empty first
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            state->board[i][j].type = EMPTY;
            state->board[i][j].color = NONE;
        }
    }

    // Place pawns
    for (int j = 0; j < 8; j++) {
        state->board[1][j].type = PAWN;
        state->board[1][j].color = WHITE;
        state->board[6][j].type = PAWN;
        state->board[6][j].color = BLACK;
    }

    // Place white
    state->board[0][0] = (Piece){ROOK, WHITE};
    state->board[0][1] = (Piece){KNIGHT, WHITE};
    state->board[0][2] = (Piece){BISHOP, WHITE};
    state->board[0][3] = (Piece){QUEEN, WHITE};
    state->board[0][4] = (Piece){KING, WHITE};
    state->board[0][5] = (Piece){BISHOP, WHITE};
    state->board[0][6] = (Piece){KNIGHT, WHITE};
    state->board[0][7] = (Piece){ROOK, WHITE};

    // Place black
    state->board[7][0] = (Piece){ROOK, BLACK};
    state->board[7][1] = (Piece){KNIGHT, BLACK};
    state->board[7][2] = (Piece){BISHOP, BLACK};
    state->board[7][3] = (Piece){QUEEN, BLACK};
    state->board[7][4] = (Piece){KING, BLACK};
    state->board[7][5] = (Piece){BISHOP, BLACK};
    state->board[7][6] = (Piece){KNIGHT, BLACK};
    state->board[7][7] = (Piece){ROOK, BLACK};

    state->current_turn = WHITE;

    state->white_king_moved = 0;
    state->white_kingside_rook_moved = 0;
    state->white_queenside_rook_moved = 0;
    state->black_king_moved = 0;
    state->black_kingside_rook_moved = 0;
    state->black_queenside_rook_moved = 0;

    // Initialize en passant fields to no target (-1)
    state->en_passant_target_row = -1;
    state->en_passant_target_col = -1;

    // Initialize game status and draw offers
    state->status = IN_PROGRESS;
    state->draw_offer_by = NONE;

}

void print_board(const GameState* state) {
    printf("  a b c d e f g h\n");
    for (int i = 7; i >= 0; i--) { // Loop from top to bottom row (8 to 1)
        printf("%d ", i + 1);
        for (int j = 0; j < 8; j++) {
            Piece piece = state->board[i][j];
            char piece_char;

            switch (piece.type) {
                case PAWN:  piece_char = 'P'; break;
                case ROOK:  piece_char = 'R'; break;
                case KNIGHT:    piece_char = 'N'; break;
                case BISHOP:    piece_char = 'B'; break;
                case QUEEN: piece_char = 'Q'; break;
                case KING:  piece_char = 'K'; break;
                default:    piece_char = '.'; break;
            }

            if (piece.color == BLACK && piece.type != EMPTY) {
                piece_char = tolower(piece_char);
            }
            printf("%c ", piece_char);
        }
        printf("%d\n", i + 1);
    }
    printf("  a b c d e f g h\n");
    fflush(stdout);
}

void make_move(GameState* state, const Move* move) {
    Piece piece_to_move = state->board[move->from_row][move->from_col];

    // Clear en passant target from previous move
    state->en_passant_target_row = -1;
    state->en_passant_target_col = -1;

    // En passant capture
    if (piece_to_move.type == PAWN && move->to_col == state->en_passant_target_col && move->to_row == state->en_passant_target_row) {
        int captured_row = (piece_to_move.color == WHITE) ? move->to_row - 1 : move->to_row + 1;
        state->board[captured_row][move->to_col].type = EMPTY;
        state->board[captured_row][move->to_col].color = NONE;
    }

    // Set en passant target if pawn makes double-step move
    if (piece_to_move.type == PAWN && abs(move->from_row - move->to_row) == 2) {
        int direction = (piece_to_move.color == WHITE) ? 1 : -1;
        state->en_passant_target_row = move->from_row + direction;
        state->en_passant_target_col = move->to_col;
    }

    // Check for castling
    if (piece_to_move.type == KING && abs(move->from_col - move->to_col) == 2) {
        if (move->to_col == 6) { // Kingside castling
            state->board[move->to_row][5] = state->board[move->to_row][7]; // Move rook
            state->board[move->to_row][7].type = EMPTY;
            state->board[move->to_row][7].color = NONE;
        } else { // Queenside castling
            state->board[move->to_row][3] = state->board[move->to_row][0]; // Move rook
            state->board[move->to_row][0].type = EMPTY;
            state->board[move->to_row][0].color = NONE;
        }
    }

    // Mark pieces as having moved for castling purposes
    if (piece_to_move.type == KING) {
        if (piece_to_move.color == WHITE) {
            state->white_king_moved = 1;
        } else {
            state->black_king_moved = 1;
        }
    } else if (piece_to_move.type == ROOK) {
        if (piece_to_move.color == WHITE) {
            if (move->from_row == 0 && move->from_col == 0) state->white_queenside_rook_moved = 1;
            if (move->from_row == 0 && move->from_col == 7) state->white_kingside_rook_moved = 1;
        } else { // Black Rook
            if (move->from_row == 7 && move->from_col == 0) state->black_queenside_rook_moved = 1;
            if (move->from_row == 7 && move->from_col == 7) state->black_kingside_rook_moved = 1;
        }
    }

    // Check for pawn promotion
    if (piece_to_move.type == PAWN && (move->to_row == 7 || move->to_row == 0)) {
        state->board[move->to_row][move->to_col].type = QUEEN;
        state->board[move->to_row][move->to_col].color = piece_to_move.color;
    } else {
        state->board[move->to_row][move->to_col] = piece_to_move;
    }

        // Clear the "from" square
    state->board[move->from_row][move->from_col].type = EMPTY;
    state->board[move->from_row][move->from_col].color = NONE;

    // Switch turns
    state->current_turn = (state->current_turn == WHITE) ? BLACK : WHITE;
}

// Inside your function that executes a move
void make_move(GameState *game, Move move) {
    Piece moved_piece = game->board[move.from_y][move.from_x];
    Piece captured_piece = game->board[move.to_y][move.to_x];

    // Check for pawn move or capture to reset the clock
    if (get_piece_type(moved_piece) == PAWN || captured_piece != EMPTY) {
        game->halfmove_clock = 0;
    } else {
        game->halfmove_clock++;
    }

    // ... rest of your move execution logic ...

    // After making a move, check for the draw condition
    if (game->halfmove_clock >= 100) { // 50 moves by each player
        game->status = DRAW_FIFTY_MOVE;
    }

    game->move_count++;
    uint64_t current_hash = compute_zobrist_hash(game);
    game->position_history[game->move_count] = current_hash;

    // Check for repetition
    int repetitions = 0;
    for (int i = 0; i < game->move_count; i++) {
        if (game->position_history[i] == current_hash) {
            repetitions++;
        }
    }

    if (repetitions >= 2) { // Current position is the 3rd occurrence
        game->status = DRAW_REPETITION;
    }
}

// Helper function to check for insufficient material
bool is_insufficient_material(const GameState *game) {
    int white_knights = 0, white_bishops = 0, white_others = 0;
    int black_knights = 0, black_bishops = 0, black_others = 0;
    int white_bishop_square_color = -1; // 0 for light, 1 for dark
    int black_bishop_square_color = -1;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            Piece p = game->board[y][x];
            if (p == EMPTY) continue;

            PieceType type = get_piece_type(p);
            Color color = get_piece_color(p);

            if (type == PAWN || type == ROOK || type == QUEEN) {
                if (color == WHITE) white_others++;
                else black_others++;
            } else if (type == KNIGHT) {
                if (color == WHITE) white_knights++;
                else black_knights++;
            } else if (type == BISHOP) {
                if (color == WHITE) {
                    white_bishops++;
                    white_bishop_square_color = (x + y) % 2;
                } else {
                    black_bishops++;
                    black_bishop_square_color = (x + y) % 2;
                }
            }
        }
    }

    // If any side has a pawn, rook, or queen, it's not a draw
    if (white_others > 0 || black_others > 0) return false;

    // King vs King
    if (white_knights == 0 && white_bishops == 0 && black_knights == 0 && black_bishops == 0) return true;

    // King + Knight vs King
    if ((white_knights == 1 && white_bishops == 0 && black_knights == 0 && black_bishops == 0) ||
        (white_knights == 0 && white_bishops == 0 && black_knights == 1 && black_bishops == 0)) return true;

    // King + Bishop vs King
    if ((white_bishops == 1 && white_knights == 0 && black_knights == 0 && black_bishops == 0) ||
        (white_knights == 0 && white_bishops == 0 && black_knights == 0 && black_bishops == 1)) return true;

    // King + Bishop vs King + Bishop (bishops on same colored squares)
    if (white_knights == 0 && white_bishops == 1 && black_knights == 0 && black_bishops == 1 &&
        white_bishop_square_color == black_bishop_square_color) return true;

    return false;
}
