/*
Board class represents the entire Sudoku board.
It contains a matrix of Grid objects, where each Grid represents a 3x3 subgrid.
For a standard 9x9 Sudoku, the Board contains a 3x3 matrix of Grid objects.
The board should be able to grow to any size and the size should be configurable.
*/

#include "board.h"
#include <iostream>

Board::Board(int gridSize) : gridSize(gridSize), grids(gridSize * gridSize, Grid(gridSize)) {}

Grid& Board::getGrid(int gridRow, int gridCol) {
    return grids[getGridIndex(gridRow, gridCol)];
}

const Grid& Board::getGrid(int gridRow, int gridCol) const {
    return grids[getGridIndex(gridRow, gridCol)];
}

Cell& Board::getCell(int row, int col) {
    // Convert absolute board coordinates to grid and cell coordinates
    int gridRow = row / gridSize;
    int gridCol = col / gridSize;
    int cellRow = row % gridSize;
    int cellCol = col % gridSize;
    
    return getGrid(gridRow, gridCol).getCell(cellRow, cellCol);
}

const Cell& Board::getCell(int row, int col) const {
    // Convert absolute board coordinates to grid and cell coordinates
    int gridRow = row / gridSize;
    int gridCol = col / gridSize;
    int cellRow = row % gridSize;
    int cellCol = col % gridSize;
    
    return getGrid(gridRow, gridCol).getCell(cellRow, cellCol);
}

bool Board::isComplete() const {
    for (const auto& grid : grids) {
        if (!grid.isComplete()) {
            return false;
        }
    }
    return true;
}

bool Board::isValid() const {
    // Check if all subgrids are valid
    for (const auto& grid : grids) {
        if (!grid.isValid()) {
            return false;
        }
    }
    
    // Check if all rows are valid
    for (int row = 0; row < getBoardSize(); ++row) {
        std::vector<bool> seen(getBoardSize() + 1, false);
        for (int col = 0; col < getBoardSize(); ++col) {
            int val = getCell(row, col).getValue();
            if (val != 0) {
                if (seen[val]) {
                    return false;
                }
                seen[val] = true;
            }
        }
    }
    
    // Check if all columns are valid
    for (int col = 0; col < getBoardSize(); ++col) {
        std::vector<bool> seen(getBoardSize() + 1, false);
        for (int row = 0; row < getBoardSize(); ++row) {
            int val = getCell(row, col).getValue();
            if (val != 0) {
                if (seen[val]) {
                    return false;
                }
                seen[val] = true;
            }
        }
    }
    
    return true;
}

void Board::print() const {
    for (int row = 0; row < getBoardSize(); ++row) {
        for (int col = 0; col < getBoardSize(); ++col) {
            std::cout << getCell(row, col).getValue() << " ";
        }
        std::cout << std::endl;
    }
}
