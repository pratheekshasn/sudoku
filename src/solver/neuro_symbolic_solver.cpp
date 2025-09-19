/*
Neuro-Symbolic Solver Implementation
Demonstrates hybrid AI combining neural networks with symbolic reasoning
*/

#include "neuro_symbolic_solver.h"
#include <algorithm>
#include <cmath>
#include <random>
#include <numeric>

// ============================================================================
// SudokuNeuralNetwork Implementation
// ============================================================================

SudokuNeuralNetwork::SudokuNeuralNetwork(int boardSize) 
    : boardSize(boardSize), rng(std::random_device{}()) {
    calculateNetworkSize();
    initializeNetwork();
}

void SudokuNeuralNetwork::calculateNetworkSize() {
    // Adaptive input size calculation
    // Board state features: boardSize * boardSize
    // Position features: 5 (row, col, value, box_row, box_col)  
    // Neighborhood features: 4 (density metrics)
    // Symbolic hints: 8 (logical reasoning features)
    inputSize = (boardSize * boardSize) + 5 + 4 + 8;
    
    // Hidden layer size: empirically good ratio for Sudoku
    // For 4x4: 33 inputs -> 20 hidden
    // For 9x9: 98 inputs -> 50 hidden  
    // For 16x16: 269 inputs -> 135 hidden
    hiddenSize = std::max(10, inputSize / 2);
}

void SudokuNeuralNetwork::initializeNetwork() {
    // Initialize hidden layer
    hiddenLayer.clear();
    hiddenLayer.resize(hiddenSize);
    
    for (auto& neuron : hiddenLayer) {
        neuron.weights.resize(inputSize);
        // Xavier initialization
        double range = sqrt(6.0 / (inputSize + hiddenSize));
        std::uniform_real_distribution<double> dist(-range, range);
        
        for (double& weight : neuron.weights) {
            weight = dist(rng);
        }
        neuron.bias = dist(rng);
    }
    
    // Initialize output layer
    outputLayer.clear();
    outputLayer.resize(1);
    outputLayer[0].weights.resize(hiddenSize);
    double range = sqrt(6.0 / (hiddenSize + 1));
    std::uniform_real_distribution<double> dist(-range, range);
    
    for (double& weight : outputLayer[0].weights) {
        weight = dist(rng);
    }
    outputLayer[0].bias = dist(rng);
}

void SudokuNeuralNetwork::adaptToBoardSize(int newSize) {
    if (newSize != boardSize) {
        boardSize = newSize;
        calculateNetworkSize();
        initializeNetwork();
    }
}

std::vector<double> SudokuNeuralNetwork::extractFeatures(const Board& board, int row, int col, int value, 
                                                      const std::vector<double>& symbolicHints) {
    std::vector<double> features;
    features.reserve(inputSize);
    
    int size = board.getBoardSize();
    
    // Board state features (size*size values: 0-max_value, normalized to 0-1)
    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            double cellValue = board.getCell(r, c).getValue() / (double)size;
            features.push_back(cellValue);
        }
    }
    
    // Position and value features (5 values) - normalized for any board size
    // Handle edge case: for size=1, avoid division by zero
    double rowNorm = (size > 1) ? row / (double)(size - 1) : 0.5;
    double colNorm = (size > 1) ? col / (double)(size - 1) : 0.5;
    double boxRowNorm = (sqrt(size) > 1) ? (row / static_cast<int>(sqrt(size))) / (sqrt(size) - 1) : 0.5;
    double boxColNorm = (sqrt(size) > 1) ? (col / static_cast<int>(sqrt(size))) / (sqrt(size) - 1) : 0.5;
    
    features.push_back(rowNorm);                            // Row position (0-1)
    features.push_back(colNorm);                            // Column position (0-1)
    features.push_back(value / (double)size);               // Candidate value (0-1)
    features.push_back(boxRowNorm);                         // Box row (0-1)
    features.push_back(boxColNorm);                         // Box col (0-1)
    
    // Local neighborhood density (4 values)
    int filledNeighbors = 0;
    int totalNeighbors = 0;
    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            int nr = row + dr, nc = col + dc;
            if (nr >= 0 && nr < size && nc >= 0 && nc < size && !(dr == 0 && dc == 0)) {
                totalNeighbors++;
                if (board.getCell(nr, nc).getValue() != 0) {
                    filledNeighbors++;
                }
            }
        }
    }
    features.push_back(totalNeighbors > 0 ? filledNeighbors / (double)totalNeighbors : 0.0);
    features.push_back(totalNeighbors / 8.0); // Max 8 neighbors
    features.push_back(0.5); // Reserved for future features
    features.push_back(0.5); // Reserved for future features
    
    // Symbolic hints (8 values) - these are the key new features!
    if (symbolicHints.size() >= 8) {
        for (int i = 0; i < 8; ++i) {
            features.push_back(symbolicHints[i]);
        }
    } else {
        // Default symbolic hints if not provided
        features.push_back(0.0); // isForced: 1.0 if this is a forced move
        features.push_back(0.0); // isNakedSingle: 1.0 if naked single detected
        features.push_back(0.0); // isHiddenSingle: 1.0 if hidden single detected
        features.push_back(0.0); // constraintViolations: number of violations (0-1)
        features.push_back(0.0); // candidateCount: normalized number of candidates
        features.push_back(0.0); // logicalConfidence: confidence from symbolic reasoning
        features.push_back(0.0); // patternStrength: strength of detected patterns
        features.push_back(0.0); // eliminationPower: how many eliminations this move enables
    }
    
    return features;
}

double SudokuNeuralNetwork::forward(const std::vector<double>& features) {
    // Forward pass through hidden layer
    for (size_t i = 0; i < hiddenLayer.size(); ++i) {
        double sum = hiddenLayer[i].bias;
        for (size_t j = 0; j < features.size(); ++j) {
            sum += features[j] * hiddenLayer[i].weights[j];
        }
        // ReLU activation
        hiddenLayer[i].output = std::max(0.0, sum);
    }
    
    // Forward pass through output layer
    double sum = outputLayer[0].bias;
    for (size_t i = 0; i < hiddenLayer.size(); ++i) {
        sum += hiddenLayer[i].output * outputLayer[0].weights[i];
    }
    
    // Sigmoid activation for confidence (0-1)
    return 1.0 / (1.0 + exp(-sum));
}

double SudokuNeuralNetwork::predictMoveConfidence(const Board& board, int row, int col, int value,
                                                 const std::vector<double>& symbolicHints) {
    std::vector<double> features = extractFeatures(board, row, col, value, symbolicHints);
    return forward(features);
}

void SudokuNeuralNetwork::updateWeights(const Board& board, int row, int col, int value, bool wasCorrect,
                                        const std::vector<double>& symbolicHints) {
    // Simplified backpropagation for demonstration
    std::vector<double> features = extractFeatures(board, row, col, value, symbolicHints);
    double predicted = forward(features);
    double target = wasCorrect ? 0.9 : 0.1;
    double error = target - predicted;
    
    // Update output layer
    for (size_t i = 0; i < hiddenLayer.size(); ++i) {
        outputLayer[0].weights[i] += learningRate * error * hiddenLayer[i].output;
    }
    outputLayer[0].bias += learningRate * error;
    
    // Update hidden layer (simplified)
    for (size_t i = 0; i < hiddenLayer.size(); ++i) {
        double hiddenError = error * outputLayer[0].weights[i] * (hiddenLayer[i].output > 0 ? 1.0 : 0.0);
        for (size_t j = 0; j < features.size(); ++j) {
            hiddenLayer[i].weights[j] += learningRate * hiddenError * features[j];
        }
        hiddenLayer[i].bias += learningRate * hiddenError;
    }
}

double SudokuNeuralNetwork::assessDifficulty(const Board& board) {
    int filledCells = 0;
    int size = board.getBoardSize();
    
    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            if (board.getCell(r, c).getValue() != 0) {
                filledCells++;
            }
        }
    }
    
    double fillRatio = filledCells / (double)(size * size);
    return 1.0 - fillRatio; // Higher difficulty = fewer filled cells
}

// ============================================================================
// SymbolicReasoner Implementation  
// ============================================================================

SymbolicReasoner::SymbolicReasoner() {}



bool SymbolicReasoner::isNakedSingle(const Board& board, int row, int col, int& value) {
    std::vector<int> candidates = getCandidates(board, row, col);
    if (candidates.size() == 1) {
        value = candidates[0];
        return true;
    }
    return false;
}

bool SymbolicReasoner::isHiddenSingle(const Board& board, int row, int col, int value) {
    if (!validateMove(board, row, col, value)) {
        return false;
    }
    
    int size = board.getBoardSize();
    
    // Check if this value can appear elsewhere in the row
    for (int c = 0; c < size; ++c) {
        if (c != col && board.getCell(row, c).getValue() == 0) {
            if (validateMove(board, row, c, value)) {
                return false; // Value can appear elsewhere
            }
        }
    }
    
    return true; // Hidden single found
}

std::vector<int> SymbolicReasoner::getCandidates(const Board& board, int row, int col) {
    std::vector<int> candidates;
    int size = board.getBoardSize();
    
    for (int value = 1; value <= size; ++value) {
        if (validateMove(board, row, col, value)) {
            candidates.push_back(value);
        }
    }
    
    return candidates;
}

bool SymbolicReasoner::validateMove(const Board& board, int row, int col, int value) {
    return !violatesConstraints(board, row, col, value);
}

bool SymbolicReasoner::violatesConstraints(const Board& board, int row, int col, int value) {
    int size = board.getBoardSize();
    int gridSize = static_cast<int>(sqrt(size));
    
    // Check row constraint
    for (int c = 0; c < size; ++c) {
        if (c != col && board.getCell(row, c).getValue() == value) {
            return true;
        }
    }
    
    // Check column constraint
    for (int r = 0; r < size; ++r) {
        if (r != row && board.getCell(r, col).getValue() == value) {
            return true;
        }
    }
    
    // Check 3x3 box constraint
    int boxStartRow = (row / gridSize) * gridSize;
    int boxStartCol = (col / gridSize) * gridSize;
    
    for (int r = boxStartRow; r < boxStartRow + gridSize; ++r) {
        for (int c = boxStartCol; c < boxStartCol + gridSize; ++c) {
            if ((r != row || c != col) && board.getCell(r, c).getValue() == value) {
                return true;
            }
        }
    }
    
    return false;
}



std::vector<double> SymbolicReasoner::generateSymbolicHints(const Board& board, int row, int col, int value) {
    std::vector<double> hints(8, 0.0);
    
    // Hint 0: isForced - Is this a forced move (only candidate)?
    std::vector<int> candidates = getCandidates(board, row, col);
    hints[0] = (candidates.size() == 1 && candidates[0] == value) ? 1.0 : 0.0;
    
    // Hint 1: isNakedSingle - Is this a naked single?
    int nakedValue;
    hints[1] = isNakedSingle(board, row, col, nakedValue) && nakedValue == value ? 1.0 : 0.0;
    
    // Hint 2: isHiddenSingle - Is this a hidden single?
    hints[2] = isHiddenSingle(board, row, col, value) ? 1.0 : 0.0;
    
    // Hint 3: constraintViolations - How many constraints would this violate? (normalized)
    double violations = violatesConstraints(board, row, col, value) ? 1.0 : 0.0;
    hints[3] = violations;
    
    // Hint 4: candidateCount - Number of possible candidates for this cell (normalized)
    int size = board.getBoardSize();
    hints[4] = candidates.size() / (double)size;
    
    // Hint 5: logicalConfidence - Overall confidence from logical rules
    double logicalConf = 0.0;
    if (hints[0] > 0.5) logicalConf = 1.0;      // Forced move
    else if (hints[1] > 0.5) logicalConf = 0.95; // Naked single  
    else if (hints[2] > 0.5) logicalConf = 0.9;  // Hidden single
    else if (hints[3] == 0.0) logicalConf = 0.7; // Valid move
    else logicalConf = 0.1; // Invalid move
    hints[5] = logicalConf;
    
    // Hint 6: patternStrength - Strength of detected patterns
    double patternStrength = 0.0;
    // Check for advanced patterns at this position
    if (hints[1] > 0.5 || hints[2] > 0.5) {
        patternStrength = 0.9; // Strong single patterns
    } else {
        // Check for weaker patterns like pairs, etc.
        // For now, base on candidate count - fewer candidates = stronger pattern
        patternStrength = std::max(0.0, 1.0 - (candidates.size() / (double)size));
    }
    hints[6] = patternStrength;
    
    // Hint 7: eliminationPower - How many eliminations would this move enable?
    double eliminationPower = 0.0;
    if (hints[3] == 0.0) { // Only if move is valid
        // Count empty cells in same row, column, and box that would be affected
        int affectedCells = 0;
        int totalEmpty = 0;
        
        // Check row
        for (int c = 0; c < size; ++c) {
            if (board.getCell(row, c).getValue() == 0) {
                totalEmpty++;
                std::vector<int> cellCandidates = getCandidates(board, row, c);
                if (std::find(cellCandidates.begin(), cellCandidates.end(), value) != cellCandidates.end()) {
                    affectedCells++;
                }
            }
        }
        
        // Check column  
        for (int r = 0; r < size; ++r) {
            if (board.getCell(r, col).getValue() == 0) {
                if (r != row) totalEmpty++; // Don't double count current cell
                std::vector<int> cellCandidates = getCandidates(board, r, col);
                if (std::find(cellCandidates.begin(), cellCandidates.end(), value) != cellCandidates.end()) {
                    affectedCells++;
                }
            }
        }
        
        // Check box
        int gridSize = static_cast<int>(sqrt(size));
        int boxStartRow = (row / gridSize) * gridSize;
        int boxStartCol = (col / gridSize) * gridSize;
        
        for (int r = boxStartRow; r < boxStartRow + gridSize; ++r) {
            for (int c = boxStartCol; c < boxStartCol + gridSize; ++c) {
                if (board.getCell(r, c).getValue() == 0 && !(r == row && c == col)) {
                    std::vector<int> cellCandidates = getCandidates(board, r, c);
                    if (std::find(cellCandidates.begin(), cellCandidates.end(), value) != cellCandidates.end()) {
                        affectedCells++;
                    }
                }
            }
        }
        
        eliminationPower = totalEmpty > 0 ? affectedCells / (double)totalEmpty : 0.0;
    }
    hints[7] = eliminationPower;
    
    return hints;
}

// ============================================================================
// NeuroSymbolicSolver Implementation
// ============================================================================

NeuroSymbolicSolver::NeuroSymbolicSolver(int boardSize) 
    : neuralNet(std::make_unique<SudokuNeuralNetwork>(boardSize))
    , symbolicReasoner(std::make_unique<SymbolicReasoner>()) {
}

bool NeuroSymbolicSolver::solve(Board& board) {
    bool progress = true;
    int iterations = 0;
    const int maxIterations = 1000;
    
    while (progress && !isBoardComplete(board) && iterations < maxIterations) {
        progress = false;
        SolverMove move{-1, -1, -1, "", 0.0}; // Initialize with default values
        
        if (getNextMove(board, move)) {
            board.getCell(move.row, move.col).setValue(move.value);
            progress = true;
            movesCount++;
        }
        
        iterations++;
    }
    
    return isBoardComplete(board);
}

bool NeuroSymbolicSolver::canSolve(const Board& board) const {
    return board.isValid();
}

bool NeuroSymbolicSolver::getNextMove(const Board& board, SolverMove& move) {
    std::vector<SolverMove> moves = getAllPossibleMoves(board);
    if (!moves.empty()) {
        move = moves[0]; // Return highest confidence move
        return true;
    }
    return false;
}

std::vector<SolverMove> NeuroSymbolicSolver::getAllPossibleMoves(const Board& board) {
    // Auto-adapt to board size if needed
    int currentBoardSize = board.getBoardSize();
    neuralNet->adaptToBoardSize(currentBoardSize);
    
    // Generate all possible moves using symbolic-informed neural network
    std::vector<SolverMove> moves;
    int size = board.getBoardSize();
    
    for (int row = 0; row < size; ++row) {
        for (int col = 0; col < size; ++col) {
            if (board.getCell(row, col).getValue() == 0) {
                for (int value = 1; value <= size; ++value) {
                    if (symbolicReasoner->validateMove(board, row, col, value)) {
                        // Generate symbolic hints for this specific move
                        std::vector<double> symbolicHints = symbolicReasoner->generateSymbolicHints(board, row, col, value);
                        
                        // Neural network receives both patterns AND symbolic reasoning
                        double confidence = neuralNet->predictMoveConfidence(board, row, col, value, symbolicHints);
                        
                        // Generate reasoning based on symbolic hints
                        std::string reasoning = "Symbolic-Informed Neural: ";
                        if (symbolicHints[0] > 0.5) reasoning += "Forced move (only one possibility)";
                        else if (symbolicHints[1] > 0.5) reasoning += "Naked single detected";
                        else if (symbolicHints[2] > 0.5) reasoning += "Hidden single detected";
                        else if (symbolicHints[3] > 0.5) reasoning += "Invalid move";
                        else reasoning += "Pattern + Logic fusion";
                        
                        moves.emplace_back(row, col, value, reasoning, confidence);
                    }
                }
            }
        }
    }
    
    // Sort by confidence (highest first)
    std::sort(moves.begin(), moves.end(),
              [](const SolverMove& a, const SolverMove& b) {
                  return a.confidence > b.confidence;
              });
    
    return moves;
}



void NeuroSymbolicSolver::trainOnSolution(const Board& originalBoard, const Board& solvedBoard) {
    // Extract training data from the solution path
    // Now training includes symbolic hints for better learning
    int size = originalBoard.getBoardSize();
    
    for (int row = 0; row < size; ++row) {
        for (int col = 0; col < size; ++col) {
            if (originalBoard.getCell(row, col).getValue() == 0) {
                int correctValue = solvedBoard.getCell(row, col).getValue();
                
                // Generate symbolic hints for the correct move
                std::vector<double> correctHints = symbolicReasoner->generateSymbolicHints(originalBoard, row, col, correctValue);
                
                // Train neural network: correct value should have high confidence
                // The neural network now learns from both patterns AND logical reasoning!
                neuralNet->updateWeights(originalBoard, row, col, correctValue, true, correctHints);
                
                // Train on wrong values (they should have low confidence)
                for (int wrongValue = 1; wrongValue <= size; ++wrongValue) {
                    if (wrongValue != correctValue && 
                        symbolicReasoner->validateMove(originalBoard, row, col, wrongValue)) {
                        
                        // Generate symbolic hints for wrong moves too
                        std::vector<double> wrongHints = symbolicReasoner->generateSymbolicHints(originalBoard, row, col, wrongValue);
                        neuralNet->updateWeights(originalBoard, row, col, wrongValue, false, wrongHints);
                    }
                }
            }
        }
    }
}

void NeuroSymbolicSolver::adaptToBoardSize(int newSize) {
    neuralNet->adaptToBoardSize(newSize);
}



void NeuroSymbolicSolver::learnFromError(const Board& board, const SolverMove& move, bool wasCorrect) {
    // Generate symbolic hints for learning from errors
    std::vector<double> hints = symbolicReasoner->generateSymbolicHints(board, move.row, move.col, move.value);
    neuralNet->updateWeights(board, move.row, move.col, move.value, wasCorrect, hints);
    
    if (wasCorrect) {
        correctPredictions++;
    }
    totalPredictions++;
}
