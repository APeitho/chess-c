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
    // Clear the board.
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            state->board[i][j].type = EMPTY;
            state->board[i][j].color = NONE;
        }
    }

    // Place pawns.
    for (int j = 0; j < 8; j++) {
        state->board[1][j].type = PAWN;
        state->board[1][j].color = WHITE;
        state->board[6][j].type = PAWN;
        state->board[6][j].color = BLACK;
    }

    // Place white major and minor pieces.
    state->board[0][0] = (Piece){ROOK, WHITE};
    state->board[0][1] = (Piece){KNIGHT, WHITE};
    state->board[0][2] = (Piece){BISHOP, WHITE};
    state->board[0][3] = (Piece){QUEEN, WHITE};
    state->board[0][4] = (Piece){KING, WHITE};
    state->board[0][5] = (Piece){BISHOP, WHITE};
    state->board[0][6] = (Piece){KNIGHT, WHITE};
    state->board[0][7] = (Piece){ROOK, WHITE};

    // Place black major and minor pieces.
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

    // No en passant target initially.
    state->en_passant_target_row = -1;
    state->en_passant_target_col = -1;

    // Set initial game status.
    state->status = IN_PROGRESS;
    state->draw_offer_by = NONE;

}

void print_board(const GameState* state) {
    printf("  a b c d e f g h\n");
    for (int i = 7; i >= 0; i--) { // Print from rank 8 down to 1.
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

    // Reset en passant target from the previous turn.
    state->en_passant_target_row = -1;
    state->en_passant_target_col = -1;

    // Handle en passant capture: the captured pawn is not on the 'to' square.
    if (piece_to_move.type == PAWN && move->to_col == state->en_passant_target_col && move->to_row == state->en_passant_target_row) {
        int captured_row = (piece_to_move.color == WHITE) ? move->to_row - 1 : move->to_row + 1;
        state->board[captured_row][move->to_col].type = EMPTY;
        state->board[captured_row][move->to_col].color = NONE;
    }

    // Set a new en passant target if a pawn makes a two-square advance.
    if (piece_to_move.type == PAWN && abs(move->from_row - move->to_row) == 2) {
        int direction = (piece_to_move.color == WHITE) ? 1 : -1;
        state->en_passant_target_row = move->from_row + direction; // The square behind the pawn.
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

    // Update castling rights if a king or rook moves for the first time.
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

    // Handle pawn promotion.
    if (piece_to_move.type == PAWN && (move->to_row == 7 || move->to_row == 0)) {
        PieceType promotion_type = (move->promotion_piece != EMPTY) ? move->promotion_piece : QUEEN;
        state->board[move->to_row][move->to_col].type = promotion_type;
        state->board[move->to_row][move->to_col].color = piece_to_move.color;
    } else {
        state->board[move->to_row][move->to_col] = piece_to_move;
    }

    // Clear the original square.
    state->board[move->from_row][move->from_col].type = EMPTY;
    state->board[move->from_row][move->from_col].color = NONE;

    // Switch player turn.
    state->current_turn = (state->current_turn == WHITE) ? BLACK : WHITE;
}
