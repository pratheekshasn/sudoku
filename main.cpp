/*
Main entry point for the Interactive Sudoku Game.
Uses MVC architecture: Model (Board/Grid/Cell), View (ConsoleView), Controller (GameController)
*/

#include <iostream>
#include "controller/game_controller.h"

int main() {
    try {
        GameController game(3);  // Create a standard 9x9 Sudoku game
        game.startGame();        // Start the interactive game with MVC architecture
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}