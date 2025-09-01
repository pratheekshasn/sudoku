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

## Things to install

```bash
sudo apt update && sudo apt install -y nlohmann-json3-dev
```

## Extending code

🚀 Next Steps:
You can now easily add even more solvers:

Neural Network Solver (true AI using machine learning)
Heuristic Solver (advanced techniques like X-Wing, Swordfish)
Competition Solver (speed-optimized for contests)
Each solver you create automatically becomes available through:

get_ai_move <solver_name>
get_ai_moves <solver_name>
solve_puzzle <solver_name>


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
├── model/                # 📊 Data layer (MVC Model)
│   ├── board.h/.cpp      #   └── Game board (9x9 grid)
│   ├── grid.h/.cpp       #   └── Subgrid (3x3 section)
│   └── cell.h/.cpp       #   └── Individual cell
├── view/                 # 🎨 Presentation layer (MVC View)
│   └── console_view.h/.cpp #   └── Console UI rendering
├── controller/           # 🎮 Logic layer (MVC Controller)
│   └── game_controller.h/.cpp # └── Game flow coordination
├── tests/                # 🧪 Test files
│   ├── test_grid_operators.cpp
│   └── test_board_architecture.cpp
├── Makefile              # Build configuration
└── README.md             # This file
```

### Architecture Benefits 🎯

- **🔄 MVC Pattern**: Clean separation of concerns
- **🎨 View Layer**: Easy to add new UI (web, mobile, desktop)
- **🎮 Controller**: Game logic separated from presentation
- **📊 Model**: Pure data structures, UI-independent
- **🧪 Testable**: Each layer can be tested independently
