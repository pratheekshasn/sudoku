/*
SudokuGenerator class for creating valid Sudoku puzzles
Uses backtracking algorithm to generate complete grids and then removes cells for puzzles
*/

#ifndef SUDOKU_GENERATOR_H
#define SUDOKU_GENERATOR_H

#include "board.h"
#include <random>
#include <vector>

class SudokuGenerator {
public:
    SudokuGenerator();
    
    // Generate a complete, valid Sudoku grid
    bool generateCompleteGrid(Board& board);
    
    // Generate a puzzle by removing cells from a complete grid
    bool generatePuzzle(Board& board, int difficulty = 40);
    
    // Difficulty levels (number of cells to remove)
    enum Difficulty {
        EASY = 30,     // Remove 30 cells
        MEDIUM = 40,   // Remove 40 cells  
        HARD = 50,     // Remove 50 cells
        EXPERT = 55    // Remove 55 cells
    };
    
private:
    std::mt19937 rng;
    
    // Helper methods for generation
    bool fillGrid(Board& board);
    bool isValidPlacement(const Board& board, int row, int col, int value);
    void shuffleArray(std::vector<int>& arr);
    bool hasUniqueSolution(Board& board);
    int countSolutions(Board board, int maxSolutions = 2);
    bool solvePuzzle(Board& board);
};

#endif // SUDOKU_GENERATOR_H
