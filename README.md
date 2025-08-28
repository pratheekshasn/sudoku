# Interactive Sudoku Game 🎯

An interactive command-line Sudoku game with a beautiful interface and real-time validation.

## Features ✨

- 🎮 **Interactive gameplay** with user-friendly commands
- 🎨 **Beautiful ASCII board** with grid lines and coordinates
- ✅ **Real-time validation** - prevents invalid moves
- 📊 **Move counter** and game status
- 🔄 **Load/clear puzzles** functionality
- 🏆 **Win detection** with celebration message

## Build 🔨

Run `make` in the root folder to compile the game.

```bash
make
```

## Run 🚀

Start the interactive game:

```bash
make run
```

## How to Play 🎲

### Commands:
- **`m` or `move`** - Make a move (enter row, column, value)
- **`h` or `help`** - Show help and rules
- **`c` or `clear`** - Clear the entire board
- **`l` or `load`** - Load a sample puzzle
- **`q` or `quit`** - Exit the game

### Input Format:
- **Row**: 1-9 (top to bottom)
- **Column**: 1-9 (left to right)  
- **Value**: 1-9 (or 0 to clear a cell)

### Example Move:
```
Enter your move:
Row (1-9): 1
Column (1-9): 3
Value (1-9, or 0 to clear): 4
```

## Game Rules 📋

Fill the 9×9 grid so that each:
- **Row** contains digits 1-9
- **Column** contains digits 1-9  
- **3×3 subgrid** contains digits 1-9

## Project Structure 🏗️

```
sudoku/
├── main.cpp              # Entry point
├── sudoku_game.h/.cpp    # Interactive game logic
├── model/                # Core Sudoku classes
│   ├── board.h/.cpp      # Game board (9x9 grid)
│   ├── grid.h/.cpp       # Subgrid (3x3 section)
│   └── cell.h/.cpp       # Individual cell
├── Makefile              # Build configuration
└── README.md             # This file
```
