/*
Cell class represents a single cell in the Sudoku grid.
It holds the value of the cell and its possible candidates.
*/

#ifndef SUDOKU_MODEL_CELL_H
#define SUDOKU_MODEL_CELL_H

#include <vector>
#include <algorithm>

class Cell {
public:
    Cell();

    int getValue() const;
    void setValue(int val);

    const std::vector<int>& getCandidates() const;
    void removeCandidate(int candidate);
    void addCandidate(int candidate);

private:
    int value;
    std::vector<int> candidates;
};

#endif // SUDOKU_MODEL_CELL_H
