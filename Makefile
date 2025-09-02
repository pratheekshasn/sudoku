# Makefile for Sudoku project with modern structure and dynamic generation
# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O2
INCLUDES = -I.

# Directories
SRCDIR = src
MODELDIR = $(SRCDIR)/model
VIEWDIR = $(SRCDIR)/view
CONTROLLERDIR = $(SRCDIR)/controller
APIDIR = $(SRCDIR)/api
SOLVERDIR = $(SRCDIR)/solver
TESTDIR = tests
BUILDDIR = build
OBJDIR = $(BUILDDIR)/obj
BINDIR = $(BUILDDIR)/bin

# Source files
MODEL_SOURCES = $(MODELDIR)/cell.cpp $(MODELDIR)/grid.cpp $(MODELDIR)/board.cpp $(MODELDIR)/sudoku_generator.cpp
VIEW_SOURCES = $(VIEWDIR)/console_view.cpp $(VIEWDIR)/web_view.cpp
CONTROLLER_SOURCES = $(CONTROLLERDIR)/game_controller.cpp
SOLVER_SOURCES = $(SOLVERDIR)/solver_interface.cpp $(SOLVERDIR)/backtrack_solver.cpp $(SOLVERDIR)/constraint_solver.cpp $(SOLVERDIR)/neuro_symbolic_solver.cpp $(SOLVERDIR)/solver_factory.cpp
API_SOURCES = $(APIDIR)/json_api.cpp
SOURCES = $(MODEL_SOURCES) $(VIEW_SOURCES) $(CONTROLLER_SOURCES) $(SOLVER_SOURCES) $(API_SOURCES)

# Object files
MODEL_OBJECTS = $(MODEL_SOURCES:$(MODELDIR)/%.cpp=$(OBJDIR)/model_%.o)
VIEW_OBJECTS = $(VIEW_SOURCES:$(VIEWDIR)/%.cpp=$(OBJDIR)/view_%.o)
CONTROLLER_OBJECTS = $(CONTROLLER_SOURCES:$(CONTROLLERDIR)/%.cpp=$(OBJDIR)/controller_%.o)
SOLVER_OBJECTS = $(SOLVER_SOURCES:$(SOLVERDIR)/%.cpp=$(OBJDIR)/solver_%.o)
API_OBJECTS = $(API_SOURCES:$(APIDIR)/%.cpp=$(OBJDIR)/api_%.o)
OBJECTS = $(MODEL_OBJECTS) $(VIEW_OBJECTS) $(CONTROLLER_OBJECTS) $(SOLVER_OBJECTS)

# Main targets
MAIN_TARGET = $(BINDIR)/sudoku
TEST_GRID_TARGET = $(BINDIR)/test_grid
TEST_BOARD_TARGET = $(BINDIR)/test_board
TEST_WEBVIEW_TARGET = $(BINDIR)/test_webview
API_TARGET = $(BINDIR)/sudoku_api

# Default target
all: $(MAIN_TARGET) $(API_TARGET)

# Create directories if they don't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Compile source files to object files
$(OBJDIR)/model_%.o: $(MODELDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile view files
$(OBJDIR)/view_%.o: $(VIEWDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile controller files
$(OBJDIR)/controller_%.o: $(CONTROLLERDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile API files
$(OBJDIR)/api_%.o: $(APIDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile solver files
$(OBJDIR)/solver_%.o: $(SOLVERDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Main executable
$(MAIN_TARGET): $(SRCDIR)/main.cpp $(OBJECTS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCDIR)/main.cpp $(OBJECTS) -o $@

# API executable (doesn't need view layer)
$(API_TARGET): $(APIDIR)/api_main.cpp $(MODEL_OBJECTS) $(SOLVER_OBJECTS) $(API_OBJECTS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(APIDIR)/api_main.cpp $(MODEL_OBJECTS) $(SOLVER_OBJECTS) $(API_OBJECTS) -o $@

# Test executables
$(TEST_GRID_TARGET): $(TESTDIR)/test_grid_operators.cpp $(OBJECTS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(TESTDIR)/test_grid_operators.cpp $(OBJECTS) -o $@

$(TEST_BOARD_TARGET): $(TESTDIR)/test_board_architecture.cpp $(OBJECTS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(TESTDIR)/test_board_architecture.cpp $(OBJECTS) -o $@

$(TEST_WEBVIEW_TARGET): $(TESTDIR)/test_webview.cpp $(OBJECTS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(TESTDIR)/test_webview.cpp $(OBJECTS) -o $@

# Run targets
run: $(MAIN_TARGET)
	./$(MAIN_TARGET)

run-api: $(API_TARGET)
	./$(API_TARGET) get_board

# Python virtual environment setup
venv:
	@echo "🐍 Creating Python virtual environment..."
	@if [ ! -d "venv" ]; then \
		python3 -m venv venv; \
	else \
		echo "📁 Virtual environment already exists"; \
	fi
	@echo "📦 Installing/updating Flask and dependencies..."
	./venv/bin/pip install --upgrade pip
	./venv/bin/pip install flask flask-cors
	@echo "✅ Virtual environment ready! Use 'make run-server' to start."

run-server: venv $(API_TARGET)
	@echo "🚀 Starting API Bridge Server..."
	@echo "🌐 Server will run on http://localhost:5000"
	@echo "📂 Open web/index.html in your browser"
	cd $(APIDIR) && ../../venv/bin/python bridge_server.py

# Alternative: run server without auto-creating venv
run-server-simple: $(API_TARGET)
	@echo "🚀 Starting API Bridge Server (using system Python)..."
	@echo "⚠️  Make sure Flask is installed: pip install flask flask-cors"
	cd $(APIDIR) && python3 bridge_server.py

# Test targets
run-test-grid: $(TEST_GRID_TARGET)
	./$(TEST_GRID_TARGET)

run-test-board: $(TEST_BOARD_TARGET)
	./$(TEST_BOARD_TARGET)

run-test-webview: $(TEST_WEBVIEW_TARGET)
	./$(TEST_WEBVIEW_TARGET)

# Clean up
clean:
	rm -rf $(BUILDDIR)

clean-all: clean
	@echo "🧹 Cleaning everything including Python venv..."
	rm -rf venv

# Debug target (with debug symbols and no optimization)
debug: CXXFLAGS = -std=c++17 -Wall -Wextra -g -DDEBUG
debug: $(MAIN_TARGET)

# Release target (optimized)
release: CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -DNDEBUG
release: $(MAIN_TARGET)

# Dependencies (automatically generated)
$(OBJDIR)/model_cell.o: $(MODELDIR)/cell.cpp $(MODELDIR)/cell.h
$(OBJDIR)/model_grid.o: $(MODELDIR)/grid.cpp $(MODELDIR)/grid.h $(MODELDIR)/cell.h
$(OBJDIR)/model_board.o: $(MODELDIR)/board.cpp $(MODELDIR)/board.h $(MODELDIR)/grid.h $(MODELDIR)/cell.h
$(OBJDIR)/model_sudoku_generator.o: $(MODELDIR)/sudoku_generator.cpp $(MODELDIR)/sudoku_generator.h $(MODELDIR)/board.h
$(OBJDIR)/view_console_view.o: $(VIEWDIR)/console_view.cpp $(VIEWDIR)/console_view.h $(MODELDIR)/board.h
$(OBJDIR)/view_web_view.o: $(VIEWDIR)/web_view.cpp $(VIEWDIR)/web_view.h $(VIEWDIR)/sudoku_view.h $(MODELDIR)/board.h
$(OBJDIR)/controller_game_controller.o: $(CONTROLLERDIR)/game_controller.cpp $(CONTROLLERDIR)/game_controller.h $(MODELDIR)/board.h $(MODELDIR)/sudoku_generator.h $(VIEWDIR)/console_view.h $(VIEWDIR)/web_view.h $(VIEWDIR)/sudoku_view.h

# Help target
help:
	@echo "Available targets:"
	@echo "  all          - Build all executables (default)"
	@echo "  run          - Build and run console sudoku game"
	@echo "  run-api      - Build and test API executable"
	@echo "  venv         - Create Python virtual environment with Flask"
	@echo "  run-server   - Start web API bridge server (auto-creates venv)"
	@echo "  run-test-grid - Build and run grid operator tests"
	@echo "  run-test-board - Build and run board architecture tests"
	@echo "  run-test-webview - Build and run webview interface tests"
	@echo "  clean        - Remove build files only"
	@echo "  clean-all    - Remove build files AND Python venv"
	@echo "  debug        - Build with debug symbols"
	@echo "  release      - Build optimized release version"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "🎯 Features:"
	@echo "  ✨ Dynamic Sudoku generation with backtracking algorithm"
	@echo "  🎮 Strategy Pattern with pluggable ConsoleView/WebView"
	@echo "  🌐 Full-stack web UI with Flask bridge server"
	@echo "  🧪 Comprehensive test suite"
	@echo ""
	@echo "🌐 For Web UI (recommended):"
	@echo "  1. make run-server      # Creates venv, installs Flask, starts server"
	@echo "  2. Open web/index.html in browser"
	@echo "  3. Enjoy your beautiful Sudoku game! 🎯"
	@echo ""
	@echo "📁 Project Structure:"
	@echo "  src/model/       - Data structures (Board, Cell, Grid, Generator)"
	@echo "  src/view/        - UI interfaces (Console, Web)"
	@echo "  src/controller/  - Game logic (MVC Controller)"
	@echo "  src/api/         - JSON API for web frontend"
	@echo "  tests/           - All test files"
	@echo "  build/           - Build artifacts (obj/, bin/)"
	@echo "  web/             - Web UI files"

# Phony targets
.PHONY: all clean clean-all run run-api run-server run-server-simple venv run-test-grid run-test-board run-test-webview debug release help
