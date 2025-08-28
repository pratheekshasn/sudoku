#include "web_view.h"
#include <iostream>
#include <sstream>

WebView::WebView() : lastMessage("") {
    // Initialize web view - this could include setting up communication channels
}

WebView::~WebView() {
    // Cleanup
}

void WebView::showWelcome() {
    showMessage("Welcome to Web Sudoku!");
}

void WebView::showBoard(const Board& board) {
    nlohmann::json boardJson = serializeBoardToJson(board);
    
    // Send board update to web client
    std::cout << "BOARD_UPDATE:" << boardJson.dump() << std::endl;
}

void WebView::showBoardWithCoordinates(const Board& board) {
    // For web interface, this is the same as showBoard since coordinates are handled by the frontend
    showBoard(board);
}

void WebView::showGameStatus(const Board& board, int moveCount) {
    nlohmann::json statusJson;
    statusJson["type"] = "status";
    statusJson["board"] = serializeBoardToJson(board);
    statusJson["moveCount"] = moveCount;
    statusJson["isComplete"] = board.isComplete() && board.isValid();
    
    std::cout << "STATUS:" << statusJson.dump() << std::endl;
}

std::string WebView::getCommand() {
    // In a web interface, commands come from the command queue
    // This is typically called by the game loop to check for pending commands
    
    if (!commandQueue.empty()) {
        std::string command = commandQueue.front();
        commandQueue.pop();
        return command;
    }
    
    // For web interface, we might want to return a special command like "wait"
    // or handle this differently based on the async nature of web communication
    return "wait";
}

bool WebView::getMove(int& row, int& col, int& value) {
    // In web interface, moves come from the move queue
    if (!moveQueue.empty()) {
        Move move = moveQueue.front();
        moveQueue.pop();
        
        row = move.row;
        col = move.col;
        value = move.value;
        return true;
    }
    
    return false; // No move available
}

void WebView::showMessage(const std::string& message) {
    lastMessage = message;
    nlohmann::json messageJson;
    messageJson["type"] = "message";
    messageJson["content"] = message;
    
    std::cout << "MESSAGE:" << messageJson.dump() << std::endl;
}

void WebView::showError(const std::string& error) {
    lastMessage = error;
    nlohmann::json errorJson;
    errorJson["type"] = "error";
    errorJson["content"] = error;
    
    std::cout << "ERROR:" << errorJson.dump() << std::endl;
}

void WebView::showSuccess(const std::string& success) {
    lastMessage = success;
    nlohmann::json successJson;
    successJson["type"] = "success";
    successJson["content"] = success;
    
    std::cout << "SUCCESS:" << successJson.dump() << std::endl;
}

void WebView::showWinMessage(int moveCount) {
    std::string winMessage = "Congratulations! You solved the puzzle in " + std::to_string(moveCount) + " moves!";
    lastMessage = winMessage;
    
    nlohmann::json winJson;
    winJson["type"] = "win";
    winJson["moveCount"] = moveCount;
    winJson["content"] = winMessage;
    
    std::cout << "WIN:" << winJson.dump() << std::endl;
}

void WebView::showHelp() {
    std::string helpMessage = "Sudoku Rules: Fill the 9x9 grid so that each row, column, and 3x3 box contains digits 1-9.";
    showMessage(helpMessage);
}

void WebView::clearScreen() {
    // For web interface, clearing screen might not be applicable
    // or could send a clear command to the frontend
    std::cout << "CLEAR_SCREEN" << std::endl;
}

void WebView::waitForEnter() {
    // For web interface, this might not be needed
    // or could be handled differently
}

std::string WebView::getLastMessage() const {
    return lastMessage;
}

void WebView::queueCommand(const std::string& command) {
    commandQueue.push(command);
}

void WebView::queueMove(int row, int col, int value) {
    moveQueue.push({row, col, value});
}

nlohmann::json WebView::serializeBoardToJson(const Board& board) {
    nlohmann::json boardJson = nlohmann::json::array();
    
    for (int i = 0; i < 9; i++) {
        nlohmann::json row = nlohmann::json::array();
        for (int j = 0; j < 9; j++) {
            row.push_back(board.getCell(i, j).getValue());
        }
        boardJson.push_back(row);
    }
    
    return boardJson;
}

std::string WebView::getGameStateJson(const Board& board, int moveCount) {
    nlohmann::json gameState;
    gameState["board"] = serializeBoardToJson(board);
    gameState["moveCount"] = moveCount;
    gameState["isComplete"] = board.isComplete() && board.isValid();
    
    return gameState.dump();
}
