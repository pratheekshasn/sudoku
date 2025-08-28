/*
Grid class represents the 3x3 grids in the sudoku board.
It contains a 3x3 array of Cell objects.
The grid should be able to grow to any size and the size should be configurable.
*/

#ifndef SUDOKU_MODEL_GRID_H
#define SUDOKU_MODEL_GRID_H

#include <vector>
#include "cell.h"

class Grid {
public:
    // Proxy class to enable grid[row][col] syntax
    class RowProxy {
    public:
        RowProxy(Grid& grid, int row) : grid_(grid), row_(row) {}
        
        Cell& operator[](int col) {
            return grid_.cells[row_ * grid_.size + col];
        }
        
    private:
        Grid& grid_;
        int row_;
    };
    
    // Const proxy class for const Grid objects
    class ConstRowProxy {
    public:
        ConstRowProxy(const Grid& grid, int row) : grid_(grid), row_(row) {}
        
        const Cell& operator[](int col) const {
            return grid_.cells[row_ * grid_.size + col];
        }
        
    private:
        const Grid& grid_;
        int row_;
    };

    Grid(int size = 3);

    Cell& getCell(int row, int col);
    const Cell& getCell(int row, int col) const;
    
    // Operator overloads for grid[row][col] syntax
    RowProxy operator[](int row) {
        return RowProxy(*this, row);
    }
    
    ConstRowProxy operator[](int row) const {
        return ConstRowProxy(*this, row);
    }

    bool isComplete() const;
    bool isValid() const;

private:
    int size;
    std::vector<Cell> cells;
    
    // Make proxy classes friends so they can access private members
    friend class RowProxy;
    friend class ConstRowProxy;
};

#endif // SUDOKU_MODEL_GRID_H
