/*
SudokuGame class provides an interactive gameplay experience.
Handles user input, game state, and provides a user-friendly interface.
*/

#ifndef SUDOKU_GAME_H
#define SUDOKU_GAME_H

#include "model/board.h"
#include <iostream>
#include <string>
#include <vector>

class SudokuGame {
public:
    SudokuGame(int gridSize = 3);
    
    // Main game loop
    void play();
    
    // Game actions
    void showHelp() const;
    void printBoard() const;
    void printBoardWithCoordinates() const;
    bool makeMove();
    void loadPuzzle();
    void clearBoard();
    
    // Game state
    bool isGameComplete() const;
    void showGameStatus() const;
    
private:
    Board board;
    int moveCount;
    
    // Helper methods
    bool isValidInput(int row, int col, int value) const;
    void printSeparator() const;
    void clearScreen() const;
    bool getUserMove(int& row, int& col, int& value);
    void initializeSamplePuzzle();
};

#endif // SUDOKU_GAME_H
