/*
Constraint Propagation Solver - More intelligent approach
Uses logical deduction techniques that humans use
*/

#ifndef SUDOKU_CONSTRAINT_SOLVER_H
#define SUDOKU_CONSTRAINT_SOLVER_H

#include "solver_interface.h"
#include <set>
#include <map>
#include <functional>

class ConstraintSolver : public SudokuSolver {
public:
    ConstraintSolver();
    
    // Core solving methods
    bool solve(Board& board) override;
    bool canSolve(const Board& board) const override;
    
    // Step-by-step solving with intelligent reasoning
    bool getNextMove(const Board& board, SolverMove& move) override;
    std::vector<SolverMove> getAllPossibleMoves(const Board& board) override;
    
    // Solver information
    std::string getSolverName() const override { return "Constraint Propagation Solver"; }
    SolverDifficulty getDifficulty() const override { return SolverDifficulty::ADVANCED; }
    std::string getDescription() const override { 
        return "Intelligent solver using human-like logical deduction techniques"; 
    }

private:
    // Constraint propagation techniques
    bool nakedSingles(Board& board, std::vector<SolverMove>& moves);
    bool hiddenSingles(Board& board, std::vector<SolverMove>& moves);
    bool nakedPairs(Board& board, std::vector<SolverMove>& moves);
    bool pointingPairs(Board& board, std::vector<SolverMove>& moves);
    
    // Helper methods
    std::set<int> getCandidates(const Board& board, int row, int col) const;
    bool eliminateCandidate(std::map<std::pair<int,int>, std::set<int>>& candidates, 
                           int row, int col, int value);
    
    // Strategy execution order (from easiest to hardest)
    std::vector<std::function<bool(Board&, std::vector<SolverMove>&)>> strategies;
    
    // Confidence calculation based on strategy used
    double calculateConfidence(const std::string& strategy, int candidateCount) const;
};

#endif // SUDOKU_CONSTRAINT_SOLVER_H
