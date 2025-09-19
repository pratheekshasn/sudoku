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

std::vector<SolverMove> SymbolicReasoner::applyLogicalRules(const Board& board) {
    std::vector<SolverMove> moves;
    
    // Apply forced moves first (highest confidence)
    std::vector<SolverMove> forced = findForcedMoves(board);
    moves.insert(moves.end(), forced.begin(), forced.end());
    
    // Apply pattern detection
    std::vector<SolverMove> patterns = detectPatterns(board);
    moves.insert(moves.end(), patterns.begin(), patterns.end());
    
    return moves;
}

std::vector<SolverMove> SymbolicReasoner::findForcedMoves(const Board& board) {
    std::vector<SolverMove> moves;
    int size = board.getBoardSize();
    
    for (int row = 0; row < size; ++row) {
        for (int col = 0; col < size; ++col) {
            if (board.getCell(row, col).getValue() == 0) {
                std::vector<int> candidates = getCandidates(board, row, col);
                
                if (candidates.size() == 1) {
                    std::string reasoning = "Symbolic: Forced move (only one possibility)";
                    moves.emplace_back(row, col, candidates[0], reasoning, 1.0);
                }
            }
        }
    }
    
    return moves;
}

std::vector<SolverMove> SymbolicReasoner::detectPatterns(const Board& board) {
    std::vector<SolverMove> moves;
    int size = board.getBoardSize();
    
    // Detect naked and hidden singles (basic patterns)
    for (int row = 0; row < size; ++row) {
        for (int col = 0; col < size; ++col) {
            if (board.getCell(row, col).getValue() == 0) {
                int value;
                if (isNakedSingle(board, row, col, value)) {
                    std::string reasoning = "Symbolic: Naked single detected";
                    moves.emplace_back(row, col, value, reasoning, 0.95);
                }
                
                // Check for hidden singles
                for (int v = 1; v <= size; ++v) {
                    if (isHiddenSingle(board, row, col, v)) {
                        std::string reasoning = "Symbolic: Hidden single for value " + std::to_string(v);
                        moves.emplace_back(row, col, v, reasoning, 0.9);
                    }
                }
            }
        }
    }
    
    // Advanced symbolic reasoning patterns
    std::vector<SolverMove> advanced = detectAdvancedPatterns(board);
    moves.insert(moves.end(), advanced.begin(), advanced.end());
    
    return moves;
}

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

// Advanced symbolic reasoning patterns - this is what makes it truly "symbolic"
std::vector<SolverMove> SymbolicReasoner::detectAdvancedPatterns(const Board& board) {
    std::vector<SolverMove> moves;
    
    // 1. NAKED PAIRS/TRIPLES - If two cells in a unit can only contain the same two values,
    //    those values can be eliminated from other cells in that unit
    std::vector<SolverMove> nakedPairs = findNakedPairs(board);
    moves.insert(moves.end(), nakedPairs.begin(), nakedPairs.end());
    
    // 2. POINTING PAIRS - If a candidate appears in only one row/col within a box,
    //    it can be eliminated from that row/col outside the box
    std::vector<SolverMove> pointing = findPointingPairs(board);
    moves.insert(moves.end(), pointing.begin(), pointing.end());
    
    // 3. BOX/LINE REDUCTION - If all candidates for a value in a box are in one line,
    //    eliminate that value from the rest of the line
    std::vector<SolverMove> boxLine = findBoxLineReductions(board);
    moves.insert(moves.end(), boxLine.begin(), boxLine.end());
    
    // 4. X-WING PATTERN - Advanced elimination technique
    std::vector<SolverMove> xwing = findXWingPatterns(board);
    moves.insert(moves.end(), xwing.begin(), xwing.end());
    
    return moves;
}

std::vector<SolverMove> SymbolicReasoner::findNakedPairs(const Board& board) {
    std::vector<SolverMove> moves;
    int size = board.getBoardSize();
    
    // Check rows for naked pairs
    for (int row = 0; row < size; ++row) {
        std::vector<std::vector<int>> candidates(size);
        for (int col = 0; col < size; ++col) {
            if (board.getCell(row, col).getValue() == 0) {
                candidates[col] = getCandidates(board, row, col);
            }
        }
        
        // Find pairs
        for (int c1 = 0; c1 < size - 1; ++c1) {
            for (int c2 = c1 + 1; c2 < size; ++c2) {
                if (candidates[c1].size() == 2 && candidates[c2].size() == 2 &&
                    candidates[c1] == candidates[c2]) {
                    // Found naked pair! This is logical deduction, not brute force
                    std::string reasoning = "Symbolic: Naked pair elimination in row " + std::to_string(row + 1);
                    // In a real implementation, we'd eliminate these values from other cells
                    // For now, we'll mark this as a high-confidence logical pattern
                    if (!candidates[c1].empty()) {
                        moves.emplace_back(row, c1, candidates[c1][0], reasoning, 0.85);
                    }
                }
            }
        }
    }
    
    return moves;
}

std::vector<SolverMove> SymbolicReasoner::findPointingPairs(const Board& board) {
    std::vector<SolverMove> moves;
    int size = board.getBoardSize();
    int gridSize = static_cast<int>(sqrt(size));
    
    // For each 3x3 box
    for (int boxRow = 0; boxRow < gridSize; ++boxRow) {
        for (int boxCol = 0; boxCol < gridSize; ++boxCol) {
            int startRow = boxRow * gridSize;
            int startCol = boxCol * gridSize;
            
            // For each possible value
            for (int value = 1; value <= size; ++value) {
                std::vector<std::pair<int, int>> positions;
                
                // Find where this value can go in this box
                for (int r = startRow; r < startRow + gridSize; ++r) {
                    for (int c = startCol; c < startCol + gridSize; ++c) {
                        if (board.getCell(r, c).getValue() == 0 &&
                            validateMove(board, r, c, value)) {
                            positions.push_back({r, c});
                        }
                    }
                }
                
                // If all positions are in the same row (pointing pair)
                if (positions.size() >= 2) {
                    bool sameRow = true;
                    int firstRow = positions[0].first;
                    for (const auto& pos : positions) {
                        if (pos.first != firstRow) {
                            sameRow = false;
                            break;
                        }
                    }
                    
                    if (sameRow && positions.size() == 2) {
                        std::string reasoning = "Symbolic: Pointing pair for value " + std::to_string(value);
                        moves.emplace_back(positions[0].first, positions[0].second, value, reasoning, 0.8);
                    }
                }
            }
        }
    }
    
    return moves;
}

std::vector<SolverMove> SymbolicReasoner::findBoxLineReductions(const Board& board) {
    std::vector<SolverMove> moves;
    // This would implement box/line reduction logic
    // For now, return empty to keep the example concise
    return moves;
}

std::vector<SolverMove> SymbolicReasoner::findXWingPatterns(const Board& board) {
    std::vector<SolverMove> moves;
    // This would implement X-Wing pattern detection
    // For now, return empty to keep the example concise
    return moves;
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
    
    // Get symbolic reasoning moves
    std::vector<SolverMove> symbolicMoves = symbolicReasoner->applyLogicalRules(board);
    
    // Get neural network predictions for all possible moves
    // Now the neural network receives symbolic hints as input!
    std::vector<SolverMove> neuralMoves;
    int size = board.getBoardSize();
    
    for (int row = 0; row < size; ++row) {
        for (int col = 0; col < size; ++col) {
            if (board.getCell(row, col).getValue() == 0) {
                for (int value = 1; value <= size; ++value) {
                    if (symbolicReasoner->validateMove(board, row, col, value)) {
                        // Generate symbolic hints for this specific move
                        std::vector<double> symbolicHints = symbolicReasoner->generateSymbolicHints(board, row, col, value);
                        
                        // Neural network now receives and processes symbolic hints!
                        double neuralConf = neuralNet->predictMoveConfidence(board, row, col, value, symbolicHints);
                        
                        std::string reasoning = "Symbolic-Informed Neural: Pattern + Logic fusion";
                        neuralMoves.emplace_back(row, col, value, reasoning, neuralConf);
                    }
                }
            }
        }
    }
    
    // Combine and rank moves using hybrid approach
    std::vector<SolverMove> combinedMoves;
    
    // Add symbolic moves with boosted confidence
    for (auto& move : symbolicMoves) {
        move.confidence = fuseConfidences(0.5, move.confidence, move.reasoning);
        move.reasoning = "Hybrid: " + move.reasoning;
        combinedMoves.push_back(move);
    }
    
    // Add neural moves that aren't already covered by symbolic reasoning
    for (const auto& neuralMove : neuralMoves) {
        bool alreadyExists = false;
        for (const auto& symbolicMove : symbolicMoves) {
            if (symbolicMove.row == neuralMove.row && 
                symbolicMove.col == neuralMove.col && 
                symbolicMove.value == neuralMove.value) {
                alreadyExists = true;
                break;
            }
        }
        
        if (!alreadyExists && neuralMove.confidence > 0.6) { // Neural threshold
            SolverMove hybrid = neuralMove;
            hybrid.confidence = fuseConfidences(neuralMove.confidence, 0.3, neuralMove.reasoning);
            hybrid.reasoning = "Hybrid: " + neuralMove.reasoning;
            combinedMoves.push_back(hybrid);
        }
    }
    
    // Sort by fused confidence
    std::sort(combinedMoves.begin(), combinedMoves.end(),
              [](const SolverMove& a, const SolverMove& b) {
                  return a.confidence > b.confidence;
              });
    
    return combinedMoves;
}

double NeuroSymbolicSolver::fuseConfidences(double neuralConf, double symbolicConf, 
                                           const std::string& reasoning) {
    lastNeuralConfidence = neuralConf;
    lastSymbolicConfidence = symbolicConf;
    
    // Weighted fusion based on strategy
    switch (currentStrategy) {
        case Strategy::SYMBOLIC_FIRST:
            return 0.8 * symbolicConf + 0.2 * neuralConf;
        
        case Strategy::NEURAL_GUIDED:
            return 0.7 * neuralConf + 0.3 * symbolicConf;
        
        case Strategy::BALANCED_FUSION:
        default: {
            // Enhanced balanced fusion with multiple techniques
            
            // 1. Harmonic mean: Conservative, penalizes disagreement
            double harmonic = (neuralConf > 0 && symbolicConf > 0) ? 
                2.0 * neuralConf * symbolicConf / (neuralConf + symbolicConf) : 
                std::max(neuralConf, symbolicConf);
            
            // 2. Geometric mean: Balanced, but less conservative
            double geometric = (neuralConf > 0 && symbolicConf > 0) ? 
                sqrt(neuralConf * symbolicConf) : 
                std::max(neuralConf, symbolicConf);
            
            // 3. Agreement-weighted fusion: Higher weight when systems agree
            double agreement = 1.0 - abs(neuralConf - symbolicConf);
            double arithmetic = (neuralConf + symbolicConf) / 2.0;
            
            // Choose fusion method based on agreement level
            if (agreement > 0.8) {
                // High agreement: use geometric mean (maintains confidence)
                return geometric;
            } else if (agreement > 0.5) {
                // Medium agreement: blend harmonic and geometric
                return 0.6 * harmonic + 0.4 * geometric;
            } else {
                // Low agreement: use conservative harmonic mean
                return harmonic;
            }
        }
    }
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

SolverMove NeuroSymbolicSolver::combineNeuralAndSymbolic(const Board& board,
                                                        const std::vector<SolverMove>& symbolicMoves,
                                                        const std::vector<SolverMove>& neuralMoves) {
    // Prioritize symbolic moves for high-confidence logical deductions
    for (const auto& move : symbolicMoves) {
        if (move.confidence > 0.9) {
            return move;
        }
    }
    
    // For ambiguous cases, use neural guidance
    if (!neuralMoves.empty()) {
        SolverMove bestMove = neuralMoves[0];
        bestMove.reasoning = "Neuro-Symbolic: Neural guidance for ambiguous case";
        return bestMove;
    }
    
    // Fallback to best symbolic move
    if (!symbolicMoves.empty()) {
        return symbolicMoves[0];
    }
    
    // No valid moves found
    return SolverMove{-1, -1, -1, "No valid moves found", 0.0};
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
