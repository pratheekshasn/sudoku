#include "model/board.h"
#include <iostream>

int main() {
    Board board(3);  // 3x3 subgrids, creating a 9x9 Sudoku board
    
    std::cout << "Board size: " << board.getBoardSize() << "x" << board.getBoardSize() << std::endl;
    std::cout << "Grid size: " << board.getGridSize() << "x" << board.getGridSize() << std::endl;
    
    // Access cells using absolute board coordinates
    board.getCell(0, 0).setValue(5);  // Top-left cell
    board.getCell(4, 4).setValue(9);  // Center cell
    board.getCell(8, 8).setValue(1);  // Bottom-right cell
    
    // Access specific subgrids
    Grid& topLeftGrid = board.getGrid(0, 0);     // Top-left 3x3 subgrid
    Grid& centerGrid = board.getGrid(1, 1);      // Center 3x3 subgrid
    Grid& bottomRightGrid = board.getGrid(2, 2); // Bottom-right 3x3 subgrid
    
    // Set values in specific subgrids
    topLeftGrid.getCell(1, 1).setValue(7);     // Cell (1,1) in top-left subgrid
    centerGrid.getCell(0, 0).setValue(3);      // Cell (0,0) in center subgrid
    
    // Print some values to verify
    std::cout << "Cell (0,0): " << board.getCell(0, 0).getValue() << std::endl;
    std::cout << "Cell (4,4): " << board.getCell(4, 4).getValue() << std::endl;
    std::cout << "Cell (8,8): " << board.getCell(8, 8).getValue() << std::endl;
    std::cout << "Cell (1,1): " << board.getCell(1, 1).getValue() << std::endl;
    std::cout << "Cell (3,3): " << board.getCell(3, 3).getValue() << std::endl;
    
    std::cout << "Board is complete: " << (board.isComplete() ? "Yes" : "No") << std::endl;
    std::cout << "Board is valid: " << (board.isValid() ? "Yes" : "No") << std::endl;
    
    return 0;
}
