#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "chess_logic.h"
#include "legal_moves.h"

// Helper: remove spaces from input into output buffer (null-terminated)
static void copy_without_spaces(const char* src, char* dst, size_t dst_size) {
    size_t w = 0;
    for (size_t r = 0; src[r] != '\0' && w + 1 < dst_size; r++) {
        if (src[r] != ' ' && src[r] != '\t') {
            dst[w++] = src[r];
        }
    }
    dst[w] = '\0';
}

// Function to parse coordinate notation (e.g., "e2e4", "a1h8")
int parse_move_notation(const char* notation, Move* move) {
    if (strlen(notation) < 4) {
        return 0;
    }

    // Parse from square (e.g., "e2")
    move->from_col = tolower(notation[0]) - 'a';
    move->from_row = notation[1] - '1';

    // Parse to square (e.g., "e4")
    move->to_col = tolower(notation[2]) - 'a';
    move->to_row = notation[3] - '1';

    // Validate coordinates
    if (move->from_row < 0 || move->from_row > 7 || move->from_col < 0 || move->from_col > 7 ||
        move->to_row < 0 || move->to_row > 7 || move->to_col < 0 || move->to_col > 7) {
        return 0;
    }

    return 1;
}

// Resolve SAN-like algebraic notation (e.g., "Bc4", "Bxc4", "Nf3", "exd5", "R1e1")
// Supports: piece letter [KQRBN] optional (default pawn), optional disambiguation (file or rank),
// optional 'x' capture marker, destination square (file+rank), optional '+' or '#'.
static int parse_algebraic(const GameState* state, const char* raw_notation, Move* out_move) {
    char s[64];
    copy_without_spaces(raw_notation, s, sizeof(s));
    size_t n = strlen(s);
    if (n < 2) return 0;

    // Strip trailing check/mate symbols
    while (n > 0 && (s[n-1] == '+' || s[n-1] == '#')) {
        s[--n] = '\0';
    }

    // Find destination square as last [file][rank]
    if (n < 2) return 0;
    char dest_file = tolower(s[n-2]);
    char dest_rank = s[n-1];
    if (dest_file < 'a' || dest_file > 'h' || dest_rank < '1' || dest_rank > '8') {
        return 0;
    }
    int to_col = dest_file - 'a';
    int to_row = dest_rank - '1';

    // Determine piece type
    int idx = 0;
    PieceType piece_type = PAWN;
    if (s[idx] >= 'A' && s[idx] <= 'Z') {
        switch (s[idx]) {
            case 'K': piece_type = KING; break;
            case 'Q': piece_type = QUEEN; break;
            case 'R': piece_type = ROOK; break;
            case 'B': piece_type = BISHOP; break;
            case 'N': piece_type = KNIGHT; break;
            default: return 0; // Unsupported piece letter
        }
        idx++;
    }

    // Remaining pattern before destination: optional disambiguation and optional 'x'
    int disambig_file = -1; // 0..7 if provided
    int disambig_rank = -1; // 0..7 if provided
    for (; idx < (int)n - 2; idx++) {
        char c = s[idx];
        if (c == 'x' || c == 'X') continue; // ignore capture marker
        if (c >= 'a' && c <= 'h') {
            disambig_file = c - 'a';
        } else if (c >= '1' && c <= '8') {
            disambig_rank = c - '1';
        } else {
            // Unknown token, bail
            return 0;
        }
    }

    // Search for matching piece that can legally move to destination
    int found = 0;
    Move candidate = {0,0,to_row,to_col};
    Colour side = state->current_turn;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            Piece p = state->board[r][c];
            if (p.color != side) continue;
            if (p.type != piece_type) continue;
            if (disambig_file != -1 && c != disambig_file) continue;
            if (disambig_rank != -1 && r != disambig_rank) continue;
            Move m = {r, c, to_row, to_col};
            if (is_legal_move(state, &m, 0)) {
                candidate = m;
                found++;
            }
        }
    }
    if (found == 1) {
        *out_move = candidate;
        return 1;
    }
    return 0; // none or ambiguous
}

// Function to parse castling notation (O-O or O-O-O)
int parse_castling(const char* notation, Move* move, Colour color) {
    if (strcmp(notation, "O-O") == 0 || strcmp(notation, "o-o") == 0 || strcmp(notation, "0-0") == 0) {
        // Kingside castling
        move->from_row = (color == WHITE) ? 0 : 7;
        move->from_col = 4;
        move->to_row = move->from_row;
        move->to_col = 6;
        return 1;
    } else if (strcmp(notation, "O-O-O") == 0 || strcmp(notation, "o-o-o") == 0 || strcmp(notation, "0-0-0") == 0) {
        // Queenside castling
        move->from_row = (color == WHITE) ? 0 : 7;
        move->from_col = 4;
        move->to_row = move->from_row;
        move->to_col = 2;
        return 1;
    }
    return 0;
}

// Function to get all legal moves for a piece at a given square
void print_legal_moves(const GameState* state, int row, int col) {
    Piece piece = state->board[row][col];
    if (piece.type == EMPTY || piece.color != state->current_turn) {
        return;
    }

    printf("Legal moves for ");
    switch (piece.type) {
        case PAWN: printf("pawn"); break;
        case ROOK: printf("rook"); break;
        case KNIGHT: printf("knight"); break;
        case BISHOP: printf("bishop"); break;
        case QUEEN: printf("queen"); break;
        case KING: printf("king"); break;
        default: break;
    }
    printf(" at %c%d: ", 'a' + col, row + 1);

    int count = 0;
    for (int to_row = 0; to_row < 8; to_row++) {
        for (int to_col = 0; to_col < 8; to_col++) {
            Move move = {row, col, to_row, to_col};
            if (is_legal_move(state, &move, 0)) {
                if (count > 0) printf(", ");
                printf("%c%d", 'a' + to_col, to_row + 1);
                count++;
            }
        }
    }
    if (count == 0) {
        printf("none");
    }
    printf("\n");
}

// Function to display game status
void display_status(const GameState* state) {
    Colour current = state->current_turn;
    printf("\n--- %s to move ---\n", (current == WHITE) ? "White" : "Black");

    if (is_in_check(state, current)) {
        int result = is_checkmate_or_stalemate(state, current);
        if (result == 1) {
            printf("*** CHECKMATE - %s wins! ***\n", (current == WHITE) ? "Black" : "White");
        } else {
            printf("*** CHECK ***\n");
        }
    } else if (is_checkmate_or_stalemate(state, current) == 2) {
         printf("*** STALEMATE - Draw! ***\n");
    }
    fflush(stdout);
}

// Main game loop
int main() {
    GameState state;
    initialize_board(&state);

    printf("=== Chess Game ===\n");
    printf("Enter moves in coordinate notation (e.g., e2e4, Nf3, O-O)\n");
    printf("Type 'help' for commands, 'quit' to exit\n\n");

    char input[256];
    int move_count = 0;

    while (1) {
        print_board(&state);
        display_status(&state);

        // Check for game over
        if (state.status != IN_PROGRESS) {
            break;
        }
        int mate_or_stale = is_checkmate_or_stalemate(&state, state.current_turn);
        if (mate_or_stale == 1 || mate_or_stale == 2) {
            break;
        }

        printf("\nEnter move: ");
        fflush(stdout);
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        // Remove newline
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        // Handle empty input
        if (strlen(input) == 0) {
            continue;
        }

        // Handle commands
        if (strcmp(input, "quit") == 0 || strcmp(input, "q") == 0) {
            printf("Game ended.\n");
            fflush(stdout);
            break;
        }

        if (strcmp(input, "help") == 0 || strcmp(input, "h") == 0) {
            printf("\nCommands:\n");
            printf("  e2e4, a1h8  - Coordinate notation (from square to square)\n");
            printf("  O-O          - Kingside castling\n");
            printf("  O-O-O        - Queenside castling\n");
            printf("  help         - Show this help\n");
            printf("  draw         - Offer or accept a draw\n");
            printf("  quit         - Exit game\n");
            printf("  moves <sq>   - Show legal moves for piece at square (e.g., moves e2)\n");
            printf("\n");
            fflush(stdout);
            continue;
        }

        if (strcmp(input, "draw") == 0) {
            if (state.draw_offer_by == NONE) {
                state.draw_offer_by = state.current_turn;
                printf("%s offers a draw. The next player may type 'draw' to accept.\n",
                       state.current_turn == WHITE ? "White" : "Black");
                // The offer stands, but it's still the current player's turn to move.
            } else if (state.draw_offer_by != state.current_turn) {
                printf("Draw by agreement.\n");
                state.status = DRAW_AGREEMENT;
                break; // End the game
            } else {
                printf("You have already offered a draw. Please make a move.\n");
            }
            fflush(stdout);
            continue;
        }

        // Handle "moves" command
        if (strncmp(input, "moves ", 6) == 0) {
            if (strlen(input) >= 8) {
                char square[3];
                square[0] = tolower(input[6]);
                square[1] = input[7];
                square[2] = '\0';

                int col = square[0] - 'a';
                int row = square[1] - '1';

                if (row >= 0 && row <= 7 && col >= 0 && col <= 7) {
                    print_legal_moves(&state, row, col);
                } else {
                    printf("Invalid square. Use format like 'e2'\n");
                }
            } else {
                printf("Usage: moves <square> (e.g., moves e2)\n");
            }
            fflush(stdout);
            continue;
        }

        Move move;
        int valid_notation = 0;

        // Try parsing as castling notation first
        if (parse_castling(input, &move, state.current_turn)) {
            valid_notation = 1;
        }
        // Try parsing as coordinate notation
        else if (parse_move_notation(input, &move)) {
            valid_notation = 1;
        }
        // Try parsing as SAN-like algebraic notation (e.g., Bc4, Nf3, exd5)
        else if (parse_algebraic(&state, input, &move)) {
            valid_notation = 1;
        }

        if (!valid_notation) {
            printf("Invalid move notation. Use coordinate notation (e2e4) or castling (O-O).\n");
            printf("Type 'help' for more information.\n");
            fflush(stdout);
            continue;
        }

        // Validate and execute move
        if (is_legal_move(&state, &move, 1)) {
            make_move(&state, &move);
            // A move is made, so any pending draw offer is automatically declined.
            state.draw_offer_by = NONE;

            move_count++;
            printf("Move %d: %s\n", move_count, input);
            fflush(stdout);
        } else {
            printf("Illegal move. Try again.\n");
            fflush(stdout);
        }
    }

    if (state.status == DRAW_AGREEMENT) {
        printf("\nGame ended in a draw by agreement.\n");
    }


    printf("\nGame ended after %d moves.\n", move_count);
    return 0;
}
