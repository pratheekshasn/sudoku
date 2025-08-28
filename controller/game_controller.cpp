/*
GameController implementation for coordinating game logic and UI.
*/

#include "game_controller.h"

GameController::GameController(int gridSize) 
    : board(gridSize), view(), moveCount(0), gameRunning(false) {}

void GameController::startGame() {
    view.showWelcome();
    initializeSamplePuzzle();
    gameRunning = true;
    gameLoop();
}

void GameController::gameLoop() {
    while (gameRunning) {
        view.showBoardWithCoordinates(board);
        view.showGameStatus(board, moveCount);
        
        std::string command = view.getCommand();
        
        if (!handleCommand(command)) {
            continue;
        }
        
        checkGameState();
        
        if (!gameRunning) {
            break;
        }
        
        view.waitForEnter();
        view.clearScreen();
    }
}

bool GameController::handleCommand(const std::string& command) {
    if (command.empty()) {
        return false;
    }
    
    if (command == "q" || command == "quit") {
        view.showMessage("Thanks for playing! Final move count: " + std::to_string(moveCount));
        gameRunning = false;
        return true;
    }
    else if (command == "m" || command == "move") {
        return makeMove();
    }
    else if (command == "h" || command == "help") {
        view.showHelp();
        return true;
    }
    else if (command == "c" || command == "clear") {
        clearBoard();
        return true;
    }
    else if (command == "l" || command == "load") {
        loadSamplePuzzle();
        return true;
    }
    else {
        view.showError("Invalid command. Type 'h' for help.");
        return false;
    }
}

bool GameController::makeMove() {
    int row, col, value;
    
    if (!view.getMove(row, col, value)) {
        return false;
    }
    
    // Convert to 0-based indexing
    row--; col--;
    
    if (!isValidMove(row, col, value)) {
        view.showError("Invalid input! Row and column must be 1-9, value must be 0-9.");
        return false;
    }
    
    // Store old value for validation
    int oldValue = board.getCell(row, col).getValue();
    
    // Make the move
    board.getCell(row, col).setValue(value);
    
    // Check if the move is valid
    if (!board.isValid()) {
        view.showError("Invalid move! This violates Sudoku rules.");
        board.getCell(row, col).setValue(oldValue); // Undo the move
        return false;
    }
    
    moveCount++;
    
    if (value == 0) {
        view.showSuccess("Cell cleared!");
    } else {
        view.showSuccess("Move successful!");
    }
    
    return true;
}

void GameController::loadSamplePuzzle() {
    clearBoard();
    initializeSamplePuzzle();
    view.showSuccess("Sample puzzle loaded!");
}

void GameController::clearBoard() {
    for (int row = 0; row < board.getBoardSize(); ++row) {
        for (int col = 0; col < board.getBoardSize(); ++col) {
            board.getCell(row, col).setValue(0);
        }
    }
    moveCount = 0;
    view.showSuccess("Board cleared!");
}

bool GameController::isGameWon() const {
    return board.isComplete() && board.isValid();
}

void GameController::checkGameState() {
    if (isGameWon()) {
        view.showWinMessage(moveCount);
        gameRunning = false;
    }
}

bool GameController::isValidMove(int row, int col, int value) const {
    return row >= 0 && row < board.getBoardSize() && 
           col >= 0 && col < board.getBoardSize() && 
           value >= 0 && value <= board.getBoardSize();
}

void GameController::initializeSamplePuzzle() {
    // Easy Sudoku puzzle
    int puzzle[9][9] = {
        {5, 3, 0, 0, 7, 0, 0, 0, 0},
        {6, 0, 0, 1, 9, 5, 0, 0, 0},
        {0, 9, 8, 0, 0, 0, 0, 6, 0},
        {8, 0, 0, 0, 6, 0, 0, 0, 3},
        {4, 0, 0, 8, 0, 3, 0, 0, 1},
        {7, 0, 0, 0, 2, 0, 0, 0, 6},
        {0, 6, 0, 0, 0, 0, 2, 8, 0},
        {0, 0, 0, 4, 1, 9, 0, 0, 5},
        {0, 0, 0, 0, 8, 0, 0, 7, 9}
    };
    
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            board.getCell(row, col).setValue(puzzle[row][col]);
        }
    }
    moveCount = 0;
}
