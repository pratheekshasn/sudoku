/*
Implementation of base SudokuSolver helper methods
*/

#include "solver_interface.h"

bool SudokuSolver::isValidMove(const Board& board, int row, int col, int value) const {
    if (value == 0) return true; // Empty cell is always valid
    
    int size = board.getBoardSize();
    if (row < 0 || row >= size || col < 0 || col >= size || value < 1 || value > size) {
        return false;
    }
    
    // Create a temporary board to test the move
    Board tempBoard = board; // Assuming Board has copy constructor
    tempBoard.getCell(row, col).setValue(value);
    
    return tempBoard.isValid();
}

std::vector<int> SudokuSolver::getPossibleValues(const Board& board, int row, int col) const {
    std::vector<int> possibilities;
    int size = board.getBoardSize();
    
    if (board.getCell(row, col).getValue() != 0) {
        return possibilities; // Cell already filled
    }
    
    for (int value = 1; value <= size; ++value) {
        if (isValidMove(board, row, col, value)) {
            possibilities.push_back(value);
        }
    }
    
    return possibilities;
}

bool SudokuSolver::isBoardComplete(const Board& board) const {
    return board.isComplete() && board.isValid();
}
