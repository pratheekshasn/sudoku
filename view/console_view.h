/*
ConsoleView handles all console-based UI rendering and input.
Separates presentation logic from game logic.
*/

#ifndef SUDOKU_VIEW_CONSOLE_VIEW_H
#define SUDOKU_VIEW_CONSOLE_VIEW_H

#include "../model/board.h"
#include <string>

class ConsoleView {
public:
    ConsoleView();
    
    // Display methods
    void showWelcome() const;
    void showHelp() const;
    void showGameStatus(const Board& board, int moveCount) const;
    void showBoard(const Board& board) const;
    void showBoardWithCoordinates(const Board& board) const;
    
    // Message methods
    void showMessage(const std::string& message) const;
    void showError(const std::string& error) const;
    void showSuccess(const std::string& message) const;
    void showWinMessage(int moveCount) const;
    
    // Input methods
    std::string getCommand() const;
    bool getMove(int& row, int& col, int& value) const;
    
    // Utility methods
    void clearScreen() const;
    void waitForEnter() const;
    
private:
    void printSeparator() const;
    void printBoardBorder() const;
};

#endif // SUDOKU_VIEW_CONSOLE_VIEW_H
