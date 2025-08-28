/*
Main entry point for the Interactive Sudoku Game.
*/

#include <iostream>
#include "sudoku_game.h"

int main() {
    try {
        SudokuGame game(3);  // Create a standard 9x9 Sudoku game
        game.play();         // Start the interactive game
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}