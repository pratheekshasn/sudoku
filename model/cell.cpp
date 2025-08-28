/*
Cell class represents a single cell in the Sudoku grid.
It holds the value of the cell and its possible candidates.
*/

#include "cell.h"

Cell::Cell() : value(0), candidates{1, 2, 3, 4, 5, 6, 7, 8, 9} {}

int Cell::getValue() const {
    return value;
}

void Cell::setValue(int val) {
    value = val;
}

const std::vector<int>& Cell::getCandidates() const {
    return candidates;
}

void Cell::removeCandidate(int candidate) {
    candidates.erase(std::remove(candidates.begin(), candidates.end(), candidate), candidates.end());
}

void Cell::addCandidate(int candidate) {
    if (std::find(candidates.begin(), candidates.end(), candidate) == candidates.end()) {
        candidates.push_back(candidate);
    }
}
