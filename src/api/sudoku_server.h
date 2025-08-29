/*
REST API Server for Sudoku Game
Provides HTTP endpoints to interact with the C++ game logic
*/

#ifndef SUDOKU_API_SERVER_H
#define SUDOKU_API_SERVER_H

#include "../controller/game_controller.h"
#include <string>
#include <map>

class SudokuApiServer {
public:
    SudokuApiServer(int port = 8080);
    
    // Server lifecycle
    void start();
    void stop();
    
    // API endpoints
    std::string handleRequest(const std::string& method, const std::string& path, const std::string& body);
    
private:
    int port;
    GameController gameController;
    
    // Endpoint handlers
    std::string getBoard();
    std::string makeMove(const std::string& body);
    std::string loadPuzzle();
    std::string clearBoard();
    std::string getGameStatus();
    
    // Utility methods
    std::string boardToJson();
    std::string createJsonResponse(const std::string& status, const std::string& data);
    std::string createErrorResponse(const std::string& error);
    
    // Simple HTTP server (for demonstration)
    void runSimpleServer();
};

#endif // SUDOKU_API_SERVER_H
