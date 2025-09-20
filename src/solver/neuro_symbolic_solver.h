/*
Symbolic-Informed Neural Solver - Advanced Hybrid AI approach
The neural network receives direct input from symbolic reasoning, creating a tightly
integrated system where logical deduction influences pattern recognition at the neural level.

Architecture Change:
- OLD: Neural Net + Symbolic Reasoner → Confidence Fusion  
- NEW: Symbolic Reasoner → Neural Net (with symbolic hints as input features)

This allows the neural network to learn not just from patterns, but from logical
reasoning principles, creating a more interpretable and effective AI system.
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
    
    // Predict confidence for a move based on board patterns and symbolic hints
    double predictMoveConfidence(const Board& board, int row, int col, int value, 
                                const std::vector<double>& symbolicHints = {});
    
    // Pure neural prediction without symbolic hints (for true testing)
    double predictMoveConfidencePure(const Board& board, int row, int col, int value);
    
    // Learn from successful moves (simplified training)
    void updateWeights(const Board& board, int row, int col, int value, bool wasCorrect,
                      const std::vector<double>& symbolicHints = {});
    
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
    std::vector<double> extractFeatures(const Board& board, int row, int col, int value,
                                       const std::vector<double>& symbolicHints = {});
    
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

// Symbolic reasoning engine - generates hints for neural network
class SymbolicReasoner {
public:
    SymbolicReasoner();
    
    // Validate moves using constraint satisfaction
    bool validateMove(const Board& board, int row, int col, int value);
    
    // Generate symbolic hints for neural network (main purpose)
    std::vector<double> generateSymbolicHints(const Board& board, int row, int col, int value);

private:
    // Rule-based pattern detection for hints
    bool isNakedSingle(const Board& board, int row, int col, int& value);
    bool isHiddenSingle(const Board& board, int row, int col, int value);
    
    // Constraint satisfaction helpers
    std::vector<int> getCandidates(const Board& board, int row, int col);
    bool violatesConstraints(const Board& board, int row, int col, int value);
};

class NeuroSymbolicSolver : public SudokuSolver {
public:
    explicit NeuroSymbolicSolver(int boardSize = 9);
    
    // Core solving methods
    bool solve(Board& board) override;
    bool canSolve(const Board& board) const override;
    
    // Step-by-step solving with symbolic-informed neural network
    bool getNextMove(const Board& board, SolverMove& move) override;
    std::vector<SolverMove> getAllPossibleMoves(const Board& board) override;
    
    // Solver information
    std::string getSolverName() const override { return "Symbolic-Informed Neural Solver"; }
    SolverDifficulty getDifficulty() const override { return SolverDifficulty::AI_NEURAL; }
    std::string getDescription() const override { 
        return "Neural network enhanced with symbolic reasoning hints as input features"; 
    }
    
    // Training interface
    void trainOnSolution(const Board& originalBoard, const Board& solvedBoard);
    
    // Training mode control
    void setTrainingMode(bool training) { isTrainingMode = training; }
    bool isInTrainingMode() const { return isTrainingMode; }
    
    // Cross-validation system
    struct CrossValidationResult {
        double accuracy;
        double averageConfidence;
        double averageSolveTime;
        int totalPuzzles;
        int correctSolutions;
        int partialSolutions;
        int failedSolutions;
        std::vector<double> foldAccuracies;
        std::string detailedReport;
    };
    
    CrossValidationResult performCrossValidation(const std::vector<std::pair<Board, Board>>& puzzleSolutionPairs,
                                                int kFolds = 5, bool verbose = false);
    
    // Performance metrics
    struct PerformanceMetrics {
        double precision;
        double recall;
        double f1Score;
        double meanAbsoluteError;
        int truePositives;
        int falsePositives;
        int trueNegatives;
        int falseNegatives;
    };
    
    PerformanceMetrics calculatePerformanceMetrics(const std::vector<std::pair<Board, Board>>& testSet);
    
    // Training utilities
    void resetNetwork();
    void saveNetworkState(const std::string& filename);
    bool loadNetworkState(const std::string& filename);
    
    // Adapt neural network to different board size
    void adaptToBoardSize(int newSize);

private:
    std::unique_ptr<SudokuNeuralNetwork> neuralNet;
    std::unique_ptr<SymbolicReasoner> symbolicReasoner;
    
    // Learning from mistakes
    void learnFromError(const Board& board, const SolverMove& move, bool wasCorrect);
    
    // Cross-validation helpers
    std::vector<std::vector<std::pair<Board, Board>>> createKFolds(
        const std::vector<std::pair<Board, Board>>& data, int kFolds);
    
    double testModelOnFold(const std::vector<std::pair<Board, Board>>& testFold,
                          bool calculateMetrics = false);
    
    std::string generateDetailedReport(const CrossValidationResult& result);
    
    // Performance tracking
    int correctPredictions = 0;
    int totalPredictions = 0;
    
    // Cross-validation state
    std::vector<double> trainingHistory;
    std::vector<double> validationHistory;
    
    // Training mode control
    bool isTrainingMode = false;
};

#endif // SUDOKU_NEURO_SYMBOLIC_SOLVER_H
