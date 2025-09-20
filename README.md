# Interactive Sudoku Game 🎯

An interactive command-line Sudoku game with a beautiful interface and real-time validation.

## Features ✨

- 🎮 **Interactive gameplay** with user-friendly commands
- 🎨 **Beautiful ASCII board** with grid lines and coordinates
- ✅ **Real-time validation** - prevents invalid moves
- 📊 **Move counter** and game status
- 🔄 **Load/clear puzzles** functionality
- 🏆 **Win detection** with celebration message

## Prerequisites 📋

Before building, make sure you have the required dependencies installed:

### Linux/macOS:
```bash
sudo apt update && sudo apt install -y nlohmann-json3-dev  # Ubuntu/Debian
# or
brew install nlohmann-json  # macOS with Homebrew
```

### Windows:
Make sure you have a C++ compiler (like MinGW-w64 or Visual Studio) and Python 3 installed.

## Build 🔨

The project uses a comprehensive Makefile with multiple targets. To build all executables:

```bash
make
```

This builds both the console game and the API server.

### Build Options:
- `make all` - Build all executables (default)
- `make debug` - Build with debug symbols
- `make release` - Build optimized release version
- `make clean` - Remove build files
- `make clean-all` - Remove build files and Python virtual environment

## Run 🚀

### Option 1: Web Interface (Recommended) 🌐

For the best experience, use the web interface with the built-in server:

```bash
make run-server
```

This will:
1. Automatically create a Python virtual environment
2. Install Flask and dependencies
3. Build the C++ API backend
4. Start the Flask bridge server on `http://localhost:5000`

Then open `web/index.html` in your browser to play!

### Option 2: Console Interface 🖥️

For a classic terminal experience:

```bash
make run
```

### Option 3: API Only 🔧

To test just the C++ API backend:

```bash
make run-api
```

## Server Details 🖥️

The project includes a sophisticated web server setup:

- **C++ Backend**: Fast native Sudoku solver and game logic
- **Python Bridge**: Flask server that communicates with the C++ API
- **Web Frontend**: Beautiful HTML/CSS/JavaScript interface
- **Auto-setup**: The Makefile handles virtual environment creation

### Manual Server Setup:
If you prefer manual setup or have issues with the automatic setup:

```bash
# Create virtual environment manually
python3 -m venv venv
source venv/bin/activate  # Linux/macOS
# or
venv\Scripts\activate     # Windows

# Install dependencies
pip install flask flask-cors

# Start server
make run-server-simple
```

## Testing 🧪

Run the comprehensive test suite:

```bash
make run-test-grid      # Test grid operations
make run-test-board     # Test board architecture  
make run-test-webview   # Test web interface
make run-test-crossval  # Test cross-validation system
```

## Available Make Targets 🎯

| Target | Description |
|--------|-------------|
| `make` or `make all` | Build all executables |
| `make run` | Build and run console game |
| `make run-server` | Start web server (auto-setup) |
| `make run-api` | Test C++ API backend |
| `make venv` | Create Python virtual environment |
| `make debug` | Build with debug symbols |
| `make release` | Build optimized version |
| `make clean` | Remove build files |
| `make clean-all` | Remove build files and venv |
| `make run-test-crossval` | Run cross-validation tests |
| `make help` | Show detailed help |

## Extending the Code 🚀

The project uses a modular architecture that makes it easy to add new solvers:

### Available Solvers:
- **Backtrack Solver**: Classic recursive algorithm
- **Constraint Solver**: Advanced constraint propagation
- **Neuro-Symbolic Solver**: Hybrid AI approach

### Adding New Solvers:
You can easily add more solvers such as:
- Neural Network Solver (machine learning)
- Heuristic Solver (X-Wing, Swordfish techniques)
- Competition Solver (speed-optimized)

Each solver automatically becomes available through the API:
- `get_ai_move <solver_name>`
- `get_ai_moves <solver_name>`
- `solve_puzzle <solver_name>`

## Cross-Validation System 🧪

The neuro-symbolic solver now includes a comprehensive cross-validation system for systematic performance evaluation:

### 🧠 Key Methodology:
- **Training Phase**: Neural network learns from board patterns + symbolic reasoning hints
- **Testing Phase**: Neural network makes predictions using ONLY learned patterns (no symbolic hints)
- **True Neural Evaluation**: Tests the network's ability to recognize patterns independently

### Features:
- **K-Fold Cross-Validation**: Configurable number of folds (default: 5)
- **Performance Metrics**: Precision, Recall, F1-Score, Mean Absolute Error
- **Adaptive Testing**: Works with different board sizes (4x4, 9x9, 16x16)
- **Detailed Reporting**: Statistical analysis with confidence intervals
- **Pure Neural Testing**: Evaluates learned pattern recognition without symbolic assistance

### API Commands:
```bash
# Run cross-validation with 50 puzzles, 5 folds
./build/bin/sudoku_api cross_validate 50,5,false

# Get performance metrics on 20 test puzzles
./build/bin/sudoku_api performance_metrics 20

# Verbose cross-validation
./build/bin/sudoku_api cross_validate 30,3,true
```

### Cross-Validation Output:
- **Overall Accuracy**: Average prediction accuracy across all folds
- **Fold-by-Fold Results**: Individual accuracy for each fold
- **Statistical Analysis**: Standard deviation and confidence intervals  
- **Performance Breakdown**: Excellent (≥95%), Good (≥50%), Poor (<50%) results
- **Detailed Metrics**: True/False positives/negatives, precision, recall


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
