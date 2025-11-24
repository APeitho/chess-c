#include <stdio.h>
#include <ctype.h>
#include "chess_logic.h"
#include "legal_moves.h"

void setup_empty_state(GameState* state) {
    // Clear board
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            state->board[i][j].type = EMPTY;
            state->board[i][j].color = NONE;
        }
    }
    // Set initial castling rights to not moved
    state->white_king_moved = 0;
    state->white_kingside_rook_moved = 0;
    state->white_queenside_rook_moved = 0;
    state->black_king_moved = 0;
    state->black_kingside_rook_moved = 0;
    state->black_queenside_rook_moved = 0;
}

void test_move(const char* test_name, int from_row, int from_col, int to_row, int to_col, PieceType piece_type, Colour piece_color, Colour turn, const char* expected_result, int setup_piece_row, int setup_piece_col, PieceType setup_piece_type, Colour setup_piece_color) {
    GameState state;
    setup_empty_state(&state);

    //Set up the moving piece
    state.board[from_row][from_col].type = piece_type;
    state.board[from_row][from_col].color = piece_color;
    state.current_turn = turn;

    // Set up an optional blocking/capturing piece
    if (setup_piece_type != EMPTY) {
        state.board[setup_piece_row][setup_piece_col].type = setup_piece_type;
        state.board[setup_piece_row][setup_piece_col].color = setup_piece_color;
    }

    Move move = {from_row, from_col, to_row, to_col};
    const char* result = is_legal_move(&state, &move, 1) ? "LEGAL" : "ILLEGAL";

    printf("Test: %-50s -> %s (%s)\n", test_name, result, expected_result);
}

void setup_stalemate_state(GameState* state) {
    // Clear board
    setup_empty_state(state);
    // Place a King in a stalemated position
    state->board[7][7].type = KING;
    state->board[7][7].color = WHITE;
    state->current_turn = WHITE;

    // Place opposing queen to create stalemate
    state->board[5][6].type = QUEEN;
    state->board[5][6].color = BLACK;
}

void setup_checkmate_state(GameState* state) {
    // Clear board
    setup_empty_state(state);
    // Place a King on the back rank
    state->board[0][4].type = KING;
    state->board[0][4].color = WHITE;
    state->board[1][3].type = PAWN;
    state->board[1][3].color = WHITE;
    state->board[1][4].type = PAWN;
    state->board[1][4].color = WHITE;
    state->board[1][5].type = PAWN;
    state->board[1][5].color = WHITE;
    state->current_turn = WHITE;

    // Place opponent rooks to create backrank checkmate
    state->board[0][0].type = ROOK;
    state->board[0][0].color = BLACK;
    state->board[0][7].type = ROOK;
    state->board[0][7].color = BLACK;
}

void setup_promotion_state(GameState* state) {
    setup_empty_state(state);
    state->board[6][4].type = PAWN;
    state->board[6][4].color = WHITE;
    state->current_turn = WHITE;
}

void setup_castling_state(GameState* state) {
    setup_empty_state(state);

    // Place kings and rooks in starting pos
    state->board[0][4].type = KING; state->board[0][4].color = WHITE;
    state->board[0][0].type = ROOK; state->board[0][0].color = WHITE;
    state->board[0][7].type = ROOK; state->board[0][7].color = WHITE;
    state->board[7][4].type = KING; state->board[7][4].color = BLACK;
    state->board[7][0].type = ROOK; state->board[7][0].color = BLACK;
    state->board[7][7].type = ROOK; state->board[7][7].color = BLACK;

    state->current_turn = WHITE;

}

void setup_en_passant_state(GameState* state) {
    setup_empty_state(state);

    state->board[6][4].type = PAWN;
    state->board[6][4].color = BLACK;
    state->current_turn = BLACK;

    state->board[4][3].type = PAWN;
    state->board[4][3].color = WHITE;

    state->en_passant_target_row = -1;
    state->en_passant_target_col = -1;

    Move black_double_step = {6, 4, 4, 4};
    make_move(state, &black_double_step);
    state->current_turn = WHITE;
}

// Main function
int main() {
    printf("---Chess Logic Tests---\n");

    // --- Pawn Tests ---
    test_move("Pawn single-step (E2 -> E3)", 1, 4, 2, 4, PAWN, WHITE, WHITE, "LEGAL", -1, -1, EMPTY, NONE);
    test_move("Pawn double-step (E2 -> E4)", 1, 4, 3, 4, PAWN, WHITE, WHITE, "LEGAL", -1, -1, EMPTY, NONE);
    test_move("Pawn diagonal capture (E2 -> F3)", 1, 4, 2, 5, PAWN, WHITE, WHITE, "LEGAL", 2, 5, PAWN, BLACK);
    test_move("Pawn illegal sideways (E2 -> F2)", 1, 4, 1, 5, PAWN, WHITE, WHITE, "ILLEGAL", -1, -1, EMPTY, NONE);
    test_move("Pawn double-step blocked (E2 -> E4)", 1, 4, 3, 4, PAWN, WHITE, WHITE, "ILLEGAL", 2, 4, PAWN, BLACK);

    // --- Rook Tests ---
    test_move("Rook legal horizontal (D4 -> H4)", 3, 3, 3, 7, ROOK, WHITE, WHITE, "LEGAL", -1, -1, EMPTY, NONE);
    test_move("Rook blocked horizontal (D4 -> H4)", 3, 3, 3, 7, ROOK, WHITE, WHITE, "ILLEGAL", 3, 5, PAWN, BLACK);
    test_move("Rook legal vertical (D4 -> D8)", 3, 3, 7, 3, ROOK, WHITE, WHITE, "LEGAL", -1, -1, EMPTY, NONE);
    test_move("Rook capture (D4 -> H4)", 3, 3, 3, 7, ROOK, WHITE, WHITE, "LEGAL", 3, 7, PAWN, BLACK);

    // --- Bishop Tests ---
    test_move("Bishop legal diagonal (E5 -> H8)", 4, 4, 7, 7, BISHOP, WHITE, WHITE, "LEGAL", -1, -1, EMPTY, NONE);
    test_move("Bishop blocked diagonal (E5 -> H8)", 4, 4, 7, 7, BISHOP, WHITE, WHITE, "ILLEGAL", 5, 5, PAWN, BLACK);
    test_move("Bishop capture (E5 -> H8)", 4, 4, 7, 7, BISHOP, WHITE, WHITE, "LEGAL", 7, 7, PAWN, BLACK);
    test_move("Bishop illegal straight (E5 -> E8)", 4, 4, 7, 4, BISHOP, WHITE, WHITE, "ILLEGAL", -1, -1, EMPTY, NONE);

    // --- Knight Tests ---
    test_move("Knight legal L-move (D4 -> E6)", 3, 3, 5, 4, KNIGHT, WHITE, WHITE, "LEGAL", -1, -1, EMPTY, NONE);
    test_move("Knight jump over piece (D4 -> C6)", 3, 3, 5, 2, KNIGHT, WHITE, WHITE, "LEGAL", 4, 3, PAWN, BLACK);
    test_move("Knight illegal move (D4 -> E5)", 3, 3, 4, 4, KNIGHT, WHITE, WHITE, "ILLEGAL", -1, -1, EMPTY, NONE);
    test_move("Knight capture (D4 -> E6)", 3, 3, 5, 4, KNIGHT, WHITE, WHITE, "LEGAL", 5, 4, PAWN, BLACK);

    // --- Queen Tests ---
    test_move("Queen legal horizontal (D4 -> H4)", 3, 3, 3, 7, QUEEN, WHITE, WHITE, "LEGAL", -1, -1, EMPTY, NONE);
    test_move("Queen legal diagonal (D4 -> G7)", 3, 3, 6, 6, QUEEN, WHITE, WHITE, "LEGAL", -1, -1, EMPTY, NONE);
    test_move("Queen blocked (D4 -> H4)", 3, 3, 3, 7, QUEEN, WHITE, WHITE, "ILLEGAL", 3, 5, PAWN, BLACK);
    test_move("Queen illegal move (D4 -> F5)", 3, 3, 4, 5, QUEEN, WHITE, WHITE, "ILLEGAL", -1, -1, EMPTY, NONE);
    test_move("Queen capture (D4 -> G7)", 3, 3, 6, 6, QUEEN, WHITE, WHITE, "LEGAL", 6, 6, PAWN, BLACK);

    // --- King Tests (Basic) ---
    test_move("King legal single step (E5->E6)", 4, 4, 5, 4, KING, WHITE, WHITE, "LEGAL", -1, -1, EMPTY, NONE);
    test_move("King legal diagonal step (E5->F6)", 4, 4, 5, 5, KING, WHITE, WHITE, "LEGAL", -1, -1, EMPTY, NONE);
    test_move("King illegal double step (E5->E7)", 4, 4, 6, 4, KING, WHITE, WHITE, "ILLEGAL", -1, -1, EMPTY, NONE);
    test_move("King capture (E5->E6)", 4, 4, 5, 4, KING, WHITE, WHITE, "LEGAL", 5, 4, PAWN, BLACK);

    // ---Check and Checkmate Tests ---
    // More sophisticated tests here later.
    test_move("Legal move that puts the opponent in check (Qd4 -> d7)", 3, 3, 6, 3, QUEEN, WHITE, WHITE, "LEGAL", 7, 3, KING, BLACK);
    test_move("Illegal king move into check (Ke5 -> d5)", 4, 4, 4, 3, KING, WHITE, WHITE, "ILLEGAL", 4, 0, ROOK, BLACK);


    // --Checkmate & Stalemate Tests--
    printf("\n--- Checkmate & Stalemate Tests---\n");
    GameState checkmate_state;

    setup_checkmate_state(&checkmate_state);

    if (is_checkmate_or_stalemate(&checkmate_state, WHITE) == 1) {
        printf("Test: Back rank checkmate: SUCCESS\n");
    } else {
        printf("Test: Back rank checkmate: FAILED\n");
    }

    //Test for Stalemate: Kh8, g7
    GameState stalemate_state;

    setup_stalemate_state(&stalemate_state);

    if (is_checkmate_or_stalemate(&stalemate_state, WHITE) == 2){
        printf("Test: Stalemate: SUCCESS\n");
    } else {
        printf("Test: Stalemate: FAILED\n");
    }

    // --- Pawn Promotion Test ---
    printf("\n--- Pawn Promotion Test ---\n");

    //Setup a board state for a white pawn promotion

    GameState promotion_state;

    setup_promotion_state(&promotion_state);
    Move promotion_move = {6, 4, 7, 4};
    printf("Initial state: Pawn at E7\n");
    printf("Making move E7 -> E8\n");
    make_move(&promotion_state, &promotion_move);
    if (promotion_state.board[7][4].type == QUEEN) {
        printf("Test: Pawn promotion to Queeen: SUCCESS\n");
    } else {
        printf("Test promotion to Queen: FAILED\n");
    }

    printf("\n--- Castling Tests ---\n");
    GameState castling_state;

    // --- White Castling Tests ---
    // Test legal white castling kingside
    setup_castling_state(&castling_state);
    test_move("White Legal Kingside Castling (E1 -> G1)", 0, 4, 0, 6, KING, WHITE, WHITE, "LEGAL", -1, -1, EMPTY, NONE);

    // Test legal white castling queenside
    setup_castling_state(&castling_state);
    test_move("White Legal Queenside Castling (E1 -> C1)", 0, 4, 0, 2, KING, WHITE, WHITE, "LEGAL", -1, -1, EMPTY, NONE);

    // Test illegal castling - king has moved
    setup_castling_state(&castling_state);
    castling_state.white_king_moved = 1;
    test_move("White Illegal Castling - King has moved", 0, 4, 0, 6, KING, WHITE, WHITE, "ILLEGAL", -1, -1, EMPTY, NONE);

    // Test illegal castling - path is blocked
    setup_castling_state(&castling_state);
    castling_state.board[0][5].type = BISHOP;
    castling_state.board[0][5].color = WHITE;
    test_move("White Illegal Castling - Path is blocked", 0, 4, 0, 6, KING, WHITE, WHITE, "ILLEGAL", -1, -1, EMPTY, NONE);

    // Test illegal castling - king is in check
    setup_castling_state(&castling_state);
    castling_state.board[3][4].type = ROOK; // Black rook puts king in check
    castling_state.board[3][4].color = BLACK;
    test_move("White Illegal Castling - King is in check", 0, 4, 0, 6, KING, WHITE, WHITE, "ILLEGAL", -1, -1, EMPTY, NONE);

    // --- Black Castling Tests ---
    // Test legal black castling kingside
    setup_castling_state(&castling_state);
    test_move("Black Legal Kingside Castling (E8 -> G8)", 7, 4, 7, 6, KING, BLACK, BLACK, "LEGAL", -1, -1, EMPTY, NONE);

    // Test legal black castling queenside
    setup_castling_state(&castling_state);
    test_move("Black Legal Queenside Castling (E8 -> C8)", 7, 4, 7, 2, KING, BLACK, BLACK, "LEGAL", -1, -1, EMPTY, NONE);

    // Test illegal black castling - king has moved
    setup_castling_state(&castling_state);
    castling_state.current_turn = BLACK;
    castling_state.black_king_moved = 1;
    test_move("Black Illegal Castling - King has moved", 7, 4, 7, 6, KING, BLACK, BLACK, "ILLEGAL", -1, -1, EMPTY, NONE);

    // Test illegal black castling - path is blocked
    setup_castling_state(&castling_state);
    castling_state.current_turn = BLACK;
    castling_state.board[7][3].type = BISHOP; // White bishop blocks path
    castling_state.board[7][3].color = WHITE;
    test_move("Black Illegal Castling - Path is blocked", 7, 4, 7, 2, KING, BLACK, BLACK, "ILLEGAL", -1, -1, EMPTY, NONE);

    // Test illegal black castling - king travels through check
    setup_castling_state(&castling_state);
    castling_state.current_turn = BLACK;
    castling_state.board[5][3].type = ROOK; // White rook attacks d8
    castling_state.board[5][3].color = WHITE;
    test_move("Black Illegal Castling - Travels through check", 7, 4, 7, 2, KING, BLACK, BLACK, "ILLEGAL", -1, -1, EMPTY, NONE);
}
