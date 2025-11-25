# Chess Game Engine

A complete chess game implementation in C with move validation, special moves, and interactive gameplay.

## Features

- **Full Chess Rules Implementation**
  - All 6 piece types (Pawn, Rook, Knight, Bishop, Queen, King)
  - Move validation for all pieces
  - Check and checkmate detection
  - Stalemate detection
  - Castling (kingside and queenside)
  - Pawn promotion (with choice of piece)
  - En passant capture

- **Interactive Gameplay**
  - Command-line interface
  - Coordinate notation input (e.g., `e2e4`)
  - Castling notation (`O-O`, `O-O-O`)
  - Legal move suggestions
  - Draw offers and agreements
  - Real-time game status display

- **Comprehensive Testing**
  - Test suite covering all piece types
  - Special move tests (castling, promotion, en passant)
  - Check/checkmate/stalemate tests

## Building

### Prerequisites

To build and run this project, you will need a C compiler and the `make` utility.

- On **Linux** (Debian/Ubuntu), you can install the necessary tools with:
  ```bash
  sudo apt-get update && sudo apt-get install build-essential
  ```
- On **macOS**, you can install Command Line Tools for Xcode by running:
  ```bash
  xcode-select --install
  ```

### Compiling the Game

```bash
# Build both test suite and interactive game
make

# Or, build only the test suite
make test

# Or, build only the interactive game
make game

# Clean build artifacts
make clean
```

## Running

### Interactive Game
```bash
./chess
```

**Commands:**
- `e2e4` - Coordinate notation (from square to square)
- `O-O` - Kingside castling
- `O-O-O` - Queenside castling
- `moves e2` - Show legal moves for piece at square
- `draw` - Offer or accept a draw
- `help` - Show help message
- `quit` - Exit game

> **📖 New to chess?** See [HOW_TO_PLAY.md](HOW_TO_PLAY.md) for a complete guide on chess rules and how to use this program!

### Test Suite
```bash
./chess_tests
```

## Project Structure

- `chess_logic.c/h` - Core game logic (board initialization, move execution, board display)
- `legal_moves.c/h` - Move validation and game state checking
- `chess.c` - Interactive game loop with user input
- `chess_tests.c` - Comprehensive test suite
- `Makefile` - Build configuration
- `HOW_TO_PLAY.md` - Complete guide on chess rules and program usage

## Implementation Details

The chess engine uses a 2D array representation of the board with structures for pieces and game state. Move validation includes:
- Piece-specific movement rules
- Path blocking detection for sliding pieces
- Check detection (preventing moves that leave own king in check)
- Castling rights tracking
- En passant target tracking

## License

CS50 Project
