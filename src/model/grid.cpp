/*
Grid class represents the 3x3 grids in the sudoku board.
It contains a 3x3 array of Cell objects.
The grid should be able to grow to any size and the size should be configurable.
*/

#include "grid.h"

Grid::Grid(int size) : size(size), cells(size * size) {}

Cell& Grid::getCell(int row, int col) {
    return cells[row * size + col];
}

const Cell& Grid::getCell(int row, int col) const {
    return cells[row * size + col];
}

bool Grid::isComplete() const {
    for (const auto& cell : cells) {
        if (cell.getValue() == 0) {
            return false;
        }
    }
    return true;
}

bool Grid::isValid() const {
    // For a sudoku grid of size NxN, valid values are 1 to N*N
    // Since the sudoku cell values range from 1 to size*size, ignore the first element of seen which is 0
    std::vector<bool> seen(size * size + 1, false);
    for (const auto& cell : cells) {
        int val = cell.getValue();
        if (val != 0) {
            if (seen[val]) {
                return false;
            }
            seen[val] = true;
        }
    }
    return true;
}