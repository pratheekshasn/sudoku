/*
GameController implementation for coordinating game logic and UI.
*/

#include "game_controller.h"
#include "../view/console_view.h"
#include "../view/web_view.h"

GameController::GameController(std::unique_ptr<SudokuView> view, int gridSize) 
    : board(gridSize), view(std::move(view)), moveCount(0), gameRunning(false), stepByStepMode(false) {}

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
    else if (command == "solve" || command == "s") {
        return solvePuzzle(SolverType::NEURO_SYMBOLIC);
    }
    else if (command == "ai" || command == "hint") {
        return getNextAIMove();
    }
    else if (command == "enable_ai" || command == "enable") {
        return enableStepByStepSolving(SolverType::NEURO_SYMBOLIC);
    }
    else if (command == "disable_ai" || command == "disable") {
        disableStepByStepSolving();
        return true;
    }
    else if (command == "hints" || command == "possible") {
        showPossibleMoves();
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
    // Generate a random puzzle instead of using the same one every time
    view->showMessage("🎲 Generating a fresh puzzle for you... Please wait...");
    
    if (generator.generatePuzzle(board, SudokuGenerator::Difficulty::EASY)) {
        view->showSuccess("✨ New puzzle generated! Enjoy solving!");
    } else {
        // Fallback to hardcoded puzzle if generation fails
        view->showMessage("⚠️  Generation failed, loading sample puzzle...");
        
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
    }
    moveCount = 0;
}

// ============================================================================
// AI Solver Integration Methods
// ============================================================================

bool GameController::solvePuzzle(SolverType solverType) {
    view->showMessage("🤖 AI is solving the puzzle... Please wait...");
    
    // Create the appropriate solver
    aiSolver = SolverFactory::createSolver(solverType);
    if (!aiSolver) {
        view->showError("❌ Failed to create solver!");
        return false;
    }
    
    // Check if puzzle can be solved
    if (!aiSolver->canSolve(board)) {
        view->showError("❌ Puzzle cannot be solved - invalid state!");
        return false;
    }
    
    // Make a copy to solve
    Board solutionBoard = board;
    
    // Solve the puzzle
    bool solved = aiSolver->solve(solutionBoard);
    
    if (solved) {
        // Update the board with solution
        board = solutionBoard;
        moveCount += aiSolver->getMovesCount();
        
        view->showSuccess("✨ Puzzle solved by " + aiSolver->getSolverName() + "!");
        view->showMessage("📊 Solver used " + std::to_string(aiSolver->getMovesCount()) + " moves");
        view->showMessage("⏱️  Solve time: " + std::to_string(aiSolver->getSolveTimeMs()) + " ms");
        
        checkGameState(); // Check if game is won
        return true;
    } else {
        view->showError("❌ AI couldn't solve the puzzle!");
        return false;
    }
}

bool GameController::getNextAIMove() {
    if (!aiSolver) {
        view->showError("❌ No AI solver active! Use 'enable step-by-step' first.");
        return false;
    }
    
    SolverMove move{-1, -1, -1, "", 0.0};
    bool hasMove = aiSolver->getNextMove(board, move);
    
    if (hasMove) {
        // Make the AI move
        board.getCell(move.row, move.col).setValue(move.value);
        moveCount++;
        
        view->showSuccess("🤖 AI Move: Row " + std::to_string(move.row + 1) + 
                         ", Col " + std::to_string(move.col + 1) + 
                         ", Value " + std::to_string(move.value));
        view->showMessage("💡 Reasoning: " + move.reasoning);
        view->showMessage("🎯 Confidence: " + std::to_string(move.confidence * 100) + "%");
        
        checkGameState();
        return true;
    } else {
        view->showMessage("🏁 No more AI moves available - puzzle may be complete!");
        return false;
    }
}

bool GameController::enableStepByStepSolving(SolverType solverType) {
    view->showMessage("🎯 Enabling step-by-step AI assistance...");
    
    aiSolver = SolverFactory::createSolver(solverType);
    if (!aiSolver) {
        view->showError("❌ Failed to create solver!");
        return false;
    }
    
    stepByStepMode = true;
    view->showSuccess("✅ Step-by-step mode enabled with " + aiSolver->getSolverName());
    view->showMessage("💡 Use 'ai' command to get next AI move, or 'hints' to see all possible moves");
    
    return true;
}

void GameController::disableStepByStepSolving() {
    aiSolver.reset();
    stepByStepMode = false;
    view->showSuccess("🚫 Step-by-step AI mode disabled");
}

std::vector<SolverMove> GameController::getAIPossibleMoves() const {
    if (!aiSolver) {
        return {};
    }
    
    return aiSolver->getAllPossibleMoves(board);
}

void GameController::showPossibleMoves() {
    if (!aiSolver) {
        view->showError("❌ No AI solver active! Use 'enable_ai' first.");
        return;
    }
    
    std::vector<SolverMove> moves = getAIPossibleMoves();
    
    if (moves.empty()) {
        view->showMessage("🏁 No AI moves available - puzzle may be complete!");
        return;
    }
    
    view->showMessage("🎯 Possible AI moves (top 5):");
    int count = 0;
    for (const auto& move : moves) {
        if (count >= 5) break; // Show only top 5 moves
        
        view->showMessage("  " + std::to_string(count + 1) + ". Row " + 
                         std::to_string(move.row + 1) + ", Col " + 
                         std::to_string(move.col + 1) + ", Value " + 
                         std::to_string(move.value) + 
                         " (Confidence: " + std::to_string(move.confidence * 100) + "%)");
        view->showMessage("     💡 " + move.reasoning);
        count++;
    }
}
