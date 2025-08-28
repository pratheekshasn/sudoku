/*
Abstract View Interface for Sudoku Game
Defines the contract that all view implementations must follow
*/

#ifndef SUDOKU_VIEW_H
#define SUDOKU_VIEW_H

#include "../model/board.h"
#include <string>
#include <memory>

// Abstract base class (interface) for all Sudoku views
class SudokuView {
public:
    virtual ~SudokuView() = default;
    
    // Core display methods
    virtual void showWelcome() = 0;
    virtual void showBoard(const Board& board) = 0;
    virtual void showBoardWithCoordinates(const Board& board) = 0;
    virtual void showGameStatus(const Board& board, int moveCount) = 0;
    
    // User interaction
    virtual std::string getCommand() = 0;
    virtual bool getMove(int& row, int& col, int& value) = 0;
    
    // Feedback messages
    virtual void showMessage(const std::string& message) = 0;
    virtual void showError(const std::string& error) = 0;
    virtual void showSuccess(const std::string& success) = 0;
    virtual void showWinMessage(int moveCount) = 0;
    
    // Utility methods
    virtual void showHelp() = 0;
    virtual void clearScreen() = 0;
    virtual void waitForEnter() = 0;
};

// Factory function to create views
enum class ViewType {
    CONSOLE,
    WEB
};

std::unique_ptr<SudokuView> createView(ViewType type);

#endif // SUDOKU_VIEW_H
