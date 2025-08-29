/*
Web UI implementation for the Sudoku game.
Communicates with web frontend through JSON API
*/

#ifndef WEB_VIEW_H
#define WEB_VIEW_H

#include "sudoku_view.h"
#include "../model/board.h"
#include <queue>
#include <string>
#include <nlohmann/json.hpp>

class WebView : public SudokuView {
private:
    struct Move {
        int row, col, value;
    };
    
public:
    WebView();
    virtual ~WebView();
    
    // Implement SudokuView interface
    void showWelcome() override;
    void showBoard(const Board& board) override;
    void showBoardWithCoordinates(const Board& board) override;
    void showGameStatus(const Board& board, int moveCount) override;
    
    std::string getCommand() override;
    bool getMove(int& row, int& col, int& value) override;
    
    void showMessage(const std::string& message) override;
    void showError(const std::string& error) override;
    void showSuccess(const std::string& success) override;
    void showWinMessage(int moveCount) override;
    
    void showHelp() override;
    void clearScreen() override;
    void waitForEnter() override;
    
    // Web-specific methods
    std::string getLastMessage() const;
    void queueCommand(const std::string& command);
    void queueMove(int row, int col, int value);
    nlohmann::json serializeBoardToJson(const Board& board);
    std::string getGameStateJson(const Board& board, int moveCount);

private:
    std::string lastMessage;
    std::queue<std::string> commandQueue;
    std::queue<Move> moveQueue;
};

#endif // SUDOKU_VIEW_WEB_VIEW_H
