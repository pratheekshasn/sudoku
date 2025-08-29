# Simple Makefile for new structure
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O2

all:
	@echo "Building with new structure..."
	mkdir -p build/obj build/bin
	$(CXX) $(CXXFLAGS) -c src/model/cell.cpp -o build/obj/cell.o
	$(CXX) $(CXXFLAGS) -c src/model/grid.cpp -o build/obj/grid.o
	$(CXX) $(CXXFLAGS) -c src/model/board.cpp -o build/obj/board.o
	$(CXX) $(CXXFLAGS) -c src/view/console_view.cpp -o build/obj/console_view.o
	$(CXX) $(CXXFLAGS) -c src/view/web_view.cpp -o build/obj/web_view.o
	$(CXX) $(CXXFLAGS) -c src/controller/game_controller.cpp -o build/obj/game_controller.o
	$(CXX) $(CXXFLAGS) src/main.cpp build/obj/*.o -o build/bin/sudoku
	@echo "✅ Build complete!"

test-webview:
	mkdir -p build/bin
	$(CXX) $(CXXFLAGS) tests/test_webview.cpp build/obj/*.o -o build/bin/test_webview
	./build/bin/test_webview

run:
	./build/bin/sudoku

clean:
	rm -rf build/

help:
	@echo "Available targets:"
	@echo "  all          - Build main executable"
	@echo "  test-webview - Build and run webview test"
	@echo "  run          - Run the main program"
	@echo "  clean        - Clean build files"

.PHONY: all test-webview run clean help
