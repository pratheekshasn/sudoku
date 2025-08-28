/*
Board class represents the entire Sudoku board.
It contains a matrix of Grid objects, where each Grid represents a 3x3 subgrid.
For a standard 9x9 Sudoku, the Board contains a 3x3 matrix of Grid objects.
The board should be able to grow to any size and the size should be configurable.
*/

#ifndef SUDOKU_MODEL_BOARD_H
#define SUDOKU_MODEL_BOARD_H

#include <vector>
#include "grid.h"
#include "cell.h"

class Board {
public:
    // gridSize refers to the size of each subgrid (typically 3 for 3x3 subgrids)
    // The board will be (gridSize x gridSize) subgrids, each of size (gridSize x gridSize)
    Board(int gridSize = 3);
    
    // Access a specific subgrid
    Grid& getGrid(int gridRow, int gridCol);
    const Grid& getGrid(int gridRow, int gridCol) const;
    
    // Access a cell by absolute board coordinates
    Cell& getCell(int row, int col);
    const Cell& getCell(int row, int col) const;
    
    // Board-level operations
    bool isComplete() const;
    bool isValid() const;
    
    // Get the size of each subgrid
    int getGridSize() const { return gridSize; }
    
    // Get the total board size (gridSize * gridSize)
    int getBoardSize() const { return gridSize * gridSize; }

    // Print the board (for debugging)
    void print() const;

private:
    int gridSize;  // Size of each subgrid (3 for standard Sudoku)
    std::vector<Grid> grids;  // 1D vector of grids (gridSize * gridSize total grids)
    
    // Helper function to convert grid coordinates to 1D index
    int getGridIndex(int gridRow, int gridCol) const {
        return gridRow * gridSize + gridCol;
    }
};

#endif // SUDOKU_MODEL_BOARD_H
