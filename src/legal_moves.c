#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "legal_moves.h"

int is_legal_move(const GameState* state, const Move* move, int verbose) {

    // 1. Check of the move is within the board boundaries
    if (move->from_row < 0 || move->from_row > 7 || move->from_col < 0 || move->from_col > 7 || move->to_row < 0 || move->to_col > 7 || move->to_col < 0 || move->to_col > 7) {
        if (verbose) printf("Error: Move is outside the board.\n");
        return 0;
    }

    Piece piece_to_move = state->board[move->from_row][move->from_col];

    // 2. Check if there is a piece on the "from" square.\n
    if (piece_to_move.type == EMPTY) {
        if (verbose) printf("Error: No piece on the starting square.\n");
        return 0;
    }

    // 3. Check if it's the correct turn for the piece
    if (piece_to_move.color != state->current_turn) {
        if (verbose) printf("Error: It's not your turn to move that piece.\n");
        return 0;
    }

    // Get the piece at the destination square
    Piece destination_piece = state->board[move->to_row][move->to_col];

    // 4. Check for "friendly fire" -- Cannot capture your won piece
    if (destination_piece.color == piece_to_move.color) {
        if (verbose) printf("Error: Cannot capture your own piece.\n");
        return 0;
    }

    int piece_move_is_legal;
    // 5. Dispatch to piece-specific validation
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
            return 0; // shouldn't happen with valid input
    }

    if (!piece_move_is_legal) {
        return 0;
    }

    // Crucial Check: Will this move leave the king in check?
    // We must simulate the move on a temporary board.
    GameState temp_state = *state;

    // Move the piece on the temporary board
    temp_state.board[move->to_row][move->to_col] = temp_state.board[move->from_row][move->from_col];
    temp_state.board[move->from_row][move->from_col].type = EMPTY;
    temp_state.board[move->from_row][move->from_col].color = NONE;

    // Now, check if the current player's king is in check on this temp board.
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

    // Check for a single-step forward move
    if (from_col == to_col && to_row == from_row + direction) {
        // A forward move is only legal if the destination square is empty.
        if (state->board[to_row][to_col].type == EMPTY) {
            return 1;
        }
    }

    // Check for a double-step initial move.
    if (from_col == to_col && to_row == from_row + 2 * direction) {
        // This move is only legal if the pawn is on its starting rank.
        int starting_row = (piece.color == WHITE) ? 1 : 6;
        if (from_row == starting_row) {
            // Both the destination square and the square in between must be empty.
            if (state->board[to_row][to_col].type == EMPTY && state->board[from_row + direction][from_col].type == EMPTY) {
               return 1;
            }
        }
    }

    // Check for diagonal capture (regular or en passant)
    if (abs(from_col - to_col) == 1 && to_row == from_row + direction) {
        // Check for regular diagonal capture
        if (state->board[to_row][to_col].type != EMPTY && state->board[to_row][to_col].color != piece.color) {
            return 1;
        }

        // Check for en passant capture
        if (state->en_passant_target_row == to_row && state->en_passant_target_col == to_col) {
            // Check if there's an enemy pawn on the adjacent file
            int enemy_pawn_row = (piece.color == WHITE) ? to_row - 1 : to_row + 1;
            if (enemy_pawn_row >= 0 && enemy_pawn_row <= 7) {
                Piece enemy_piece = state->board[enemy_pawn_row][to_col];
                if (enemy_piece.type == PAWN && enemy_piece.color != piece.color) {
                    return 1;
                }
            }
        }
    }

    // If none of the above conditions are met, the move is illegal.
    return 0;
}


int is_knight_move_legal(const GameState* state, const Move* move) {
    int from_row = move->from_row;
    int from_col = move->from_col;
    int to_row = move->to_row;
    int to_col = move->to_col;

    int row_diff = abs(from_row - to_row);
    int col_diff = abs(from_col - to_col);

    // A knight moves two sqaures in one direction and one in the perpendicular direction
    if ((row_diff == 2 && col_diff == 1) || (row_diff == 1 && col_diff == 2)) {
        return 1; // Legal move
    }

    return 0;
}


int is_bishop_move_legal(const GameState* state, const Move* move) {
    int from_row = move->from_row;
    int from_col = move->from_col;
    int to_row = move->to_row;
    int to_col = move->to_col;

    // Check for a diagonal move
    if (abs(from_row - to_row) != abs(from_col - to_col)) {
        return 0; // Not a diagonal move
    }

    // Determine the direction of the diagonal
    int row_dir = (to_row > from_row) ? 1 : -1;
    int col_dir = (to_col > from_col) ? 1 : -1;

    //Check for clear path
    int row = from_row + row_dir;
    int col = from_col + col_dir;
    while (row != to_row) {
        if (state->board[row][col].type != EMPTY) {
            return 0; // Path is blocked
        }
        row += row_dir;
        col += col_dir;
    }

    return 1; // Move is legal
}


int is_rook_move_legal(const GameState* state, const Move* move) {
    int from_row = move->from_row;
    int from_col = move->from_col;
    int to_row = move->to_row;
    int to_col = move->to_col;

    // Check for a straight horizontal or vertical move
    if (from_row != to_row && from_col != to_col) {
        return 0; // Not a straight move
    }

    // Check for a clear path
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
                return 0; // Path is blocked
            }
        }
    }

    return 1; // Move is legal
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

    // Basic king move: one square in any direction
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

                // Path must be clear and squares not under attack
                if (state->board[king_start_row][5].type != EMPTY || state->board[king_start_row][6].type != EMPTY) return 0; // Path blocked
                if (is_square_attacked(state, king_start_row, 4, opponent_color) ||
                    is_square_attacked(state, king_start_row, 5, opponent_color) ||
                    is_square_attacked(state, king_start_row, 6, opponent_color)) {
                    return 0;
                }
                return 1;

            } else if (to_col == 2) { // Queenside
                if (state->white_queenside_rook_moved) return 0;
                // Path must be clear and squares not under attack
                if (state->board[king_start_row][1].type != EMPTY ||
                    state->board[king_start_row][2].type != EMPTY ||
                    state->board[king_start_row][3].type != EMPTY) return 0; // Path blocked
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
                    if (state->board[king_start_row][5].type != EMPTY || state->board[king_start_row][6].type != EMPTY) return 0; // Path blocked
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
                        state->board[king_start_row][3].type != EMPTY) return 0; // Path blocked

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
    // 1. Find the King's position
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

    // If for some reason the king isn't on the board, we can't be in check
    if (king_row == -1) return 0;

    // 2. Iterate through all sqaures and check for threats
    Colour opponent_color = (color == WHITE) ? BLACK : WHITE;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece piece = state->board[i][j];
            if (piece.type != EMPTY && piece.color == opponent_color) {
                Move threat_move = {i, j, king_row, king_col};

                // Now, check if this piece can legally attack the king
                switch (piece.type) {
                    // For sliding pieces (Rook, Bishop, Queen), we can't just use their
                    // move legal functions because those check for blocked paths.
                    // An attack is valid even if pieces are in the way.
                    // We can reuse the logic from is_square_attacked by reversing the move.
                    case PAWN: {
                        int direction = (opponent_color == WHITE) ? 1 : -1;
                        if (abs(j - king_col) == 1 && i + direction == king_row) {
                            return 1;
                        }
                        break;
                    }
                    case ROOK: {
                        Move reverse_move = {king_row, king_col, i, j};
                        if (is_rook_move_legal(state, &reverse_move)) return 1;
                        break;
                    }
                    case BISHOP: {
                        Move reverse_move = {king_row, king_col, i, j};
                        if (is_bishop_move_legal(state, &reverse_move)) return 1;
                        break;
                    }
                    case KNIGHT:
                        if (is_knight_move_legal(state, &threat_move)) return 1;
                        break;
                    case QUEEN: {
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

    return 0; // No piece cam attack the king
}

int is_checkmate_or_stalemate(const GameState* state, Colour color) {
    // First, check if there are any legal moves at all for the current player
    for (int from_row = 0; from_row < 8; from_row++){
        for (int from_col = 0; from_col < 8; from_col++) {
            Piece piece = state->board[from_row][from_col];
            if (piece.type != EMPTY && piece.color == color) {
                // Now check all possible destinations for this piece
                for (int to_row = 0; to_row < 8; to_row++) {
                    for (int to_col = 0; to_col < 8; to_col++) {
                        Move move = {from_row, from_col, to_row, to_col};
                        // Use is_legal_move to check if this is a valid move
                        // is_legal_move already handles the "does it leave the king in check" rule
                        if (is_legal_move(state, &move, 0)) { // Pass 0 for non-verbose mode
                            return 0;
                        }
                    }
                }
            }
        }
    }
    // If we've reached here, there are no legal moves.
    // Now we need to determine if it's checkmate or stalemate.
    if (is_in_check(state, color)) {
        return 1; // Checkmate
    } else {
        return 2; // Stalemate
    }
}



int is_square_attacked(const GameState* state, int row, int col, Colour by_color) {
    // This checks if a specific square is attacked by a piece of 'by_color'
    // We can do this without creating a temporary board.

    // Check if a pawn of 'by_color' can attack this square

    int dir = (by_color == WHITE) ? -1 : 1;

    // Check the two diagonal squares where a pawn would be
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

    // Check for other pieces
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
                        // A king can attack adjacent squares.
                        if (abs(i - row) <= 1 && abs(j - col) <= 1) return 1;
                        break;
                    // For sliding pieces (Rook, Bishop, Queen), we can't just use their
                    // move legal functions because those check for blocked paths.
                    // An attack is valid even if pieces are in the way.
                    // We need to simulate a move from the target square back to the attacker.
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
                        // A queen's attack is a combination of a rook's and a bishop's.
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
