#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "legal_moves.h"

int is_legal_move(const GameState* state, const Move* move, int verbose) {

    // 1. Check if the move is within board boundaries.
    if (move->from_row < 0 || move->from_row > 7 || move->from_col < 0 || move->from_col > 7 || move->to_row < 0 || move->to_col > 7 || move->to_col < 0 || move->to_col > 7) {
        if (verbose) printf("Error: Move is outside the board.\n");
        return 0;
    }

    Piece piece_to_move = state->board[move->from_row][move->from_col];

    // 2. Check if there is a piece on the starting square.
    if (piece_to_move.type == EMPTY) {
        if (verbose) printf("Error: No piece on the starting square.\n");
        return 0;
    }

    // 3. Check if the piece belongs to the current player.
    if (piece_to_move.color != state->current_turn) {
        if (verbose) printf("Error: It's not your turn to move that piece.\n");
        return 0;
    }

    Piece destination_piece = state->board[move->to_row][move->to_col];

    // 4. Check for capturing your own piece ("friendly fire").
    if (destination_piece.color == piece_to_move.color) {
        if (verbose) printf("Error: Cannot capture your own piece.\n");
        return 0;
    }

    int piece_move_is_legal;
    // 5. Check if the move follows the piece's specific movement rules.
    switch (piece_to_move.type) {
        case PAWN:
            piece_move_is_legal = is_pawn_move_legal(state, move);
            break;
        case ROOK:
            piece_move_is_legal = is_rook_move_legal(state, move);
            break;
        case KNIGHT:
            piece_move_is_legal = is_knight_move_legal(state, move);
            break;
        case BISHOP:
            piece_move_is_legal = is_bishop_move_legal(state, move);
            break;
        case QUEEN:
            piece_move_is_legal = is_queen_move_legal(state, move);
            break;
        case KING:
            piece_move_is_legal = is_king_move_legal(state, move);
            break;
        default:
            return 0; // Should not happen.
    }

    if (!piece_move_is_legal) {
        return 0;
    }

    // 6. Simulate the move on a temporary board to see if it leaves the king in check.
    // This is a crucial and final validation step.
    GameState temp_state = *state;

    temp_state.board[move->to_row][move->to_col] = temp_state.board[move->from_row][move->from_col];
    temp_state.board[move->from_row][move->from_col].type = EMPTY;
    temp_state.board[move->from_row][move->from_col].color = NONE;

    // If the king is in check after the move, the move is illegal.
    if (is_in_check(&temp_state, state->current_turn)) {
        if (verbose) printf("Error: that move leaves your king in check.\n");
        return 0;
    }
    return 1; // Move is legal
}

int is_pawn_move_legal(const GameState* state, const Move* move) {
    int from_row = move->from_row;
    int from_col = move->from_col;
    int to_row = move->to_row;
    int to_col = move->to_col;

    Piece piece = state->board[from_row][from_col];
    int direction = (piece.color == WHITE) ? 1 : -1;

    // Single-step forward move.
    if (from_col == to_col && to_row == from_row + direction) {
        // Destination must be empty.
        if (state->board[to_row][to_col].type == EMPTY) {
            return 1;
        }
    }

    // Double-step initial move.
    if (from_col == to_col && to_row == from_row + 2 * direction) {
        // Must be on the starting rank.
        int starting_row = (piece.color == WHITE) ? 1 : 6;
        if (from_row == starting_row) {
            // Path must be clear.
            if (state->board[to_row][to_col].type == EMPTY && state->board[from_row + direction][from_col].type == EMPTY) {
               return 1;
            }
        }
    }

    // Diagonal capture.
    if (abs(from_col - to_col) == 1 && to_row == from_row + direction) {
        // Regular capture: destination must have an opponent's piece.
        if (state->board[to_row][to_col].type != EMPTY && state->board[to_row][to_col].color != piece.color) {
            return 1;
        }

        // En passant capture: destination must be the en passant target square.
        if (state->en_passant_target_row == to_row && state->en_passant_target_col == to_col) {
            int enemy_pawn_row = (piece.color == WHITE) ? to_row - 1 : to_row + 1;
            if (enemy_pawn_row >= 0 && enemy_pawn_row <= 7) {
                Piece enemy_piece = state->board[enemy_pawn_row][to_col];
                if (enemy_piece.type == PAWN && enemy_piece.color != piece.color) {
                    return 1;
                }
            }
        }
    }

    // If none of the above pawn moves are valid, the move is illegal.
    return 0;
}


int is_knight_move_legal(const GameState* state, const Move* move) {
    int from_row = move->from_row;
    int from_col = move->from_col;
    int to_row = move->to_row;
    int to_col = move->to_col;

    int row_diff = abs(from_row - to_row);
    int col_diff = abs(from_col - to_col);

    // A knight moves in an 'L' shape: 2 squares in one direction, 1 in a perpendicular direction.
    if ((row_diff == 2 && col_diff == 1) || (row_diff == 1 && col_diff == 2)) {
        return 1;
    }

    return 0;
}


int is_bishop_move_legal(const GameState* state, const Move* move) {
    int from_row = move->from_row;
    int from_col = move->from_col;
    int to_row = move->to_row;
    int to_col = move->to_col;

    // A bishop move must be diagonal.
    if (abs(from_row - to_row) != abs(from_col - to_col)) {
        return 0;
    }

    // Determine the direction of movement.
    int row_dir = (to_row > from_row) ? 1 : -1;
    int col_dir = (to_col > from_col) ? 1 : -1;

    // Check for obstructions along the path.
    int row = from_row + row_dir;
    int col = from_col + col_dir;
    while (row != to_row) {
        if (state->board[row][col].type != EMPTY) {
            return 0; // Path is blocked.
        }
        row += row_dir;
        col += col_dir;
    }

    return 1;
}


int is_rook_move_legal(const GameState* state, const Move* move) {
    int from_row = move->from_row;
    int from_col = move->from_col;
    int to_row = move->to_row;
    int to_col = move->to_col;

    // A rook move must be straight (horizontal or vertical).
    if (from_row != to_row && from_col != to_col) {
        return 0;
    }

    // Check for obstructions along the path.
    if (from_row == to_row) { // Horizontal move
        int start_col = (from_col < to_col) ? from_col + 1 : to_col + 1;
        int end_col = (from_col < to_col) ? to_col : from_col;
        for (int col = start_col; col < end_col; col++) {
            if (state->board[from_row][col].type != EMPTY) {
                return 0;
            }
        }
    } else {
        int start_row = (from_row < to_row) ? from_row + 1 : to_row + 1;
        int end_row = (from_row < to_row) ? to_row : from_row;
        for (int row = start_row; row < end_row; row++) {
            if (state->board[row][from_col].type != EMPTY) {
                return 0; // Path is blocked.
            }
        }
    }

    return 1;
}


int is_queen_move_legal(const GameState* state, const Move* move) {
    if (is_rook_move_legal(state, move) || is_bishop_move_legal(state, move)) {
        return 1;
    }
    return 0;
}


int is_king_move_legal(const GameState* state, const Move* move) {
    int from_row = move->from_row;
    int from_col = move->from_col;
    int to_row = move->to_row;
    int to_col = move->to_col;

    Colour color = state->board[from_row][from_col].color;
    Colour opponent_color = (color == WHITE) ? BLACK : WHITE;

    int row_diff = abs(from_row - to_row);
    int col_diff = abs(from_col - to_col);

    // Standard king move: one square in any direction.
    if (row_diff <= 1 && col_diff <= 1) {
        return 1;
    }

    // Castling logic (King moves two squares horizontally)
    if (row_diff == 0 && col_diff == 2) {
        int king_start_row = (color == WHITE) ? 0 : 7;

        if (from_row != king_start_row || from_col != 4) return 0;

        if (color == WHITE) {
            if (state->white_king_moved) return 0;
            if (to_col == 6) { // Kingside
                if (state->white_kingside_rook_moved) return 0;

                // Path must be clear and squares king travels over must not be attacked.
                if (state->board[king_start_row][5].type != EMPTY || state->board[king_start_row][6].type != EMPTY) return 0;
                if (is_square_attacked(state, king_start_row, 4, opponent_color) ||
                    is_square_attacked(state, king_start_row, 5, opponent_color) ||
                    is_square_attacked(state, king_start_row, 6, opponent_color)) {
                    return 0;
                }
                return 1;

            } else if (to_col == 2) { // Queenside
                if (state->white_queenside_rook_moved) return 0;
                if (state->board[king_start_row][1].type != EMPTY ||
                    state->board[king_start_row][2].type != EMPTY ||
                    state->board[king_start_row][3].type != EMPTY) return 0;
                if (is_square_attacked(state, king_start_row, 4, opponent_color) ||
                    is_square_attacked(state, king_start_row, 3, opponent_color) ||
                    is_square_attacked(state, king_start_row, 2, opponent_color)) {
                    return 0;
                }
                return 1;
            }
        } else { // BLACK
                if (state->black_king_moved) return 0;
                if (to_col == 6) { // Kingside
                    if (state->black_kingside_rook_moved) return 0;
                    if (state->board[king_start_row][5].type != EMPTY || state->board[king_start_row][6].type != EMPTY) return 0;
                    if (is_square_attacked(state, king_start_row, 4, opponent_color) ||
                        is_square_attacked(state, king_start_row, 5, opponent_color) ||
                        is_square_attacked(state, king_start_row, 6, opponent_color)) {
                        return 0;
                    }
                    return 1;
                } else if (to_col == 2) { // Queenside
                    if (state->black_queenside_rook_moved) return 0;
                    if (state->board[king_start_row][1].type != EMPTY ||
                        state->board[king_start_row][2].type != EMPTY ||
                        state->board[king_start_row][3].type != EMPTY) return 0;

                    if (is_square_attacked(state, king_start_row, 4, opponent_color) ||
                        is_square_attacked(state, king_start_row, 3, opponent_color) ||
                        is_square_attacked(state, king_start_row, 2, opponent_color)) {
                        return 0;
                    }
                    return 1;
                }
        }
    }

    return 0;
}

int is_in_check(const GameState* state, Colour color) {
    // 1. Find the king's position.
    int king_row = -1;
    int king_col = -1;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (state->board[i][j].type == KING && state->board[i][j].color == color) {
                king_row = i;
                king_col = j;
                break;
            }
        }
        if (king_row != -1) break;
    }

    // If the king is not on the board, it cannot be in check.
    if (king_row == -1) return 0;

    // 2. Check if any opponent piece can attack the king's square.
    Colour opponent_color = (color == WHITE) ? BLACK : WHITE;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece piece = state->board[i][j];
            if (piece.type != EMPTY && piece.color == opponent_color) {
                Move threat_move = {i, j, king_row, king_col};

                // Check if this piece can legally attack the king.
                switch (piece.type) {
                    case PAWN: {
                        int direction = (opponent_color == WHITE) ? 1 : -1;
                        if (abs(j - king_col) == 1 && i + direction == king_row) {
                            return 1;
                        }
                        break;
                    }
                    case ROOK: {
                        // A reverse move check works because path blocking is symmetrical.
                        Move reverse_move = {king_row, king_col, i, j};
                        if (is_rook_move_legal(state, &reverse_move)) return 1;
                        break;
                    }
                    case BISHOP: {
                        // A reverse move check works because path blocking is symmetrical.
                        Move reverse_move = {king_row, king_col, i, j};
                        if (is_bishop_move_legal(state, &reverse_move)) return 1;
                        break;
                    }
                    case KNIGHT:
                        if (is_knight_move_legal(state, &threat_move)) return 1;
                        break;
                    case QUEEN: {
                        // A reverse move check works because path blocking is symmetrical.
                        Move reverse_move = {king_row, king_col, i, j};
                        if (is_rook_move_legal(state, &reverse_move) || is_bishop_move_legal(state, &reverse_move)) return 1;
                        break;
                    }
                    case KING:
                        if (abs(i - king_row) <= 1 && abs(j - king_col) <= 1) return 1;
                        break;
                    default:
                        break;
                }
            }
        }
    }

    return 0; // No piece can attack the king.
}

int is_checkmate_or_stalemate(const GameState* state, Colour color) {
    // Iterate through all pieces of the given color to find any legal move.
    for (int from_row = 0; from_row < 8; from_row++){
        for (int from_col = 0; from_col < 8; from_col++) {
            Piece piece = state->board[from_row][from_col];
            if (piece.type != EMPTY && piece.color == color) {
                // Check all possible destination squares for this piece.
                for (int to_row = 0; to_row < 8; to_row++) {
                    for (int to_col = 0; to_col < 8; to_col++) {
                        Move move = {from_row, from_col, to_row, to_col};
                        // If even one legal move is found, it's neither checkmate nor stalemate.
                        if (is_legal_move(state, &move, 0)) { // Use non-verbose mode.
                            return 0;
                        }
                    }
                }
            }
        }
    }
    // If no legal moves were found, determine if it's checkmate or stalemate.
    if (is_in_check(state, color)) {
        return 1; // Checkmate
    } else {
        return 2; // Stalemate
    }
}



int is_square_attacked(const GameState* state, int row, int col, Colour by_color) {
    // Checks if a square is attacked by any piece of the specified color.

    int dir = (by_color == WHITE) ? -1 : 1;

    // Check for pawn attacks.
    if (row + dir >= 0 && row + dir <= 7) {
        if (col - 1 >= 0) {
            Piece piece1 = state->board[row + dir][col - 1];
            if (piece1.type == PAWN && piece1.color == by_color) return 1;
        }
        if (col + 1 <= 7) {
            Piece piece2 = state->board[row + dir][col + 1];
            if (piece2.type == PAWN && piece2.color == by_color) return 1;
        }
    }

    Move temp_move = {0, 0, row, col};

    // Check for attacks from other pieces.
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece piece_on_board = state->board[i][j];
            if (piece_on_board.color == by_color) {
                temp_move.from_row = i;
                temp_move.from_col = j;

                switch (piece_on_board.type) {
                    case KNIGHT:
                        if (is_knight_move_legal(state, &temp_move)) return 1;
                        break;
                    case KING:
                        if (abs(i - row) <= 1 && abs(j - col) <= 1) return 1;
                        break;
                    // For sliding pieces, check if a move from the target square back to the
                    // attacker's square is legal (i.e., the path is clear).
                    case ROOK: {
                        Move reverse_move = {row, col, i, j};
                        if (is_rook_move_legal(state, &reverse_move)) return 1;
                        break;
                    }
                    case BISHOP: {
                        Move reverse_move = {row, col, i, j};
                        if (is_bishop_move_legal(state, &reverse_move)) return 1;
                        break;
                    }
                    case QUEEN:
                        {
                           Move reverse_move = {row, col, i, j};
                           if (is_rook_move_legal(state, &reverse_move) || is_bishop_move_legal(state, &reverse_move)) {
                               return 1;
                           }
                        }
                        break;
                    default:
                        break;
                }
            }
        }
    }
    return 0;
}
