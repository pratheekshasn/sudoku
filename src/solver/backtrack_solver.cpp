/*
Backtracking Solver Implementation
*/

#include "backtrack_solver.h"
#include <algorithm>
#include <random>

BacktrackSolver::BacktrackSolver() : currentStep(0) {
    reset();
}

bool BacktrackSolver::solve(Board& board) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    reset();
    bool result = solveRecursive(board);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    solveTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    return result;
}

bool BacktrackSolver::canSolve(const Board& board) const {
    // Backtracking can solve any valid Sudoku puzzle
    return board.isValid();
}

bool BacktrackSolver::solveRecursive(Board& board) {
    int row, col;
    
    // Find empty cell
    if (!findEmptyCell(board, row, col)) {
        return true; // Board is complete
    }
    
    // Try values 1-9 (or 1-size for different board sizes)
    int size = board.getBoardSize();
    for (int value = 1; value <= size; ++value) {
        if (isValidMove(board, row, col, value)) {
            // Make move
            board.getCell(row, col).setValue(value);
            movesCount++;
            
            // Recursively solve
            if (solveRecursive(board)) {
                return true;
            }
            
            // Backtrack
            board.getCell(row, col).setValue(0);
        }
    }
    
    return false; // No solution found
}

bool BacktrackSolver::findEmptyCell(const Board& board, int& row, int& col) const {
    int size = board.getBoardSize();
    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            if (board.getCell(r, c).getValue() == 0) {
                row = r;
                col = c;
                return true;
            }
        }
    }
    return false;
}

bool BacktrackSolver::getNextMove(const Board& board, SolverMove& move) {
    int row, col;
    
    if (!findEmptyCell(board, row, col)) {
        return false; // No empty cells
    }
    
    // Find the first valid value for this cell
    int size = board.getBoardSize();
    for (int value = 1; value <= size; ++value) {
        if (isValidMove(board, row, col, value)) {
            move = SolverMove(row, col, value, 
                            "Backtracking: First valid value for cell (" + 
                            std::to_string(row + 1) + "," + std::to_string(col + 1) + ")",
                            0.9);
            return true;
        }
    }
    
    return false; // No valid move found
}

std::vector<SolverMove> BacktrackSolver::getAllPossibleMoves(const Board& board) {
    std::vector<SolverMove> moves;
    int size = board.getBoardSize();
    
    // Find all empty cells and their possible values
    for (int row = 0; row < size; ++row) {
        for (int col = 0; col < size; ++col) {
            if (board.getCell(row, col).getValue() == 0) {
                std::vector<int> possibleValues = getPossibleValues(board, row, col);
                
                for (int value : possibleValues) {
                    std::string reasoning = "Possible value " + std::to_string(value) + 
                                          " for cell (" + std::to_string(row + 1) + 
                                          "," + std::to_string(col + 1) + ")";
                    
                    // Improved confidence calculation
                    double confidence = calculateSmartConfidence(board, row, col, value, possibleValues.size());
                    
                    moves.emplace_back(row, col, value, reasoning, confidence);
                }
            }
        }
    }
    
    // Sort by confidence (higher confidence first)
    std::sort(moves.begin(), moves.end(), 
              [](const SolverMove& a, const SolverMove& b) {
                  return a.confidence > b.confidence;
              });
    
    return moves;
}

double BacktrackSolver::calculateSmartConfidence(const Board& board, int row, int col, int value, int possibilityCount) const {
    // Base confidence from constraint level
    double baseConfidence = 1.0 / possibilityCount;
    
    // Bonus for naked singles (only one possibility)
    if (possibilityCount == 1) {
        return 1.0; // Forced move - maximum confidence
    }
    
    // Check if this is a hidden single (only cell in region that can have this value)
    int size = board.getBoardSize();
    int gridSize = static_cast<int>(sqrt(size));
    
    // Check row
    bool hiddenInRow = true;
    for (int c = 0; c < size; ++c) {
        if (c != col && board.getCell(row, c).getValue() == 0) {
            if (isValidMove(board, row, c, value)) {
                hiddenInRow = false;
                break;
            }
        }
    }
    
    // Check column  
    bool hiddenInCol = true;
    for (int r = 0; r < size; ++r) {
        if (r != row && board.getCell(r, col).getValue() == 0) {
            if (isValidMove(board, r, col, value)) {
                hiddenInCol = false;
                break;
            }
        }
    }
    
    // Check 3x3 box
    bool hiddenInBox = true;
    int boxStartRow = (row / gridSize) * gridSize;
    int boxStartCol = (col / gridSize) * gridSize;
    for (int r = boxStartRow; r < boxStartRow + gridSize; ++r) {
        for (int c = boxStartCol; c < boxStartCol + gridSize; ++c) {
            if ((r != row || c != col) && board.getCell(r, c).getValue() == 0) {
                if (isValidMove(board, r, c, value)) {
                    hiddenInBox = false;
                    break;
                }
            }
        }
        if (!hiddenInBox) break;
    }
    
    // Hidden single bonus - much higher confidence
    if (hiddenInRow || hiddenInCol || hiddenInBox) {
        return 0.95; // Very high confidence for logical deduction
    }
    
    // Apply strategy-based multiplier
    if (possibilityCount == 2) {
        return baseConfidence * 0.8; // Naked pair potential
    } else if (possibilityCount == 3) {
        return baseConfidence * 0.6; // Naked triple potential  
    }
    
    return baseConfidence * 0.5; // Lower confidence for many possibilities
}
