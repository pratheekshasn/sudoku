#include "model/grid.h"
#include <iostream>

int main() {
    Grid grid(3);  // 3x3 grid
    
    // Test operator[] syntax
    grid[0][0].setValue(5);
    grid[1][1].setValue(9);
    grid[2][2].setValue(3);
    
    // Test reading with operator[] syntax
    std::cout << "grid[0][0] = " << grid[0][0].getValue() << std::endl;
    std::cout << "grid[1][1] = " << grid[1][1].getValue() << std::endl;
    std::cout << "grid[2][2] = " << grid[2][2].getValue() << std::endl;
    
    // Test const access
    const Grid& constGrid = grid;
    std::cout << "const grid[0][0] = " << constGrid[0][0].getValue() << std::endl;
    
    // Compare with getCell method
    std::cout << "getCell(0,0) = " << grid.getCell(0, 0).getValue() << std::endl;
    
    return 0;
}
