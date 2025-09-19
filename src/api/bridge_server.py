# Enhanced Python REST API Bridge - connects web frontend to C++ backend
from flask import Flask, jsonify, request
from flask_cors import CORS
import subprocess
import json
import os

app = Flask(__name__)
CORS(app)  # Allow web frontend to call API

def call_cpp_api(command, params=""):
    """Call the C++ API and return parsed JSON response"""
    try:
        # Path to compiled C++ API executable - try multiple possible paths
        possible_paths = [
            "../../build/bin/sudoku_api",  # From src/api/ directory
            "./build/bin/sudoku_api",      # From project root
            "build/bin/sudoku_api"         # From project root (alternative)
        ]
        
        api_path = None
        for path in possible_paths:
            if os.path.exists(path):
                api_path = path
                break
        
        if api_path is None:
            return {"success": False, "message": "C++ API not found. Run 'make' first. Checked paths: " + str(possible_paths)}
        
        # Call C++ API with command and parameters
        if params:
            result = subprocess.run([api_path, command, params], 
                                  capture_output=True, text=True, check=True)
        else:
            result = subprocess.run([api_path, command], 
                                  capture_output=True, text=True, check=True)
        
        # Parse JSON response from C++
        return json.loads(result.stdout)
        
    except subprocess.CalledProcessError as e:
        return {"success": False, "message": f"C++ API error: {e}"}
    except json.JSONDecodeError as e:
        return {"success": False, "message": f"JSON parse error: {e}"}
    except Exception as e:
        return {"success": False, "message": f"Unexpected error: {e}"}

@app.route('/api/board', methods=['GET'])
def get_board():
    """Get current board state from C++ backend"""
    response = call_cpp_api("get_board")
    return jsonify(response)

@app.route('/api/move', methods=['POST'])
def make_move():
    """Make a move using C++ backend validation"""
    data = request.json
    row, col, value = data['row'], data['col'], data['value']
    
    # Format parameters for C++ API: "row,col,value"
    params = f"{row},{col},{value}"
    response = call_cpp_api("make_move", params)
    
    return jsonify(response)

@app.route('/api/puzzle', methods=['POST'])
def load_puzzle():
    """Load a new puzzle using C++ backend"""
    response = call_cpp_api("load_puzzle")
    return jsonify(response)

@app.route('/api/generate', methods=['POST'])
def generate_puzzle():
    """Generate a new puzzle with specified difficulty using C++ backend"""
    data = request.json
    difficulty = data.get('difficulty', 'medium')  # Default to medium
    
    response = call_cpp_api("generate_puzzle", difficulty)
    return jsonify(response)

@app.route('/api/clear', methods=['POST'])
def clear_board():
    """Clear the board using C++ backend"""
    response = call_cpp_api("clear_board")
    return jsonify(response)

@app.route('/api/status', methods=['GET'])
def get_status():
    """Get game status from C++ backend"""
    response = call_cpp_api("get_status")
    return jsonify(response)

@app.route('/api/validate', methods=['GET'])
def validate_board():
    """Validate current board using C++ logic"""
    response = call_cpp_api("validate")
    return jsonify(response)

@app.route('/api/ai/hint', methods=['POST'])
def get_ai_hint():
    """Get AI hint using the symbolic-informed neural solver"""
    data = request.json
    solver_type = data.get('solver', 'neuro_symbolic')  # Default to our new solver
    
    response = call_cpp_api("get_ai_move", solver_type)
    return jsonify(response)

@app.route('/api/ai/moves', methods=['POST'])
def get_ai_moves():
    """Get all possible AI moves with reasoning"""
    data = request.json
    solver_type = data.get('solver', 'neuro_symbolic')
    
    response = call_cpp_api("get_ai_moves", solver_type)
    return jsonify(response)

@app.route('/api/ai/solve', methods=['POST'])
def solve_puzzle():
    """Solve the puzzle using AI"""
    data = request.json
    solver_type = data.get('solver', 'neuro_symbolic')
    
    response = call_cpp_api("solve_puzzle", solver_type)
    return jsonify(response)

@app.route('/api/ai/stats', methods=['GET'])
def get_ai_stats():
    """Get AI training statistics"""
    response = call_cpp_api("training_stats")
    return jsonify(response)

@app.route('/api/ai/train', methods=['POST'])
def train_ai():
    """Train the AI on puzzle batch"""
    data = request.json
    num_puzzles = data.get('puzzles', 10)
    
    response = call_cpp_api("train_batch", str(num_puzzles))
    return jsonify(response)

@app.route('/health', methods=['GET'])
def health_check():
    """Health check endpoint"""
    return jsonify({"status": "healthy", "message": "Sudoku API Bridge is running"})

if __name__ == '__main__':
    print("🚀 Sudoku API Bridge Server starting...")
    print("📡 Connecting Web Frontend ←→ C++ Backend")
    print("🌐 Server running on http://localhost:5000")
    print("💡 Make sure to compile C++ API first: make api")
    print("🔗 Web UI: Open web/index.html in browser")
    print("─" * 50)
    app.run(debug=True, port=5000)
