/*
JSON API Implementation for C++ Sudoku Game
*/

#include "json_api.h"
#include <iostream>
#include <iomanip>

SudokuJsonApi::SudokuJsonApi() : board(3), moveCount(0) {
    // Initialize with a sample puzzle
    initializeSamplePuzzle();
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
        else if (command == "clear_board") {
            return clearBoard();
        }
        else if (command == "get_status") {
            return getStatus();
        }
        else if (command == "validate") {
            return validateBoard();
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
    
    if (row < 0 || row >= 9 || col < 0 || col >= 9 || value < 0 || value > 9) {
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
    
    std::string message = value == 0 ? "Cell cleared" : "Move made successfully";
    std::string boardJson = boardToJson();
    
    return createResponse(true, message, boardJson);
}

std::string SudokuJsonApi::loadPuzzle() {
    initializeSamplePuzzle();
    moveCount = 0;
    std::string boardJson = boardToJson();
    return createResponse(true, "Puzzle loaded", boardJson);
}

std::string SudokuJsonApi::clearBoard() {
    for (int row = 0; row < board.getBoardSize(); ++row) {
        for (int col = 0; col < board.getBoardSize(); ++col) {
            board.getCell(row, col).setValue(0);
        }
    }
    moveCount = 0;
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

std::string SudokuJsonApi::boardToJson() {
    std::ostringstream oss;
    oss << "[";
    
    for (int row = 0; row < 9; row++) {
        oss << "[";
        for (int col = 0; col < 9; col++) {
            int value = board.getCell(row, col).getValue();
            oss << value;
            if (col < 8) oss << ",";
        }
        oss << "]";
        if (row < 8) oss << ",";
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
}
