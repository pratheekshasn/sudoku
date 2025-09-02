/*
Solver Factory Implementation
*/

#include "solver_factory.h"
#include "backtrack_solver.h"
#include "constraint_solver.h"
#include "neuro_symbolic_solver.h"

// Static member initialization
std::map<std::string, SolverType> SolverFactory::nameToTypeMap;
std::map<SolverType, std::string> SolverFactory::typeToNameMap;

std::unique_ptr<SudokuSolver> SolverFactory::createSolver(SolverType type) {
    switch (type) {
        case SolverType::BACKTRACK:
            return std::make_unique<BacktrackSolver>();
        
        case SolverType::CONSTRAINT:
            return std::make_unique<ConstraintSolver>();
        
        case SolverType::HEURISTIC:
            // TODO: Implement HeuristicSolver
            return nullptr;
        
        case SolverType::AI_NEURAL:
            // TODO: Implement AISolver
            return nullptr;
        
        case SolverType::NEURO_SYMBOLIC:
            return std::make_unique<NeuroSymbolicSolver>();
        
        default:
            return nullptr;
    }
}

std::unique_ptr<SudokuSolver> SolverFactory::createSolver(const std::string& name) {
    initializeMaps();
    
    auto it = nameToTypeMap.find(name);
    if (it != nameToTypeMap.end()) {
        return createSolver(it->second);
    }
    
    return nullptr;
}

std::vector<SolverType> SolverFactory::getAvailableSolvers() {
    return {
        SolverType::BACKTRACK,
        SolverType::CONSTRAINT,
        SolverType::NEURO_SYMBOLIC
        // Add more as they're implemented
    };
}

std::vector<std::string> SolverFactory::getAvailableSolverNames() {
    initializeMaps();
    
    std::vector<std::string> names;
    for (auto type : getAvailableSolvers()) {
        auto it = typeToNameMap.find(type);
        if (it != typeToNameMap.end()) {
            names.push_back(it->second);
        }
    }
    
    return names;
}

std::string SolverFactory::getSolverDescription(SolverType type) {
    switch (type) {
        case SolverType::BACKTRACK:
            return "Classic recursive backtracking algorithm";
        case SolverType::CONSTRAINT:
            return "Constraint propagation with backtracking";
        case SolverType::HEURISTIC:
            return "Advanced heuristics and strategies";
        case SolverType::AI_NEURAL:
            return "Machine learning neural network solver";
        case SolverType::NEURO_SYMBOLIC:
            return "Hybrid neural-symbolic reasoning solver";
        default:
            return "Unknown solver type";
    }
}

SolverDifficulty SolverFactory::getSolverDifficulty(SolverType type) {
    switch (type) {
        case SolverType::BACKTRACK:
            return SolverDifficulty::BASIC;
        case SolverType::CONSTRAINT:
            return SolverDifficulty::ADVANCED;
        case SolverType::HEURISTIC:
            return SolverDifficulty::EXPERT;
        case SolverType::AI_NEURAL:
            return SolverDifficulty::AI_NEURAL;
        case SolverType::NEURO_SYMBOLIC:
            return SolverDifficulty::AI_NEURAL;
        default:
            return SolverDifficulty::BASIC;
    }
}

void SolverFactory::initializeMaps() {
    if (nameToTypeMap.empty()) {
        nameToTypeMap["backtrack"] = SolverType::BACKTRACK;
        nameToTypeMap["constraint"] = SolverType::CONSTRAINT;
        nameToTypeMap["heuristic"] = SolverType::HEURISTIC;
        nameToTypeMap["ai_neural"] = SolverType::AI_NEURAL;
        nameToTypeMap["neuro_symbolic"] = SolverType::NEURO_SYMBOLIC;
        
        typeToNameMap[SolverType::BACKTRACK] = "backtrack";
        typeToNameMap[SolverType::CONSTRAINT] = "constraint";
        typeToNameMap[SolverType::HEURISTIC] = "heuristic";
        typeToNameMap[SolverType::AI_NEURAL] = "ai_neural";
        typeToNameMap[SolverType::NEURO_SYMBOLIC] = "neuro_symbolic";
    }
}
