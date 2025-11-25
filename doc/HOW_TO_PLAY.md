# How to Play Chess - User Guide

## Table of Contents
1. [Getting Started](#getting-started)
2. [Basic Chess Rules](#basic-chess-rules)
3. [Using the Chess Program](#using-the-chess-program)
4. [Move Notation](#move-notation)
5. [Special Moves](#special-moves)
6. [Game Status Indicators](#game-status-indicators)
7. [Tips and Strategies](#tips-and-strategies)

---

## Getting Started

### Starting the Game

1. **Prerequisites**: Before you can build the game, make sure you have a C compiler and `make` installed.
   - On **Linux** (like Ubuntu), open a terminal and run:
     ```bash
     sudo apt-get update && sudo apt-get install build-essential
     ```
   - On **macOS**, open a terminal and run:
     ```bash
     xcode-select --install
     ```

2. **Build the program**: Navigate to the project directory in your terminal and run the `make` command. This will create the executable file for the game.
   ```bash
   make
   ```

3. **Run the game**: Once the build is complete, start the interactive chess game by running the following command:
   ```bash
   ./chess
   ```

3. The board will display with White to move first.

### Understanding the Board Display

The board is displayed with:
- **Letters (a-h)** along the bottom and top: represent files (columns)
- **Numbers (1-8)** on the left and right: represent ranks (rows)
- **Uppercase letters**: White pieces (P=Pawn, R=Rook, N=Knight, B=Bishop, Q=Queen, K=King)
- **Lowercase letters**: Black pieces
- **Periods (.)**: Empty squares

Example:
```
  a b c d e f g h
8 r n b q k b n r 8
7 p p p p p p p p 7
6 . . . . . . . . 6
5 . . . . . . . . 5
4 . . . . . . . . 4
3 . . . . . . . . 3
2 P P P P P P P P 2
1 R N B Q K B N R 1
  a b c d e f g h
```

---

## Basic Chess Rules

### Objective
The goal is to **checkmate** your opponent's king. This means the opponent's king is in check (under attack) and cannot escape.

### The Pieces and Their Moves

#### 1. **Pawn (P/p)**
- **Forward movement**: Moves one square forward (toward opponent's side)
- **Initial move**: Can move two squares forward on its first move
- **Capture**: Captures diagonally one square forward
- **Promotion**: When a pawn reaches the opposite end of the board, you can promote it to a Queen, Rook, Bishop, or Knight.

#### 2. **Rook (R/r)**
- Moves horizontally or vertically any number of squares
- Cannot jump over other pieces
- Captures by moving to an occupied square

#### 3. **Knight (N/n)**
- Moves in an "L-shape": two squares in one direction, then one square perpendicular
- **Can jump over other pieces** (unique ability)
- Example moves: two squares up and one right, or two squares left and one down

#### 4. **Bishop (B/b)**
- Moves diagonally any number of squares
- Cannot jump over other pieces
- Stays on the same color square throughout the game

#### 5. **Queen (Q/q)**
- Combines the powers of Rook and Bishop
- Moves horizontally, vertically, or diagonally any number of squares
- Cannot jump over other pieces
- Most powerful piece

#### 6. **King (K/k)**
- Moves one square in any direction (horizontal, vertical, or diagonal)
- Cannot move into check (a square attacked by the opponent)
- Must be protected at all times

### Important Rules

1. **Check**: When your king is under attack by an opponent's piece
   - You must get out of check on your next move
   - You can: move the king, block the attack, or capture the attacking piece

2. **Checkmate**: When your king is in check and cannot escape
   - The game ends immediately
   - The player who achieved checkmate wins

3. **Stalemate**: When it's your turn, you're not in check, but you have no legal moves
   - The game ends in a draw

4. **Draw by Agreement**: Players can agree to end the game in a draw
   - One player offers a draw, and the other player accepts it

4. **You cannot**:
   - Move your own king into check
   - Make a move that leaves your king in check
   - Capture your own pieces

---

## Using the Chess Program

### Basic Commands

#### Making Moves

Enter moves in **coordinate notation** (from square to square):
- Format: `[from][to]`
- Example: `e2e4` (move from e2 to e4)
- Example: `g1f3` (move from g1 to f3)

#### Getting Help

Type `help` or `h` to see available commands:
```
help
```

#### Viewing Legal Moves

To see all legal moves for a piece at a specific square:
```
moves e2
```
This shows all squares the piece at e2 can legally move to.

#### Exiting the Game

Type `quit` or `q` to exit:
```
quit
```

---

## Move Notation

### Coordinate Notation

The program uses **coordinate notation** which is simple and unambiguous:

- **Format**: `[file][rank][file][rank]`
- **Files**: a, b, c, d, e, f, g, h (left to right)
- **Ranks**: 1, 2, 3, 4, 5, 6, 7, 8 (bottom to top)

**Examples:**
- `e2e4` - Move from e2 to e4 (common opening move)
- `g1f3` - Move knight from g1 to f3
- `e1g1` - Move king from e1 to g1 (kingside castling)
- `a7a8` - Move pawn from a7 to a8 (promotion)

### Castling Notation

Castling uses special notation:
- `O-O` or `o-o` or `0-0` - Kingside castling (short castling)
- `O-O-O` or `o-o-o` or `0-0-0` - Queenside castling (long castling)

---

## Special Moves

### 1. Castling

**Kingside Castling (`O-O`):**
- King moves two squares toward the rook on the right
- The rook jumps over the king to the square next to it
- **Requirements:**
  - King and rook must not have moved
  - Squares between must be empty
  - King must not be in check
  - King cannot move through or into check

**Queenside Castling (`O-O-O`):**
- King moves two squares toward the rook on the left
- The rook jumps over the king to the square next to it
- Same requirements as kingside castling

### 2. En Passant

When a pawn moves two squares forward from its starting position, and an enemy pawn could have captured it if it had moved one square, the enemy pawn can capture it "en passant" (in passing).

**How it works:**
1. Your pawn is on the 5th rank (for White) or 4th rank (for Black)
2. Opponent's pawn moves two squares forward, landing next to your pawn
3. On your next move only, you can capture diagonally as if the pawn had moved one square

**Example:**
- White pawn on e5, Black pawn on d7
- Black moves d7-d5
- White can capture with e5xd6 (en passant)

### 3. Pawn Promotion

When a pawn reaches the opposite end of the board:
- The game will prompt you to choose a piece to promote to.
- You can choose a Queen (Q), Rook (R), Bishop (B), or Knight (N).

---

## Game Status Indicators

The program displays game status messages:

### During Game

- **"White to move"** or **"Black to move"**: Shows whose turn it is
- **"*** CHECK ***"**: Your king is in check - you must get out of check!
- **"*** CHECKMATE - [color] wins! ***"**: Game over, winner declared
- **"*** STALEMATE - Draw! ***"**: Game over, it's a draw

### Move Feedback

- **"Move X: [move]"**: Confirms successful move
- **"Illegal move. Try again."**: The move violates chess rules
- **"Invalid move notation..."**: The input format is incorrect

---

## Tips and Strategies

### Opening Principles

1. **Control the center**: Occupy and control central squares (e4, e5, d4, d5)
2. **Develop pieces**: Move knights and bishops toward the center
3. **Castle early**: Protect your king by castling
4. **Don't move the same piece twice**: Develop all pieces efficiently
5. **Don't bring out the queen too early**: Keep it safe

### Common Openings

**For White:**
- `e2e4` - King's Pawn Opening (most popular)
- `d2d4` - Queen's Pawn Opening
- `g1f3` - King's Knight Opening

**For Black (responding to e2e4):**
- `e7e5` - Mirror White's opening
- `c7c5` - Sicilian Defense
- `e7e6` - French Defense

### General Tips

1. **Always check for checks**: Before making a move, ensure you're not leaving your king in check
2. **Think ahead**: Consider what your opponent might do in response
3. **Protect your king**: Keep your king safe, especially after castling
4. **Use all your pieces**: Don't leave pieces undeveloped
5. **Watch for forks**: Knights are excellent at attacking multiple pieces
6. **Control key squares**: Control important squares to limit opponent's options

### Common Mistakes to Avoid

1. **Moving into check**: Never move your king into an attacked square
2. **Leaving pieces undefended**: Don't leave valuable pieces unprotected
3. **Moving too quickly**: Take time to think about each move
4. **Ignoring opponent's threats**: Always consider what your opponent is trying to do
5. **Premature attacks**: Don't attack before developing all pieces

---

## Example Game Session

```
=== Chess Game ===
Enter moves in coordinate notation (e.g., e2e4, Nf3, O-O)
Type 'help' for commands, 'quit' to exit

  a b c d e f g h
8 r n b q k b n r 8
7 p p p p p p p p 7
6 . . . . . . . . 6
5 . . . . . . . . 5
4 . . . . . . . . 4
3 . . . . . . . . 3
2 P P P P P P P P 2
1 R N B Q K B N R 1
  a b c d e f g h

--- White to move ---

Enter move: e2e4
Move 1: e2e4

  a b c d e f g h
8 r n b q k b n r 8
7 p p p p p p p p 7
6 . . . . . . . . 6
5 . . . . . . . . 5
4 . . . . P . . . 4
3 . . . . . . . . 3
2 P P P P . P P P 2
1 R N B Q K B N R 1
  a b c d e f g h

--- Black to move ---

Enter move: e7e5
Move 2: e7e5
...
```

---

## Troubleshooting

### "Illegal move" Errors

If you get an "Illegal move" error, check:
- Is it your turn? (Wrong color piece)
- Is the move legal for that piece type?
- Would the move leave your king in check?
- Is the destination square occupied by your own piece?
- For pawns: Are you trying to move forward into an occupied square?

### "Invalid move notation" Errors

Ensure your move format is correct:
- Use lowercase or uppercase letters for files (a-h)
- Use numbers for ranks (1-8)
- Format: `[file][rank][file][rank]` (e.g., `e2e4`)
- For castling: Use `O-O` or `O-O-O`

### Piece Not Moving

- Check that you're entering coordinates correctly
- Verify the piece exists at the starting square
- Use `moves [square]` to see legal moves for that piece

---

## Additional Resources

- **Chess Notation Guide**: Learn standard algebraic notation (Nf3, Bxc5, etc.)
- **Chess Tactics**: Practice common tactical patterns (forks, pins, skewers)
- **Endgame Principles**: Learn how to convert advantages into wins
- **Chess Puzzles**: Practice with chess puzzles to improve

---

Enjoy playing chess! Remember, chess is a game of skill that improves with practice. Take your time, think carefully, and have fun!
