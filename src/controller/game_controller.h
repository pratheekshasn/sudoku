/*
GameController coordinates between the model (Board) and view (SudokuView interface).
Implements the game logic and flow control with pluggable view backends.
*/

#ifndef SUDOKU_CONTROLLER_GAME_CONTROLLER_H
#define SUDOKU_CONTROLLER_GAME_CONTROLLER_H

#include "../model/board.h"
#include "../model/sudoku_generator.h"
#include "../view/sudoku_view.h"
#include <memory>

class GameController {
public:
    // Constructor with pluggable view
    GameController(std::unique_ptr<SudokuView> view, int gridSize = 3);
    
    // Convenience constructors
    static std::unique_ptr<GameController> createConsoleGame(int gridSize = 3);
    static std::unique_ptr<GameController> createWebGame(int gridSize = 3);
    
    // Main game flow
    void startGame();
    void gameLoop();
    
    // Game actions
    bool handleCommand(const std::string& command);
    bool makeMove();
    void loadSamplePuzzle();
    void generateNewPuzzle(SudokuGenerator::Difficulty difficulty = SudokuGenerator::MEDIUM);
    void clearBoard();
    
    // Game state
    bool isGameWon() const;
    void checkGameState();
    
    // Access to components (for web API)
    Board& getBoard() { return board; }
    const Board& getBoard() const { return board; }
    SudokuView& getView() { return *view; }
    int getMoveCount() const { return moveCount; }
    
private:
    Board board;
    std::unique_ptr<SudokuView> view;
    SudokuGenerator generator;
    int moveCount;
    bool gameRunning;
    
    // Helper methods
    bool isValidMove(int row, int col, int value) const;
    void initializeSamplePuzzle();
};

#endif // SUDOKU_CONTROLLER_GAME_CONTROLLER_H
