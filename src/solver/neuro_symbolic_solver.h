/*
Neuro-Symbolic Solver - Hybrid AI approach
Combines neural networks for pattern recognition with symbolic reasoning for logical deduction
*/

#ifndef SUDOKU_NEURO_SYMBOLIC_SOLVER_H
#define SUDOKU_NEURO_SYMBOLIC_SOLVER_H

#include "solver_interface.h"
#include <vector>
#include <map>
#include <memory>
#include <random>

// Simplified neural network for pattern recognition
class SudokuNeuralNetwork {
public:
    SudokuNeuralNetwork(int boardSize = 9);
    
    // Predict confidence for a move based on board patterns
    double predictMoveConfidence(const Board& board, int row, int col, int value);
    
    // Learn from successful moves (simplified training)
    void updateWeights(const Board& board, int row, int col, int value, bool wasCorrect);
    
    // Get pattern-based difficulty assessment
    double assessDifficulty(const Board& board);
    
    // Adapt to new board size (reinitializes network)
    void adaptToBoardSize(int newSize);

private:
    int boardSize;
    int inputSize;
    int hiddenSize;
    
    struct Neuron {
        std::vector<double> weights;
        double bias;
        double output;
    };
    
    // Adaptive 3-layer network: input -> hidden -> output
    std::vector<Neuron> hiddenLayer;
    std::vector<Neuron> outputLayer;
    
    // Extract features from board state around a cell (size-adaptive)
    std::vector<double> extractFeatures(const Board& board, int row, int col, int value);
    
    // Forward propagation
    double forward(const std::vector<double>& features);
    
    // Initialize network weights for current board size
    void initializeNetwork();
    
    // Calculate optimal network architecture for board size
    void calculateNetworkSize();
    
    // Learning rate for weight updates
    double learningRate = 0.01;
    std::mt19937 rng;
};

// Symbolic reasoning engine
class SymbolicReasoner {
public:
    SymbolicReasoner();
    
    // Apply logical rules and constraints
    std::vector<SolverMove> applyLogicalRules(const Board& board);
    
    // Validate moves using constraint satisfaction
    bool validateMove(const Board& board, int row, int col, int value);
    
    // Find forced moves using constraint propagation
    std::vector<SolverMove> findForcedMoves(const Board& board);
    
    // Detect logical patterns (naked singles, hidden singles, etc.)
    std::vector<SolverMove> detectPatterns(const Board& board);

private:
    // Rule-based pattern detection
    bool isNakedSingle(const Board& board, int row, int col, int& value);
    bool isHiddenSingle(const Board& board, int row, int col, int value);
    
    // Constraint satisfaction helpers
    std::vector<int> getCandidates(const Board& board, int row, int col);
    bool violatesConstraints(const Board& board, int row, int col, int value);
};

class NeuroSymbolicSolver : public SudokuSolver {
public:
    // Hybrid strategy selection
    enum class Strategy {
        SYMBOLIC_FIRST,    // Use logic first, neural for ambiguous cases
        NEURAL_GUIDED,     // Neural guides symbolic search
        BALANCED_FUSION    // Equal weight to both approaches
    };
    
    explicit NeuroSymbolicSolver(int boardSize = 9);
    
    // Core solving methods
    bool solve(Board& board) override;
    bool canSolve(const Board& board) const override;
    
    // Step-by-step solving with hybrid reasoning
    bool getNextMove(const Board& board, SolverMove& move) override;
    std::vector<SolverMove> getAllPossibleMoves(const Board& board) override;
    
    // Solver information
    std::string getSolverName() const override { return "Neuro-Symbolic Solver"; }
    SolverDifficulty getDifficulty() const override { return SolverDifficulty::AI_NEURAL; }
    std::string getDescription() const override { 
        return "Hybrid AI solver combining neural pattern recognition with symbolic reasoning"; 
    }
    
    // Training interface
    void trainOnSolution(const Board& originalBoard, const Board& solvedBoard);
    
    // Adapt neural network to different board size
    void adaptToBoardSize(int newSize);
    
    // Getters for inspection
    double getNeuralConfidence() const { return lastNeuralConfidence; }
    double getSymbolicConfidence() const { return lastSymbolicConfidence; }
    Strategy getCurrentStrategy() const { return currentStrategy; }

private:
    std::unique_ptr<SudokuNeuralNetwork> neuralNet;
    std::unique_ptr<SymbolicReasoner> symbolicReasoner;
    
    // Hybrid decision making
    SolverMove combineNeuralAndSymbolic(const Board& board, 
                                       const std::vector<SolverMove>& symbolicMoves,
                                       const std::vector<SolverMove>& neuralMoves);
    
    // Confidence fusion
    double fuseConfidences(double neuralConf, double symbolicConf, 
                          const std::string& reasoning);
    
    // Learning from mistakes
    void learnFromError(const Board& board, const SolverMove& move, bool wasCorrect);
    
    // Performance tracking
    double lastNeuralConfidence = 0.0;
    double lastSymbolicConfidence = 0.0;
    int correctPredictions = 0;
    int totalPredictions = 0;
    
    Strategy currentStrategy = Strategy::BALANCED_FUSION;
};

#endif // SUDOKU_NEURO_SYMBOLIC_SOLVER_H
