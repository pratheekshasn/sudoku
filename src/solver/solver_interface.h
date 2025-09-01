/*
Abstract Solver Interface for Sudoku AI Solvers
Implements Strategy Pattern for different solving algorithms
*/

#ifndef SUDOKU_SOLVER_INTERFACE_H
#define SUDOKU_SOLVER_INTERFACE_H

#include "../model/board.h"
#include <string>
#include <vector>

// Solver difficulty levels
enum class SolverDifficulty {
    BASIC,      // Simple backtracking
    ADVANCED,   // Constraint propagation + backtracking
    EXPERT,     // Advanced heuristics
    AI_NEURAL   // Machine learning based (future)
};

// Move result for step-by-step solving
struct SolverMove {
    int row, col, value;
    std::string reasoning;
    double confidence;
    
    SolverMove(int r, int c, int v, const std::string& reason = "", double conf = 1.0)
        : row(r), col(c), value(v), reasoning(reason), confidence(conf) {}
};

// Abstract base class for all Sudoku solvers
class SudokuSolver {
public:
    virtual ~SudokuSolver() = default;
    
    // Core solving methods
    virtual bool solve(Board& board) = 0;
    virtual bool canSolve(const Board& board) const = 0;
    
    // Step-by-step solving for visualization
    virtual bool getNextMove(const Board& board, SolverMove& move) = 0;
    virtual std::vector<SolverMove> getAllPossibleMoves(const Board& board) = 0;
    
    // Solver information
    virtual std::string getSolverName() const = 0;
    virtual SolverDifficulty getDifficulty() const = 0;
    virtual std::string getDescription() const = 0;
    
    // Performance metrics
    virtual int getMovesCount() const { return movesCount; }
    virtual double getSolveTimeMs() const { return solveTimeMs; }
    
    // Reset solver state
    virtual void reset() { movesCount = 0; solveTimeMs = 0.0; }

protected:
    int movesCount = 0;
    double solveTimeMs = 0.0;
    
    // Helper methods for derived classes
    bool isValidMove(const Board& board, int row, int col, int value) const;
    std::vector<int> getPossibleValues(const Board& board, int row, int col) const;
    bool isBoardComplete(const Board& board) const;
};

#endif // SUDOKU_SOLVER_INTERFACE_H
