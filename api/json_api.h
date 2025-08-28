/*
JSON API Interface for C++ Sudoku Game
Provides a command-line interface that outputs JSON for easy integration with web frontend
*/

#ifndef SUDOKU_JSON_API_H
#define SUDOKU_JSON_API_H

#include "../model/board.h"
#include <string>
#include <sstream>

class SudokuJsonApi {
public:
    SudokuJsonApi();
    
    // Main API entry point
    std::string processCommand(const std::string& command, const std::string& params);
    
    // Command handlers
    std::string getBoard();
    std::string makeMove(int row, int col, int value);
    std::string loadPuzzle();
    std::string clearBoard();
    std::string getStatus();
    std::string validateBoard();
    
private:
    Board board;
    int moveCount;
    
    // JSON formatting helpers
    std::string boardToJson();
    std::string createResponse(bool success, const std::string& message, const std::string& data = "");
    std::string escapeJson(const std::string& str);
    void initializeSamplePuzzle();
};

#endif // SUDOKU_JSON_API_H
