/*
SudokuGame class implementation for interactive gameplay.
*/

#include "sudoku_game.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <sstream>

SudokuGame::SudokuGame(int gridSize) : board(gridSize), moveCount(0) {}

void SudokuGame::play() {
    clearScreen();
    std::cout << "=== Welcome to Interactive Sudoku! ===\n\n";
    
    // Initialize with a sample puzzle
    initializeSamplePuzzle();
    
    std::string command;
    while (true) {
        printBoardWithCoordinates();
        showGameStatus();
        
        std::cout << "\nCommands: [m]ove, [h]elp, [c]lear, [l]oad puzzle, [q]uit\n";
        std::cout << "Enter command: ";
        
        if (!(std::cin >> command)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        
        if (command == "q" || command == "quit") {
            std::cout << "Thanks for playing! Final move count: " << moveCount << std::endl;
            break;
        }
        else if (command == "m" || command == "move") {
            if (makeMove()) {
                moveCount++;
                if (isGameComplete()) {
                    clearScreen();
                    printBoardWithCoordinates();
                    std::cout << "\n🎉 Congratulations! You solved the puzzle! 🎉\n";
                    std::cout << "Total moves: " << moveCount << std::endl;
                    break;
                }
            }
        }
        else if (command == "h" || command == "help") {
            showHelp();
        }
        else if (command == "c" || command == "clear") {
            clearBoard();
            moveCount = 0;
        }
        else if (command == "l" || command == "load") {
            loadPuzzle();
            moveCount = 0;
        }
        else {
            std::cout << "Invalid command. Type 'h' for help.\n";
        }
        
        std::cout << "\nPress Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
        clearScreen();
    }
}

void SudokuGame::showHelp() const {
    std::cout << "\n=== SUDOKU HELP ===\n";
    std::cout << "Goal: Fill the 9x9 grid so that each row, column, and 3x3 subgrid\n";
    std::cout << "      contains all digits from 1 to 9.\n\n";
    std::cout << "Commands:\n";
    std::cout << "  m, move  - Make a move (enter row, column, value)\n";
    std::cout << "  h, help  - Show this help\n";
    std::cout << "  c, clear - Clear the entire board\n";
    std::cout << "  l, load  - Load a new puzzle\n";
    std::cout << "  q, quit  - Exit the game\n\n";
    std::cout << "Input format for moves:\n";
    std::cout << "  Row: 1-9 (top to bottom)\n";
    std::cout << "  Col: 1-9 (left to right)\n";
    std::cout << "  Value: 1-9 (or 0 to clear a cell)\n";
}

void SudokuGame::printBoard() const {
    board.print();
}

void SudokuGame::printBoardWithCoordinates() const {
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

bool SudokuGame::makeMove() {
    int row, col, value;
    
    if (!getUserMove(row, col, value)) {
        return false;
    }
    
    // Convert to 0-based indexing
    row--; col--;
    
    if (!isValidInput(row, col, value)) {
        std::cout << "Invalid input! Row and column must be 1-9, value must be 0-9.\n";
        return false;
    }
    
    // Store old value for validation
    int oldValue = board.getCell(row, col).getValue();
    
    // Make the move
    board.getCell(row, col).setValue(value);
    
    // Check if the move is valid
    if (!board.isValid()) {
        std::cout << "Invalid move! This violates Sudoku rules.\n";
        board.getCell(row, col).setValue(oldValue); // Undo the move
        return false;
    }
    
    if (value == 0) {
        std::cout << "Cell cleared!\n";
    } else {
        std::cout << "Move successful!\n";
    }
    
    return true;
}

void SudokuGame::loadPuzzle() {
    clearBoard();
    initializeSamplePuzzle();
    std::cout << "Sample puzzle loaded!\n";
}

void SudokuGame::clearBoard() {
    for (int row = 0; row < board.getBoardSize(); ++row) {
        for (int col = 0; col < board.getBoardSize(); ++col) {
            board.getCell(row, col).setValue(0);
        }
    }
    std::cout << "Board cleared!\n";
}

bool SudokuGame::isGameComplete() const {
    return board.isComplete() && board.isValid();
}

void SudokuGame::showGameStatus() const {
    std::cout << "\nStatus: ";
    if (board.isComplete()) {
        if (board.isValid()) {
            std::cout << "SOLVED! 🎉";
        } else {
            std::cout << "Complete but INVALID ❌";
        }
    } else {
        if (board.isValid()) {
            std::cout << "In progress ✓";
        } else {
            std::cout << "INVALID state ❌";
        }
    }
    std::cout << " | Moves: " << moveCount << std::endl;
}

bool SudokuGame::isValidInput(int row, int col, int value) const {
    return row >= 0 && row < board.getBoardSize() && 
           col >= 0 && col < board.getBoardSize() && 
           value >= 0 && value <= board.getBoardSize();
}

void SudokuGame::clearScreen() const {
    // Clear screen (works on most terminals)
    std::cout << "\033[2J\033[1;1H";
}

bool SudokuGame::getUserMove(int& row, int& col, int& value) {
    std::cout << "\nEnter your move:\n";
    std::cout << "Row (1-9): ";
    if (!(std::cin >> row)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input!\n";
        return false;
    }
    
    std::cout << "Column (1-9): ";
    if (!(std::cin >> col)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input!\n";
        return false;
    }
    
    std::cout << "Value (1-9, or 0 to clear): ";
    if (!(std::cin >> value)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input!\n";
        return false;
    }
    
    return true;
}

void SudokuGame::initializeSamplePuzzle() {
    // Easy Sudoku puzzle
    int puzzle[9][9] = {
        {5, 3, 0, 0, 7, 0, 0, 0, 0},
        {6, 0, 0, 1, 9, 5, 0, 0, 0},
        {0, 9, 8, 0, 0, 0, 0, 6, 0},
        {8, 0, 0, 0, 6, 0, 0, 0, 3},
        {4, 0, 0, 8, 0, 3, 0, 0, 1},
        {7, 0, 0, 0, 2, 0, 0, 0, 6},
        {0, 6, 0, 0, 0, 0, 2, 8, 0},
        {0, 0, 0, 4, 1, 9, 0, 0, 5},
        {0, 0, 0, 0, 8, 0, 0, 7, 9}
    };
    
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            board.getCell(row, col).setValue(puzzle[row][col]);
        }
    }
}
