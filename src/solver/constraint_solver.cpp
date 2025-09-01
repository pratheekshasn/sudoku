/*
Constraint Propagation Solver Implementation
Uses human-like logical deduction techniques
*/

#include "constraint_solver.h"
#include <algorithm>
#include <functional>
#include <cmath>

ConstraintSolver::ConstraintSolver() {
    // Initialize strategies in order of difficulty (easiest first)
    strategies = {
        [this](Board& board, std::vector<SolverMove>& moves) { return nakedSingles(board, moves); },
        [this](Board& board, std::vector<SolverMove>& moves) { return hiddenSingles(board, moves); },
        [this](Board& board, std::vector<SolverMove>& moves) { return nakedPairs(board, moves); },
        [this](Board& board, std::vector<SolverMove>& moves) { return pointingPairs(board, moves); }
    };
}

bool ConstraintSolver::solve(Board& board) {
    bool progress = true;
    while (progress && !isBoardComplete(board)) {
        progress = false;
        std::vector<SolverMove> moves;
        
        // Try each strategy until one makes progress
        for (auto& strategy : strategies) {
            if (strategy(board, moves)) {
                // Apply the first move found
                if (!moves.empty()) {
                    board.getCell(moves[0].row, moves[0].col).setValue(moves[0].value);
                    progress = true;
                    break;
                }
            }
        }
    }
    
    return isBoardComplete(board);
}

bool ConstraintSolver::canSolve(const Board& board) const {
    // This solver can attempt any valid Sudoku puzzle
    return board.isValid();
}

bool ConstraintSolver::getNextMove(const Board& board, SolverMove& move) {
    std::vector<SolverMove> moves = getAllPossibleMoves(board);
    if (!moves.empty()) {
        move = moves[0]; // Return highest confidence move
        return true;
    }
    return false;
}

std::vector<SolverMove> ConstraintSolver::getAllPossibleMoves(const Board& board) {
    std::vector<SolverMove> allMoves;
    
    // Try each strategy and collect moves
    for (size_t i = 0; i < strategies.size(); ++i) {
        std::vector<SolverMove> strategyMoves;
        Board tempBoard = board; // Work on a copy
        
        if (strategies[i](tempBoard, strategyMoves)) {
            // Add strategy name to moves
            for (auto& move : strategyMoves) {
                switch (i) {
                    case 0: move.reasoning = "Naked Single: " + move.reasoning; break;
                    case 1: move.reasoning = "Hidden Single: " + move.reasoning; break;
                    case 2: move.reasoning = "Naked Pair: " + move.reasoning; break;
                    case 3: move.reasoning = "Pointing Pair: " + move.reasoning; break;
                }
                allMoves.push_back(move);
            }
        }
    }
    
    // Sort by confidence (highest first)
    std::sort(allMoves.begin(), allMoves.end(), 
              [](const SolverMove& a, const SolverMove& b) {
                  return a.confidence > b.confidence;
              });
    
    return allMoves;
}

bool ConstraintSolver::nakedSingles(Board& board, std::vector<SolverMove>& moves) {
    int size = board.getBoardSize();
    bool found = false;
    
    for (int row = 0; row < size; ++row) {
        for (int col = 0; col < size; ++col) {
            if (board.getCell(row, col).getValue() == 0) {
                std::set<int> candidates = getCandidates(board, row, col);
                
                if (candidates.size() == 1) {
                    int value = *candidates.begin();
                    std::string reasoning = "Only possible value for cell (" + 
                                          std::to_string(row + 1) + "," + 
                                          std::to_string(col + 1) + ")";
                    
                    moves.emplace_back(row, col, value, reasoning, 1.0);
                    found = true;
                }
            }
        }
    }
    
    return found;
}

bool ConstraintSolver::hiddenSingles(Board& board, std::vector<SolverMove>& moves) {
    int size = board.getBoardSize();
    int gridSize = static_cast<int>(sqrt(size));
    bool found = false;
    
    // Check each value (1-9)
    for (int value = 1; value <= size; ++value) {
        // Check rows
        for (int row = 0; row < size; ++row) {
            std::vector<int> possibleCols;
            for (int col = 0; col < size; ++col) {
                if (board.getCell(row, col).getValue() == 0) {
                    std::set<int> candidates = getCandidates(board, row, col);
                    if (candidates.count(value)) {
                        possibleCols.push_back(col);
                    }
                }
            }
            
            if (possibleCols.size() == 1) {
                int col = possibleCols[0];
                std::string reasoning = "Only cell in row " + std::to_string(row + 1) + 
                                      " that can contain " + std::to_string(value);
                moves.emplace_back(row, col, value, reasoning, 0.95);
                found = true;
            }
        }
        
        // Check columns
        for (int col = 0; col < size; ++col) {
            std::vector<int> possibleRows;
            for (int row = 0; row < size; ++row) {
                if (board.getCell(row, col).getValue() == 0) {
                    std::set<int> candidates = getCandidates(board, row, col);
                    if (candidates.count(value)) {
                        possibleRows.push_back(row);
                    }
                }
            }
            
            if (possibleRows.size() == 1) {
                int row = possibleRows[0];
                std::string reasoning = "Only cell in column " + std::to_string(col + 1) + 
                                      " that can contain " + std::to_string(value);
                moves.emplace_back(row, col, value, reasoning, 0.95);
                found = true;
            }
        }
        
        // Check 3x3 boxes
        for (int boxRow = 0; boxRow < gridSize; ++boxRow) {
            for (int boxCol = 0; boxCol < gridSize; ++boxCol) {
                std::vector<std::pair<int,int>> possibleCells;
                
                for (int r = boxRow * gridSize; r < (boxRow + 1) * gridSize; ++r) {
                    for (int c = boxCol * gridSize; c < (boxCol + 1) * gridSize; ++c) {
                        if (board.getCell(r, c).getValue() == 0) {
                            std::set<int> candidates = getCandidates(board, r, c);
                            if (candidates.count(value)) {
                                possibleCells.push_back({r, c});
                            }
                        }
                    }
                }
                
                if (possibleCells.size() == 1) {
                    int row = possibleCells[0].first;
                    int col = possibleCells[0].second;
                    std::string reasoning = "Only cell in 3x3 box that can contain " + std::to_string(value);
                    moves.emplace_back(row, col, value, reasoning, 0.95);
                    found = true;
                }
            }
        }
    }
    
    return found;
}

bool ConstraintSolver::nakedPairs(Board& board, std::vector<SolverMove>& moves) {
    // Simplified naked pairs - find cells with exactly 2 candidates
    int size = board.getBoardSize();
    bool found = false;
    
    for (int row = 0; row < size; ++row) {
        for (int col = 0; col < size; ++col) {
            if (board.getCell(row, col).getValue() == 0) {
                std::set<int> candidates = getCandidates(board, row, col);
                
                if (candidates.size() == 2) {
                    // For now, just add both possibilities with medium confidence
                    for (int value : candidates) {
                        std::string reasoning = "Part of naked pair in cell (" + 
                                              std::to_string(row + 1) + "," + 
                                              std::to_string(col + 1) + ")";
                        moves.emplace_back(row, col, value, reasoning, 0.7);
                        found = true;
                    }
                }
            }
        }
    }
    
    return found;
}

bool ConstraintSolver::pointingPairs(Board& board, std::vector<SolverMove>& moves) {
    // Simplified pointing pairs - advanced technique
    // For now, return false (not implemented)
    return false;
}

std::set<int> ConstraintSolver::getCandidates(const Board& board, int row, int col) const {
    std::set<int> candidates;
    int size = board.getBoardSize();
    
    for (int value = 1; value <= size; ++value) {
        if (isValidMove(board, row, col, value)) {
            candidates.insert(value);
        }
    }
    
    return candidates;
}

bool ConstraintSolver::eliminateCandidate(std::map<std::pair<int,int>, std::set<int>>& candidates, 
                                        int row, int col, int value) {
    auto it = candidates.find({row, col});
    if (it != candidates.end()) {
        return it->second.erase(value) > 0;
    }
    return false;
}

double ConstraintSolver::calculateConfidence(const std::string& strategy, int candidateCount) const {
    if (strategy == "Naked Single") return 1.0;
    if (strategy == "Hidden Single") return 0.95;
    if (strategy == "Naked Pair") return 0.7;
    if (strategy == "Pointing Pair") return 0.6;
    
    // Default confidence based on candidate count
    return candidateCount > 0 ? 1.0 / candidateCount : 0.0;
}
