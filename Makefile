# Makefile for Sudoku project
# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O2
INCLUDES = -I.

# Directories
SRCDIR = model
OBJDIR = obj
BINDIR = bin

# Source files
MODEL_SOURCES = $(SRCDIR)/cell.cpp $(SRCDIR)/grid.cpp $(SRCDIR)/board.cpp
VIEW_SOURCES = view/console_view.cpp view/web_view.cpp
CONTROLLER_SOURCES = controller/game_controller.cpp
API_SOURCES = api/json_api.cpp
SOURCES = $(MODEL_SOURCES) $(VIEW_SOURCES) $(CONTROLLER_SOURCES) $(API_SOURCES)

# Object files
MODEL_OBJECTS = $(MODEL_SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
VIEW_OBJECTS = $(VIEW_SOURCES:view/%.cpp=$(OBJDIR)/view_%.o)
CONTROLLER_OBJECTS = $(CONTROLLER_SOURCES:controller/%.cpp=$(OBJDIR)/controller_%.o)
API_OBJECTS = $(API_SOURCES:api/%.cpp=$(OBJDIR)/api_%.o)
OBJECTS = $(MODEL_OBJECTS) $(VIEW_OBJECTS) $(CONTROLLER_OBJECTS)

# Main targets
MAIN_TARGET = $(BINDIR)/sudoku
TEST_GRID_TARGET = $(BINDIR)/test_grid
TEST_BOARD_TARGET = $(BINDIR)/test_board
API_TARGET = $(BINDIR)/sudoku_api

# Default target
all: $(MAIN_TARGET) $(API_TARGET)

# Create directories if they don't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile view files
$(OBJDIR)/view_%.o: view/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile controller files
$(OBJDIR)/controller_%.o: controller/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile API files
$(OBJDIR)/api_%.o: api/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Main executable
$(MAIN_TARGET): main.cpp $(OBJECTS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) main.cpp $(OBJECTS) -o $@

# API executable (doesn't need view layer)
$(API_TARGET): api/api_main.cpp $(MODEL_OBJECTS) $(API_OBJECTS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) api/api_main.cpp $(MODEL_OBJECTS) $(API_OBJECTS) -o $@

# Test executables
$(TEST_GRID_TARGET): tests/test_grid_operators.cpp $(OBJECTS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/test_grid_operators.cpp $(OBJECTS) -o $@

$(TEST_BOARD_TARGET): tests/test_board_architecture.cpp $(OBJECTS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/test_board_architecture.cpp $(OBJECTS) -o $@

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
	cd api && ../venv/bin/python bridge_server.py

# Alternative: run server without auto-creating venv
run-server-simple: $(API_TARGET)
	@echo "🚀 Starting API Bridge Server (using system Python)..."
	@echo "⚠️  Make sure Flask is installed: pip install flask flask-cors"
	cd api && python3 bridge_server.py

run-test-grid: $(TEST_GRID_TARGET)
	./$(TEST_GRID_TARGET)

run-test-board: $(TEST_BOARD_TARGET)
	./$(TEST_BOARD_TARGET)

# Clean up
clean:
	rm -rf $(OBJDIR) $(BINDIR)

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
$(OBJDIR)/cell.o: $(SRCDIR)/cell.cpp $(SRCDIR)/cell.h
$(OBJDIR)/grid.o: $(SRCDIR)/grid.cpp $(SRCDIR)/grid.h $(SRCDIR)/cell.h
$(OBJDIR)/board.o: $(SRCDIR)/board.cpp $(SRCDIR)/board.h $(SRCDIR)/grid.h $(SRCDIR)/cell.h
$(OBJDIR)/view_console_view.o: view/console_view.cpp view/console_view.h $(SRCDIR)/board.h
$(OBJDIR)/view_web_view.o: view/web_view.cpp view/web_view.h view/sudoku_view.h $(SRCDIR)/board.h
$(OBJDIR)/controller_game_controller.o: controller/game_controller.cpp controller/game_controller.h $(SRCDIR)/board.h view/console_view.h view/web_view.h view/sudoku_view.h

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
	@echo "  clean        - Remove build files only"
	@echo "  clean-all    - Remove build files AND Python venv"
	@echo "  debug        - Build with debug symbols"
	@echo "  release      - Build optimized release version"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "🌐 For Web UI (recommended):"
	@echo "  1. make run-server      # Creates venv, installs Flask, starts server"
	@echo "  2. Open web/index.html in browser"
	@echo "  3. Enjoy your beautiful Sudoku game! 🎯"

# Phony targets
.PHONY: all clean clean-all run run-api run-server run-server-simple venv run-test-grid run-test-board debug release help
