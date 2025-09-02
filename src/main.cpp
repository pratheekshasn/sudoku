/*
Main entry point for the Interactive Sudoku Game.
Uses MVC architecture: Model (Board/Grid/Cell), View (SudokuView interface), Controller (GameController)
*/

#include <iostream>
#include "controller/game_controller.h"
#include "solver/solver_factory.h"

void showSolverMenu() {
    std::cout << "\n🤖 Choose your AI Assistant:\n";
    std::cout << "1. Backtrack Solver (Classic algorithmic approach)\n";
    std::cout << "2. Constraint Solver (Human-like logical deduction)\n"; 
    std::cout << "3. Neuro-Symbolic Solver (AI + Logic hybrid)\n";
    std::cout << "4. No AI (Manual solving only)\n";
    std::cout << "Enter your choice (1-4): ";
}

int main() {
    try {
        std::cout << "🎯 Welcome to Intelligent Sudoku!\n";
        
        // Choose AI solver
        showSolverMenu();
        int choice;
        std::cin >> choice;
        
        // Create console game
        auto game = GameController::createConsoleGame(3);
        
        // Setup AI solver based on choice
        switch (choice) {
            case 1:
                std::cout << "🔧 Backtrack Solver selected!\n";
                game->enableStepByStepSolving(SolverType::BACKTRACK);
                break;
            case 2:
                std::cout << "🧠 Constraint Solver selected!\n";
                game->enableStepByStepSolving(SolverType::CONSTRAINT);
                break;
            case 3:
                std::cout << "🚀 Neuro-Symbolic AI selected!\n";
                game->enableStepByStepSolving(SolverType::NEURO_SYMBOLIC);
                break;
            case 4:
                std::cout << "✋ Manual mode - You're on your own!\n";
                break;
            default:
                std::cout << "🚀 Invalid choice, defaulting to Neuro-Symbolic AI!\n";
                game->enableStepByStepSolving(SolverType::NEURO_SYMBOLIC);
                break;
        }
        
        // Start the interactive game
        game->startGame();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}