/*
GameController implementation for coordinating game logic and UI.
*/

#include "game_controller.h"
#include "../view/console_view.h"
#include "../view/web_view.h"

GameController::GameController(std::unique_ptr<SudokuView> view, int gridSize) 
    : board(gridSize), view(std::move(view)), moveCount(0), gameRunning(false) {}

std::unique_ptr<GameController> GameController::createConsoleGame(int gridSize) {
    auto consoleView = std::make_unique<ConsoleView>();
    return std::make_unique<GameController>(std::move(consoleView), gridSize);
}

std::unique_ptr<GameController> GameController::createWebGame(int gridSize) {
    auto webView = std::make_unique<WebView>();
    return std::make_unique<GameController>(std::move(webView), gridSize);
}

void GameController::startGame() {
    view->showWelcome();
    initializeSamplePuzzle();
    gameRunning = true;
    gameLoop();
}

void GameController::gameLoop() {
    while (gameRunning) {
        view->showBoardWithCoordinates(board);
        view->showGameStatus(board, moveCount);
        
        std::string command = view->getCommand();
        
        if (!handleCommand(command)) {
            continue;
        }
        
        checkGameState();
        
        if (!gameRunning) {
            break;
        }
        
        view->waitForEnter();
        view->clearScreen();
    }
}

bool GameController::handleCommand(const std::string& command) {
    if (command.empty()) {
        return false;
    }
    
    if (command == "q" || command == "quit") {
        view->showMessage("Thanks for playing! Final move count: " + std::to_string(moveCount));
        gameRunning = false;
        return true;
    }
    else if (command == "m" || command == "move") {
        return makeMove();
    }
    else if (command == "h" || command == "help") {
        view->showHelp();
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
    else if (command == "g" || command == "generate") {
        generateNewPuzzle();
        return true;
    }
    else {
        view->showError("Invalid command. Type 'h' for help.");
        return false;
    }
}

bool GameController::makeMove() {
    int row, col, value;
    
    if (!view->getMove(row, col, value)) {
        return false;
    }
    
    // Convert to 0-based indexing
    row--; col--;
    
    if (!isValidMove(row, col, value)) {
        view->showError("Invalid input! Row and column must be 1-9, value must be 0-9.");
        return false;
    }
    
    // Store old value for validation
    int oldValue = board.getCell(row, col).getValue();
    
    // Make the move
    board.getCell(row, col).setValue(value);
    
    // Check if the move is valid
    if (!board.isValid()) {
        view->showError("Invalid move! This violates Sudoku rules.");
        board.getCell(row, col).setValue(oldValue); // Undo the move
        return false;
    }
    
    moveCount++;
    
    if (value == 0) {
        view->showSuccess("Cell cleared!");
    } else {
        view->showSuccess("Move successful!");
    }
    
    return true;
}

void GameController::loadSamplePuzzle() {
    // Clear the board first
    clearBoard();
    
    // Sample puzzle
    std::vector<std::vector<int>> puzzle = {
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
    
    for (int i = 0; i < board.getBoardSize(); i++) {
        for (int j = 0; j < board.getBoardSize(); j++) {
            board.getCell(i, j).setValue(puzzle[i][j]);
        }
    }
    
    view->showSuccess("Sample puzzle loaded!");
}

void GameController::clearBoard() {
    for (int i = 0; i < board.getBoardSize(); i++) {
        for (int j = 0; j < board.getBoardSize(); j++) {
            board.getCell(i, j).setValue(0);
        }
    }
    moveCount = 0;
    view->showSuccess("Board cleared!");
}

void GameController::generateNewPuzzle(SudokuGenerator::Difficulty difficulty) {
    view->showMessage("🎲 Generating new puzzle... Please wait...");
    
    if (generator.generatePuzzle(board, difficulty)) {
        moveCount = 0;
        view->showSuccess("✨ New puzzle generated! Enjoy solving!");
    } else {
        view->showError("❌ Failed to generate puzzle. Loading sample puzzle instead.");
        loadSamplePuzzle();
    }
}

bool GameController::isGameWon() const {
    return board.isComplete() && board.isValid();
}

void GameController::checkGameState() {
    if (isGameWon()) {
        view->showWinMessage(moveCount);
        gameRunning = false;
    }
}

bool GameController::isValidMove(int row, int col, int value) const {
    return row >= 0 && row < board.getBoardSize() && 
           col >= 0 && col < board.getBoardSize() && 
           value >= 0 && value <= board.getBoardSize();
}

void GameController::initializeSamplePuzzle() {
    // Easy Sudoku puzzle (works for 9x9 boards)
    if (board.getBoardSize() == 9) {
        std::vector<std::vector<int>> puzzle = {
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
        
        for (int row = 0; row < board.getBoardSize(); ++row) {
            for (int col = 0; col < board.getBoardSize(); ++col) {
                board.getCell(row, col).setValue(puzzle[row][col]);
            }
        }
    } else {
        // For non-9x9 boards, just clear the board
        for (int row = 0; row < board.getBoardSize(); ++row) {
            for (int col = 0; col < board.getBoardSize(); ++col) {
                board.getCell(row, col).setValue(0);
            }
        }
    }
    moveCount = 0;
}
