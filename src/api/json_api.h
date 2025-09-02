/*
JSON API Interface for C++ Sudoku Game
Provides a command-line interface that outputs JSON for easy integration with web frontend
*/

#ifndef SUDOKU_JSON_API_H
#define SUDOKU_JSON_API_H

#include "../model/board.h"
#include "../model/sudoku_generator.h"
#include "../solver/solver_interface.h"
#include "../solver/solver_factory.h"
#include "../solver/neuro_symbolic_solver.h"
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
    std::string generatePuzzle(const std::string& difficulty = "medium");
    std::string clearBoard();
    std::string getStatus();
    std::string validateBoard();
    
    // AI Solver commands
    std::string solvePuzzle(const std::string& solverType = "backtrack");
    std::string getNextAIMove(const std::string& solverType = "backtrack");
    std::string getAIPossibleMoves(const std::string& solverType = "backtrack");
    
    // Neural Network Training commands
    std::string trainOnPuzzleBatch(int numPuzzles = 100);
    std::string getTrainingStats();
    std::string enableRealTimeLearning(bool enable = true);
    
private:
    Board board;
    SudokuGenerator generator;
    std::unique_ptr<SudokuSolver> aiSolver;
    int moveCount;
    
    // JSON formatting helpers
    std::string boardToJson();
    std::string createResponse(bool success, const std::string& message, const std::string& data = "");
    std::string escapeJson(const std::string& str);
    void initializeSamplePuzzle();
    
    // State persistence
    void saveState();
    void loadState();
    void parseBoardFromJson(const std::string& jsonData);
};

#endif // SUDOKU_JSON_API_H
