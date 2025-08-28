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
VIEW_SOURCES = view/console_view.cpp
CONTROLLER_SOURCES = controller/game_controller.cpp
SOURCES = $(MODEL_SOURCES) $(VIEW_SOURCES) $(CONTROLLER_SOURCES)

# Object files
MODEL_OBJECTS = $(MODEL_SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
VIEW_OBJECTS = $(VIEW_SOURCES:view/%.cpp=$(OBJDIR)/view_%.o)
CONTROLLER_OBJECTS = $(CONTROLLER_SOURCES:controller/%.cpp=$(OBJDIR)/controller_%.o)
OBJECTS = $(MODEL_OBJECTS) $(VIEW_OBJECTS) $(CONTROLLER_OBJECTS)

# Main targets
MAIN_TARGET = $(BINDIR)/sudoku
TEST_GRID_TARGET = $(BINDIR)/test_grid
TEST_BOARD_TARGET = $(BINDIR)/test_board

# Default target
all: $(MAIN_TARGET)

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

# Main executable
$(MAIN_TARGET): main.cpp $(OBJECTS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) main.cpp $(OBJECTS) -o $@

# Test executables
$(TEST_GRID_TARGET): tests/test_grid_operators.cpp $(OBJECTS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/test_grid_operators.cpp $(OBJECTS) -o $@

$(TEST_BOARD_TARGET): tests/test_board_architecture.cpp $(OBJECTS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/test_board_architecture.cpp $(OBJECTS) -o $@

# Run targets
run: $(MAIN_TARGET)
	./$(MAIN_TARGET)

run-test-grid: $(TEST_GRID_TARGET)
	./$(TEST_GRID_TARGET)

run-test-board: $(TEST_BOARD_TARGET)
	./$(TEST_BOARD_TARGET)

# Clean up
clean:
	rm -rf $(OBJDIR) $(BINDIR)

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
$(OBJDIR)/controller_game_controller.o: controller/game_controller.cpp controller/game_controller.h $(SRCDIR)/board.h view/console_view.h

# Help target
help:
	@echo "Available targets:"
	@echo "  all          - Build all executables (default)"
	@echo "  run          - Build and run main sudoku program"
	@echo "  run-test-grid - Build and run grid operator tests"
	@echo "  run-test-board - Build and run board architecture tests"
	@echo "  clean        - Remove all build files"
	@echo "  debug        - Build with debug symbols"
	@echo "  release      - Build optimized release version"
	@echo "  help         - Show this help message"

# Phony targets
.PHONY: all clean run run-test-grid run-test-board debug release help
