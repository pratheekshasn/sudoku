/*
Main entry point for the Interactive Sudoku Game.
Uses MVC architecture: Model (Board/Grid/Cell), View (SudokuView interface), Controller (GameController)
*/

#include <iostream>
#include "controller/game_controller.h"

int main() {
    try {
        // Create a console-based Sudoku game using the factory method
        auto game = GameController::createConsoleGame(3);
        game->startGame();        // Start the interactive game with MVC architecture
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}