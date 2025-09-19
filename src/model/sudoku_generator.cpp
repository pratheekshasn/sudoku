/*
SudokuGenerator implementation
Generates valid Sudoku puzzles using backtracking algorithm
*/

#include "sudoku_generator.h"
#include <algorithm>
#include <chrono>

SudokuGenerator::SudokuGenerator() 
    : rng(std::chrono::steady_clock::now().time_since_epoch().count()) {
}

bool SudokuGenerator::generateCompleteGrid(Board& board) {
    // Clear the board first
    for (int i = 0; i < board.getBoardSize(); i++) {
        for (int j = 0; j < board.getBoardSize(); j++) {
            board.getCell(i, j).setValue(0);
        }
    }
    
    return fillGrid(board);
}

bool SudokuGenerator::generatePuzzle(Board& board, int difficulty) {
    // First generate a complete grid
    if (!generateCompleteGrid(board)) {
        return false;
    }
    
    // Create a list of all cell positions
    std::vector<std::pair<int, int>> positions;
    for (int i = 0; i < board.getBoardSize(); i++) {
        for (int j = 0; j < board.getBoardSize(); j++) {
            positions.push_back({i, j});
        }
    }
    
    // Shuffle the positions randomly
    std::shuffle(positions.begin(), positions.end(), rng);
    
    // Remove cells while maintaining unique solution
    int cellsRemoved = 0;
    for (const auto& pos : positions) {
        if (cellsRemoved >= difficulty) {
            break;
        }
        
        int row = pos.first;
        int col = pos.second;
        int originalValue = board.getCell(row, col).getValue();
        
        // Temporarily remove the cell
        board.getCell(row, col).setValue(0);
        
        // Check if puzzle still has unique solution
        if (hasUniqueSolution(board)) {
            cellsRemoved++;
        } else {
            // Restore the value if it makes solution non-unique
            board.getCell(row, col).setValue(originalValue);
        }
    }
    
    return cellsRemoved >= difficulty / 2; // Accept if we removed at least half the target
}

bool SudokuGenerator::createPuzzleFromCompleteGrid(Board& board, int difficulty) {
    // This method assumes the board is already a complete, valid solution
    // We just remove cells to create a puzzle, without regenerating the grid
    
    // Create a list of all cell positions
    std::vector<std::pair<int, int>> positions;
    for (int i = 0; i < board.getBoardSize(); i++) {
        for (int j = 0; j < board.getBoardSize(); j++) {
            positions.push_back({i, j});
        }
    }
    
    // Shuffle the positions randomly
    std::shuffle(positions.begin(), positions.end(), rng);
    
    // Remove cells while maintaining unique solution
    int cellsRemoved = 0;
    for (const auto& pos : positions) {
        if (cellsRemoved >= difficulty) {
            break;
        }
        
        int row = pos.first;
        int col = pos.second;
        int originalValue = board.getCell(row, col).getValue();
        
        // Temporarily remove the cell
        board.getCell(row, col).setValue(0);
        
        // Check if puzzle still has unique solution
        if (hasUniqueSolution(board)) {
            cellsRemoved++;
        } else {
            // Restore the value if it makes solution non-unique
            board.getCell(row, col).setValue(originalValue);
        }
    }
    
    return cellsRemoved >= difficulty / 2; // Accept if we removed at least half the target
}

bool SudokuGenerator::fillGrid(Board& board) {
    // Find empty cell
    int row = -1, col = -1;
    bool found = false;
    
    for (int i = 0; i < board.getBoardSize() && !found; i++) {
        for (int j = 0; j < board.getBoardSize() && !found; j++) {
            if (board.getCell(i, j).getValue() == 0) {
                row = i;
                col = j;
                found = true;
            }
        }
    }
    
    // If no empty cell found, grid is complete
    if (!found) {
        return true;
    }
    
    // Create shuffled list of numbers 1-9
    std::vector<int> numbers;
    for (int i = 1; i <= board.getBoardSize(); i++) {
        numbers.push_back(i);
    }
    shuffleArray(numbers);
    
    // Try each number
    for (int num : numbers) {
        if (isValidPlacement(board, row, col, num)) {
            board.getCell(row, col).setValue(num);
            
            if (fillGrid(board)) {
                return true;
            }
            
            // Backtrack
            board.getCell(row, col).setValue(0);
        }
    }
    
    return false;
}

bool SudokuGenerator::isValidPlacement(const Board& board, int row, int col, int value) {
    // Check row
    for (int j = 0; j < board.getBoardSize(); j++) {
        if (j != col && board.getCell(row, j).getValue() == value) {
            return false;
        }
    }
    
    // Check column  
    for (int i = 0; i < board.getBoardSize(); i++) {
        if (i != row && board.getCell(i, col).getValue() == value) {
            return false;
        }
    }
    
    // Check 3x3 box (assuming 9x9 grid)
    if (board.getBoardSize() == 9) {
        int boxRow = (row / 3) * 3;
        int boxCol = (col / 3) * 3;
        
        for (int i = boxRow; i < boxRow + 3; i++) {
            for (int j = boxCol; j < boxCol + 3; j++) {
                if ((i != row || j != col) && board.getCell(i, j).getValue() == value) {
                    return false;
                }
            }
        }
    }
    
    return true;
}

void SudokuGenerator::shuffleArray(std::vector<int>& arr) {
    std::shuffle(arr.begin(), arr.end(), rng);
}

bool SudokuGenerator::hasUniqueSolution(Board& board) {
    return countSolutions(board, 2) == 1;
}

int SudokuGenerator::countSolutions(Board board, int maxSolutions) {
    // Find empty cell
    int row = -1, col = -1;
    bool found = false;
    
    for (int i = 0; i < board.getBoardSize() && !found; i++) {
        for (int j = 0; j < board.getBoardSize() && !found; j++) {
            if (board.getCell(i, j).getValue() == 0) {
                row = i;
                col = j;
                found = true;
            }
        }
    }
    
    // If no empty cell found, we have a solution
    if (!found) {
        return 1;
    }
    
    int solutions = 0;
    
    // Try each number 1-9
    for (int num = 1; num <= board.getBoardSize(); num++) {
        if (isValidPlacement(board, row, col, num)) {
            board.getCell(row, col).setValue(num);
            
            solutions += countSolutions(board, maxSolutions);
            
            if (solutions >= maxSolutions) {
                return solutions; // Early exit if too many solutions
            }
            
            // Backtrack
            board.getCell(row, col).setValue(0);
        }
    }
    
    return solutions;
}

bool SudokuGenerator::solvePuzzle(Board& board) {
    return fillGrid(board);
}
