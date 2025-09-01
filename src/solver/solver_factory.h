/*
Solver Factory - Creates different types of solvers
Implements Factory Pattern for solver instantiation
*/

#ifndef SUDOKU_SOLVER_FACTORY_H
#define SUDOKU_SOLVER_FACTORY_H

#include "solver_interface.h"
#include "backtrack_solver.h"
#include <memory>
#include <map>
#include <string>

enum class SolverType {
    BACKTRACK,
    CONSTRAINT,  // Future: constraint propagation
    HEURISTIC,   // Future: advanced heuristics
    AI_NEURAL    // Future: machine learning
};

class SolverFactory {
public:
    // Create solver by type
    static std::unique_ptr<SudokuSolver> createSolver(SolverType type);
    
    // Create solver by name
    static std::unique_ptr<SudokuSolver> createSolver(const std::string& name);
    
    // Get available solver types
    static std::vector<SolverType> getAvailableSolvers();
    static std::vector<std::string> getAvailableSolverNames();
    
    // Get solver information without creating instance
    static std::string getSolverDescription(SolverType type);
    static SolverDifficulty getSolverDifficulty(SolverType type);

private:
    static std::map<std::string, SolverType> nameToTypeMap;
    static std::map<SolverType, std::string> typeToNameMap;
    
    static void initializeMaps();
};

#endif // SUDOKU_SOLVER_FACTORY_H
