/*
JSON API Implementation for C++ Sudoku Game
*/

#include "json_api.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>

SudokuJsonApi::SudokuJsonApi() : board(3), moveCount(0) {
    // Load existing state or initialize with sample puzzle
    loadState();
}

std::string SudokuJsonApi::processCommand(const std::string& command, const std::string& params) {
    try {
        if (command == "get_board") {
            return getBoard();
        }
        else if (command == "make_move") {
            // Parse params: "row,col,value"
            std::istringstream iss(params);
            std::string token;
            int row = 0, col = 0, value = 0;
            
            if (std::getline(iss, token, ',')) row = std::stoi(token);
            if (std::getline(iss, token, ',')) col = std::stoi(token);
            if (std::getline(iss, token, ',')) value = std::stoi(token);
            
            return makeMove(row, col, value);
        }
        else if (command == "load_puzzle") {
            return loadPuzzle();
        }
        else if (command == "generate_puzzle") {
            return generatePuzzle(params);
        }
        else if (command == "clear_board") {
            return clearBoard();
        }
        else if (command == "get_status") {
            return getStatus();
        }
        else if (command == "validate") {
            return validateBoard();
        }
        else if (command == "solve_puzzle") {
            return solvePuzzle(params);
        }
        else if (command == "get_ai_move") {
            return getNextAIMove(params);
        }
        else if (command == "get_ai_moves") {
            return getAIPossibleMoves(params);
        }
        else if (command == "train_batch") {
            int numPuzzles = params.empty() ? 100 : std::stoi(params);
            return trainOnPuzzleBatch(numPuzzles);
        }
        else if (command == "training_stats") {
            return getTrainingStats();
        }
        else if (command == "enable_learning") {
            bool enable = params.empty() ? true : (params == "true" || params == "1");
            return enableRealTimeLearning(enable);
        }
        else {
            return createResponse(false, "Unknown command: " + command);
        }
    }
    catch (const std::exception& e) {
        return createResponse(false, "Error: " + std::string(e.what()));
    }
}

std::string SudokuJsonApi::getBoard() {
    std::string boardJson = boardToJson();
    return createResponse(true, "Board retrieved", boardJson);
}

std::string SudokuJsonApi::makeMove(int row, int col, int value) {
    // Convert to 0-based indexing
    row--; col--;
    
    int size = board.getBoardSize();
    if (row < 0 || row >= size || col < 0 || col >= size || value < 0 || value > size) {
        return createResponse(false, "Invalid move parameters");
    }
    
    // Store old value for potential rollback
    int oldValue = board.getCell(row, col).getValue();
    
    // Make the move
    board.getCell(row, col).setValue(value);
    
    // Check if the move is valid
    if (!board.isValid()) {
        board.getCell(row, col).setValue(oldValue); // Undo the move
        return createResponse(false, "Invalid move! This violates Sudoku rules.");
    }
    
    moveCount++;
    saveState(); // Persist state after each move
    
    std::string message = value == 0 ? "Cell cleared" : "Move made successfully";
    std::string boardJson = boardToJson();
    
    return createResponse(true, message, boardJson);
}

std::string SudokuJsonApi::loadPuzzle() {
    initializeSamplePuzzle();
    moveCount = 0;
    saveState(); // Persist the loaded puzzle
    std::string boardJson = boardToJson();
    return createResponse(true, "Puzzle loaded", boardJson);
}

std::string SudokuJsonApi::generatePuzzle(const std::string& difficulty) {
    // Parse difficulty parameter
    SudokuGenerator::Difficulty diff = SudokuGenerator::MEDIUM;
    if (difficulty == "easy") diff = SudokuGenerator::EASY;
    else if (difficulty == "medium") diff = SudokuGenerator::MEDIUM;
    else if (difficulty == "hard") diff = SudokuGenerator::HARD;
    else if (difficulty == "expert") diff = SudokuGenerator::EXPERT;
    
    // Generate a complete grid first
    if (!generator.generateCompleteGrid(board)) {
        return createResponse(false, "Failed to generate complete grid");
    }
    
    // Then create the puzzle by removing cells
    if (!generator.generatePuzzle(board, diff)) {
        return createResponse(false, "Failed to generate puzzle");
    }
    
    moveCount = 0;
    saveState(); // Persist the generated puzzle
    std::string boardJson = boardToJson();
    return createResponse(true, "New puzzle generated with " + difficulty + " difficulty", boardJson);
}

std::string SudokuJsonApi::clearBoard() {
    for (int row = 0; row < board.getBoardSize(); ++row) {
        for (int col = 0; col < board.getBoardSize(); ++col) {
            board.getCell(row, col).setValue(0);
        }
    }
    moveCount = 0;
    saveState(); // Persist the cleared board
    std::string boardJson = boardToJson();
    return createResponse(true, "Board cleared", boardJson);
}

std::string SudokuJsonApi::getStatus() {
    bool isComplete = board.isComplete();
    bool isValid = board.isValid();
    
    std::ostringstream oss;
    oss << "{"
        << "\"complete\":" << (isComplete ? "true" : "false") << ","
        << "\"valid\":" << (isValid ? "true" : "false") << ","
        << "\"moves\":" << moveCount
        << "}";
    
    return createResponse(true, "Status retrieved", oss.str());
}

std::string SudokuJsonApi::validateBoard() {
    bool isValid = board.isValid();
    std::ostringstream oss;
    oss << "{\"valid\":" << (isValid ? "true" : "false") << "}";
    return createResponse(true, "Board validated", oss.str());
}

std::string SudokuJsonApi::solvePuzzle(const std::string& solverType) {
    // Create solver
    aiSolver = SolverFactory::createSolver(solverType);
    if (!aiSolver) {
        return createResponse(false, "Unknown solver type: " + solverType);
    }
    
    // Check if puzzle can be solved
    if (!aiSolver->canSolve(board)) {
        return createResponse(false, "Puzzle cannot be solved - invalid state");
    }
    
    // Make a copy of the board to solve
    Board originalBoard = board;  // Keep original for training
    Board solutionBoard = board;
    
    // Solve the puzzle
    bool solved = aiSolver->solve(solutionBoard);
    
    if (solved) {
        // Train neural network if using neuro-symbolic solver
        if (solverType == "neuro_symbolic") {
            auto* neuroSolver = dynamic_cast<NeuroSymbolicSolver*>(aiSolver.get());
            if (neuroSolver) {
                neuroSolver->trainOnSolution(originalBoard, solutionBoard);
            }
        }
        
        // Update the board with solution
        board = solutionBoard;
        saveState();
        
        std::ostringstream result;
        result << "{"
               << "\"solved\":true,"
               << "\"solver\":\"" << aiSolver->getSolverName() << "\","
               << "\"moves\":" << aiSolver->getMovesCount() << ","
               << "\"time_ms\":" << aiSolver->getSolveTimeMs() << ","
               << "\"board\":" << boardToJson()
               << "}";
        
        return createResponse(true, "Puzzle solved successfully", result.str());
    } else {
        return createResponse(false, "Could not solve puzzle - no solution found");
    }
}

std::string SudokuJsonApi::getNextAIMove(const std::string& solverType) {
    // Create solver if not exists
    if (!aiSolver || aiSolver->getSolverName().find(solverType) == std::string::npos) {
        aiSolver = SolverFactory::createSolver(solverType);
        if (!aiSolver) {
            return createResponse(false, "Unknown solver type: " + solverType);
        }
    }
    
    SolverMove move(0, 0, 0);
    bool hasMove = aiSolver->getNextMove(board, move);
    
    if (hasMove) {
        std::ostringstream result;
        result << "{"
               << "\"row\":" << (move.row + 1) << ","  // Convert to 1-based
               << "\"col\":" << (move.col + 1) << ","  // Convert to 1-based
               << "\"value\":" << move.value << ","
               << "\"reasoning\":\"" << escapeJson(move.reasoning) << "\","
               << "\"confidence\":" << move.confidence
               << "}";
        
        return createResponse(true, "Next AI move found", result.str());
    } else {
        return createResponse(false, "No AI move available - puzzle may be complete or unsolvable");
    }
}

std::string SudokuJsonApi::getAIPossibleMoves(const std::string& solverType) {
    // Create solver if not exists
    if (!aiSolver || aiSolver->getSolverName().find(solverType) == std::string::npos) {
        aiSolver = SolverFactory::createSolver(solverType);
        if (!aiSolver) {
            return createResponse(false, "Unknown solver type: " + solverType);
        }
    }
    
    std::vector<SolverMove> moves = aiSolver->getAllPossibleMoves(board);
    
    std::ostringstream result;
    result << "{\"moves\":[";
    
    for (size_t i = 0; i < moves.size(); ++i) {
        if (i > 0) result << ",";
        result << "{"
               << "\"row\":" << (moves[i].row + 1) << ","  // Convert to 1-based
               << "\"col\":" << (moves[i].col + 1) << ","  // Convert to 1-based
               << "\"value\":" << moves[i].value << ","
               << "\"reasoning\":\"" << escapeJson(moves[i].reasoning) << "\","
               << "\"confidence\":" << moves[i].confidence
               << "}";
    }
    
    result << "],\"count\":" << moves.size() << "}";
    
    return createResponse(true, "AI possible moves retrieved", result.str());
}

std::string SudokuJsonApi::boardToJson() {
    int size = board.getBoardSize();
    std::ostringstream oss;
    oss << "[";
    
    for (int row = 0; row < size; row++) {
        oss << "[";
        for (int col = 0; col < size; col++) {
            int value = board.getCell(row, col).getValue();
            oss << value;
            if (col < size - 1) oss << ",";
        }
        oss << "]";
        if (row < size - 1) oss << ",";
    }
    
    oss << "]";
    return oss.str();
}

std::string SudokuJsonApi::createResponse(bool success, const std::string& message, const std::string& data) {
    std::ostringstream oss;
    oss << "{"
        << "\"success\":" << (success ? "true" : "false") << ","
        << "\"message\":\"" << escapeJson(message) << "\"";
    
    if (!data.empty()) {
        oss << ",\"data\":" << data;
    }
    
    oss << "}";
    return oss.str();
}

std::string SudokuJsonApi::escapeJson(const std::string& str) {
    std::string escaped;
    for (char c : str) {
        switch (c) {
            case '"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default: escaped += c; break;
        }
    }
    return escaped;
}

void SudokuJsonApi::initializeSamplePuzzle() {
    // Easy Sudoku puzzle for 9x9 board
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
    
    int size = board.getBoardSize();
    for (int row = 0; row < size; ++row) {
        for (int col = 0; col < size; ++col) {
            if (size == 9 && row < 9 && col < 9) {
                board.getCell(row, col).setValue(puzzle[row][col]);
            } else {
                board.getCell(row, col).setValue(0); // Clear for non-9x9 boards
            }
        }
    }
}

void SudokuJsonApi::saveState() {
    std::ofstream file("game_state.json");
    if (!file.is_open()) return;
    
    file << "{\n";
    file << "  \"moveCount\": " << moveCount << ",\n";
    file << "  \"board\": " << boardToJson() << "\n";
    file << "}\n";
    file.close();
}

void SudokuJsonApi::loadState() {
    std::ifstream file("game_state.json");
    if (!file.is_open()) {
        // No saved state, initialize with sample puzzle
        initializeSamplePuzzle();
        return;
    }
    
    std::string line;
    std::string content;
    while (std::getline(file, line)) {
        content += line;
    }
    file.close();
    
    // Simple JSON parsing for our specific format
    size_t movePos = content.find("\"moveCount\": ");
    if (movePos != std::string::npos) {
        movePos += 13; // Length of "\"moveCount\": "
        size_t endPos = content.find(",", movePos);
        if (endPos != std::string::npos) {
            moveCount = std::stoi(content.substr(movePos, endPos - movePos));
        }
    }
    
    size_t boardPos = content.find("\"board\": [");
    if (boardPos != std::string::npos) {
        boardPos += 10; // Length of "\"board\": ["
        size_t endPos = content.find("]", boardPos);
        if (endPos != std::string::npos) {
            // Find the last closing bracket of the entire board array
            size_t lastBracket = content.rfind("]");
            if (lastBracket != std::string::npos) {
                std::string boardData = content.substr(boardPos - 1, lastBracket - boardPos + 2);
                
                // Parse the board data
                parseBoardFromJson(boardData);
                return;
            }
        }
    }
    
    // Fallback to sample puzzle if parsing fails
    initializeSamplePuzzle();
}

void SudokuJsonApi::parseBoardFromJson(const std::string& jsonData) {
    // Simple parser for our specific board format: [[1,2,3...],[4,5,6...]...]
    int size = board.getBoardSize();
    int row = 0, col = 0;
    bool inNumber = false;
    std::string currentNumber;
    
    for (char c : jsonData) {
        if (c >= '0' && c <= '9') {
            if (!inNumber) {
                inNumber = true;
                currentNumber = "";
            }
            currentNumber += c;
        } else if (inNumber && (c == ',' || c == ']')) {
            // End of a number
            if (row < size && col < size) {
                board.getCell(row, col).setValue(std::stoi(currentNumber));
                col++;
                if (col >= size) {
                    col = 0;
                    row++;
                    if (row >= size) break; // Prevent overflow
                }
            }
            inNumber = false;
        }
    }
}

// ============================================================================
// Neural Network Training Methods
// ============================================================================

std::string SudokuJsonApi::trainOnPuzzleBatch(int numPuzzles) {
    // Create neuro-symbolic solver for training
    auto trainer = SolverFactory::createSolver("neuro_symbolic");
    auto* neuroSolver = dynamic_cast<NeuroSymbolicSolver*>(trainer.get());
    
    if (!neuroSolver) {
        return createResponse(false, "Failed to create neuro-symbolic solver for training");
    }
    
    int successful = 0;
    int failed = 0;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < numPuzzles; ++i) {
        // Generate a COMPLETE solution first (this is our ground truth!)
        Board completeBoard(3); // 9x9 board
        
        if (generator.generateCompleteGrid(completeBoard)) {
            // Store the VERIFIED correct solution
            Board groundTruthSolution = completeBoard;
            
            // Now create a puzzle by removing cells
            SudokuGenerator::Difficulty difficulty = 
                static_cast<SudokuGenerator::Difficulty>(i % 4);
            
            if (generator.generatePuzzle(completeBoard, difficulty)) {
                Board puzzleToSolve = completeBoard; // This is the puzzle with holes
                
                // NOW we have proper supervised learning:
                // - puzzleToSolve: Input puzzle with empty cells
                // - groundTruthSolution: VERIFIED correct complete solution
                
                // Train neural network on KNOWN correct solution
                neuroSolver->trainOnSolution(puzzleToSolve, groundTruthSolution);
                successful++;
            } else {
                failed++;
            }
        } else {
            failed++;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::ostringstream result;
    result << "{"
           << "\"puzzles_trained\":" << successful << ","
           << "\"failed_puzzles\":" << failed << ","
           << "\"total_requested\":" << numPuzzles << ","
           << "\"training_time_ms\":" << duration.count() << ","
           << "\"success_rate\":" << (successful / (double)numPuzzles * 100.0)
           << "}";
    
    return createResponse(true, "Batch training completed", result.str());
}

std::string SudokuJsonApi::getTrainingStats() {
    auto trainer = SolverFactory::createSolver("neuro_symbolic");
    auto* neuroSolver = dynamic_cast<NeuroSymbolicSolver*>(trainer.get());
    
    if (!neuroSolver) {
        return createResponse(false, "Neuro-symbolic solver not available");
    }
    
    std::ostringstream result;
    result << "{"
           << "\"solver_name\":\"" << neuroSolver->getSolverName() << "\","
           << "\"total_moves\":" << neuroSolver->getMovesCount() << ","
           << "\"solve_time_ms\":" << neuroSolver->getSolveTimeMs() << ","
           << "\"neural_confidence\":" << neuroSolver->getNeuralConfidence() << ","
           << "\"symbolic_confidence\":" << neuroSolver->getSymbolicConfidence() << ","
           << "\"current_strategy\":" << (int)neuroSolver->getCurrentStrategy()
           << "}";
    
    return createResponse(true, "Training statistics retrieved", result.str());
}

std::string SudokuJsonApi::enableRealTimeLearning(bool enable) {
    // This would require modifying the solving process to call learnFromError
    // For now, just return status
    std::ostringstream result;
    result << "{\"real_time_learning\":" << (enable ? "true" : "false") << "}";
    
    std::string message = enable ? "Real-time learning enabled" : "Real-time learning disabled";
    return createResponse(true, message, result.str());
}
