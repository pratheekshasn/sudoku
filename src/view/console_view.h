/*
Console UI implementation for the Sudoku game.
Provides text-based interaction through standard input/output.
*/

#ifndef SUDOKU_VIEW_CONSOLE_VIEW_H
#define SUDOKU_VIEW_CONSOLE_VIEW_H

#include "sudoku_view.h"
#include "../model/board.h"
#include <iostream>
#include <string>

class ConsoleView : public SudokuView {
public:
    ConsoleView();
    
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

private:
    void printBoardBorder();
    void printSeparator();
    void printColoredCell(int value);
};

#endif // SUDOKU_VIEW_CONSOLE_VIEW_H
