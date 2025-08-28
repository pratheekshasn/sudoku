/*
GameController coordinates between the model (Board) and view (ConsoleView).
Implements the game logic and flow control.
*/

#ifndef SUDOKU_CONTROLLER_GAME_CONTROLLER_H
#define SUDOKU_CONTROLLER_GAME_CONTROLLER_H

#include "../model/board.h"
#include "../view/console_view.h"
#include <memory>

class GameController {
public:
    GameController(int gridSize = 3);
    
    // Main game flow
    void startGame();
    void gameLoop();
    
    // Game actions
    bool handleCommand(const std::string& command);
    bool makeMove();
    void loadSamplePuzzle();
    void clearBoard();
    
    // Game state
    bool isGameWon() const;
    void checkGameState();
    
private:
    Board board;
    ConsoleView view;
    int moveCount;
    bool gameRunning;
    
    // Helper methods
    bool isValidMove(int row, int col, int value) const;
    void initializeSamplePuzzle();
};

#endif // SUDOKU_CONTROLLER_GAME_CONTROLLER_H
