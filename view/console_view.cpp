/*
ConsoleView implementation for console-based UI rendering and input.
*/

#include "console_view.h"
#include <iostream>
#include <iomanip>
#include <limits>

ConsoleView::ConsoleView() {}

void ConsoleView::showWelcome() const {
    clearScreen();
    std::cout << "╔══════════════════════════════════════╗\n";
    std::cout << "║        Interactive Sudoku Game       ║\n";
    std::cout << "║              🎯 Welcome! 🎯          ║\n";
    std::cout << "╚══════════════════════════════════════╝\n\n";
}

void ConsoleView::showHelp() const {
    std::cout << "\n╔═══════════ SUDOKU HELP ══════════════╗\n";
    std::cout << "║ Goal: Fill the 9×9 grid so that each ║\n";
    std::cout << "║ row, column, and 3×3 subgrid contains║\n";
    std::cout << "║ all digits from 1 to 9.              ║\n";
    std::cout << "║                                       ║\n";
    std::cout << "║ Commands:                             ║\n";
    std::cout << "║  m, move  - Make a move               ║\n";
    std::cout << "║  h, help  - Show this help            ║\n";
    std::cout << "║  c, clear - Clear the board           ║\n";
    std::cout << "║  l, load  - Load a puzzle             ║\n";
    std::cout << "║  q, quit  - Exit the game             ║\n";
    std::cout << "║                                       ║\n";
    std::cout << "║ Input format for moves:               ║\n";
    std::cout << "║  Row: 1-9, Col: 1-9, Value: 1-9      ║\n";
    std::cout << "║  (Use 0 to clear a cell)              ║\n";
    std::cout << "╚═══════════════════════════════════════╝\n";
}

void ConsoleView::showGameStatus(const Board& board, int moveCount) const {
    std::cout << "\n📊 Status: ";
    if (board.isComplete()) {
        if (board.isValid()) {
            std::cout << "🎉 SOLVED! 🎉";
        } else {
            std::cout << "❌ Complete but INVALID";
        }
    } else {
        if (board.isValid()) {
            std::cout << "✅ In progress";
        } else {
            std::cout << "❌ INVALID state";
        }
    }
    std::cout << " | 🎯 Moves: " << moveCount << std::endl;
}

void ConsoleView::showBoard(const Board& board) const {
    for (int row = 0; row < board.getBoardSize(); ++row) {
        for (int col = 0; col < board.getBoardSize(); ++col) {
            int value = board.getCell(row, col).getValue();
            std::cout << (value == 0 ? "·" : std::to_string(value)) << " ";
        }
        std::cout << std::endl;
    }
}

void ConsoleView::showBoardWithCoordinates(const Board& board) const {
    std::cout << "\n    1 2 3   4 5 6   7 8 9\n";
    std::cout << "  ┌───────┬───────┬───────┐\n";
    
    for (int row = 0; row < board.getBoardSize(); ++row) {
        std::cout << row + 1 << " │ ";
        
        for (int col = 0; col < board.getBoardSize(); ++col) {
            int value = board.getCell(row, col).getValue();
            if (value == 0) {
                std::cout << "·";
            } else {
                std::cout << value;
            }
            
            if (col == 2 || col == 5) {
                std::cout << " │ ";
            } else if (col == 8) {
                std::cout << " │";
            } else {
                std::cout << " ";
            }
        }
        std::cout << "\n";
        
        if (row == 2 || row == 5) {
            std::cout << "  ├───────┼───────┼───────┤\n";
        }
    }
    std::cout << "  └───────┴───────┴───────┘\n";
}

void ConsoleView::showMessage(const std::string& message) const {
    std::cout << "💬 " << message << std::endl;
}

void ConsoleView::showError(const std::string& error) const {
    std::cout << "❌ Error: " << error << std::endl;
}

void ConsoleView::showSuccess(const std::string& message) const {
    std::cout << "✅ " << message << std::endl;
}

void ConsoleView::showWinMessage(int moveCount) const {
    clearScreen();
    std::cout << "╔══════════════════════════════════════╗\n";
    std::cout << "║            🎉 CONGRATULATIONS! 🎉    ║\n";
    std::cout << "║                                      ║\n";
    std::cout << "║         You solved the puzzle!       ║\n";
    std::cout << "║                                      ║\n";
    std::cout << "║         Total moves: " << std::setw(3) << moveCount << "           ║\n";
    std::cout << "║                                      ║\n";
    std::cout << "║          🌟 Well done! 🌟           ║\n";
    std::cout << "╚══════════════════════════════════════╝\n\n";
}

std::string ConsoleView::getCommand() const {
    std::cout << "\n🎮 Commands: [m]ove, [h]elp, [c]lear, [l]oad, [q]uit\n";
    std::cout << "Enter command: ";
    
    std::string command;
    if (!(std::cin >> command)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return "";
    }
    return command;
}

bool ConsoleView::getMove(int& row, int& col, int& value) const {
    std::cout << "\n🎯 Enter your move:\n";
    
    std::cout << "Row (1-9): ";
    if (!(std::cin >> row)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        showError("Invalid input!");
        return false;
    }
    
    std::cout << "Column (1-9): ";
    if (!(std::cin >> col)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        showError("Invalid input!");
        return false;
    }
    
    std::cout << "Value (1-9, or 0 to clear): ";
    if (!(std::cin >> value)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        showError("Invalid input!");
        return false;
    }
    
    return true;
}

void ConsoleView::clearScreen() const {
    // Clear screen (works on most terminals)
    std::cout << "\033[2J\033[1;1H";
}

void ConsoleView::waitForEnter() const {
    std::cout << "\n⏸️  Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void ConsoleView::printSeparator() const {
    std::cout << "────────────────────────────────────────\n";
}

void ConsoleView::printBoardBorder() const {
    std::cout << "┌───────┬───────┬───────┐\n";
}
