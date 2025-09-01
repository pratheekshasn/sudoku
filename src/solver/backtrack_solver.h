/*
Backtracking Solver - Basic algorithmic approach
Uses recursive backtracking with constraint checking
*/

#ifndef SUDOKU_BACKTRACK_SOLVER_H
#define SUDOKU_BACKTRACK_SOLVER_H

#include "solver_interface.h"
#include <chrono>

class BacktrackSolver : public SudokuSolver {
public:
    BacktrackSolver();
    
    // Core solving methods
    bool solve(Board& board) override;
    bool canSolve(const Board& board) const override;
    
    // Step-by-step solving
    bool getNextMove(const Board& board, SolverMove& move) override;
    std::vector<SolverMove> getAllPossibleMoves(const Board& board) override;
    
    // Solver information
    std::string getSolverName() const override { return "Backtracking Solver"; }
    SolverDifficulty getDifficulty() const override { return SolverDifficulty::BASIC; }
    std::string getDescription() const override { 
        return "Classic recursive backtracking algorithm with constraint checking"; 
    }

private:
    bool solveRecursive(Board& board);
    bool findEmptyCell(const Board& board, int& row, int& col) const;
    double calculateSmartConfidence(const Board& board, int row, int col, int value, int possibilityCount) const;
    
    // For step-by-step solving
    mutable Board workingBoard;
    mutable std::vector<SolverMove> moveHistory;
    mutable int currentStep;
};

#endif // SUDOKU_BACKTRACK_SOLVER_H
