/*
Command-line API server for Sudoku game
Accepts commands via command line and outputs JSON responses
*/

#include "json_api.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << R"({"success":false,"message":"Usage: sudoku_api <command> [params]"})" << std::endl;
        return 1;
    }
    
    SudokuJsonApi api;
    std::string command = argv[1];
    std::string params = (argc > 2) ? argv[2] : "";
    
    std::string response = api.processCommand(command, params);
    std::cout << response << std::endl;
    
    return 0;
}
