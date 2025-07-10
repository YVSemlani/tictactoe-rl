# Python AI Integration ✅ WORKING

This document explains how to integrate your own Python-based AI strategies with the C++ tic-tac-toe engine.

## ✅ Current Status

- **Working**: Full Python AI integration with `uv` package management
- **Tested**: Both random and strategic Python models working correctly  
- **Features**: Error handling, virtual environment support, hot-reloading capability

## Quick Start

### 1. Install Dependencies

```bash
# Using uv (recommended)
uv add pybind11 numpy

# Or using pip
pip install pybind11 numpy
```

### 2. Build with Python Support

```bash
mkdir build
cd build
cmake ..
make
```

You should see: `✅ pybind11 found - Python AI support enabled`

### 3. Test the Integration

```bash
# Run the Python AI test
./test_python_ai

# Or use the interactive game with Python AI
./interactive_game
# Select "Custom Game Setup" -> choose Python AI types
```

## Working Example Output

```
✅ PythonAI 'PyRandom' initialized successfully
✅ PythonAI 'PyStrategy' initialized successfully

PyRandom (Python) is thinking...
Python Random Model: Player 1 choosing move 8

PyStrategy (Python) is thinking...  
Python Strategy Model: Player -1 taking center 4
Python Strategy Model: Player -1 blocking opponent with move 1
Python Strategy Model: Player -1 winning with move 2

🎉 WINNER: PyStrategy (O) 🎉
```

## Available Python AI Types

When creating AI players, you can use these types:

- **`PythonRandom`**: Random move selection using `models.random_model`
- **`PythonStrategy`**: Strategic play using `models.strategy_model`  
- **`PythonRandomDebug`**: Random with debug output
- **`PythonStrategyDebug`**: Strategy with debug output

## Creating Your Own Python Models

### Basic Interface

Your Python model must implement this function signature:

```python
def get_move(board_state, player, valid_moves):
    """
    Args:
        board_state: (N, N) numpy array with 0=empty, 1=player1, -1=player2
        player: Current player (1 or -1)
        valid_moves: (N*N,) boolean array of valid move positions
    
    Returns:
        action_index: Integer in range [0, N*N-1]
    """
    pass
```

### Example: Simple Random Model

```python
# models/my_random_model.py
import numpy as np
import random

def get_move(board_state, player, valid_moves):
    valid_indices = np.where(valid_moves)[0]
    return random.choice(valid_indices)
```

### Example: Neural Network Model

```python
# models/my_neural_model.py
import torch
import numpy as np

# Load your pre-trained model
model = torch.load('path/to/your/model.pth')
model.eval()

def get_move(board_state, player, valid_moves):
    # Convert to tensor
    state_tensor = torch.FloatTensor(board_state).unsqueeze(0)
    
    # Run inference
    with torch.no_grad():
        logits = model(state_tensor)
    
    # Mask invalid moves
    logits = logits.squeeze(0)
    logits[~valid_moves] = float('-inf')
    
    # Select action (argmax or sample)
    action = torch.argmax(logits).item()
    return action
```

### Using Your Model

```cpp
// In C++ code:
auto my_ai = session.create_ai_player("MyAI", "CustomPython");

// Or create directly:
auto my_ai = std::make_shared<PythonAI>("MyAI", "models.my_neural_model", "get_move");
```

## Advanced Features

### Class-Based Models

You can use class-based models for stateful AI:

```python
# models/stateful_model.py
class MyModel:
    def __init__(self):
        self.game_count = 0
        
    def get_move(self, board_state, player, valid_moves):
        # Your logic here
        return selected_move

# Global instance
_model = MyModel()

def get_move(board_state, player, valid_moves):
    return _model.get_move(board_state, player, valid_moves)
```

### Model Loading and Hot Reloading

For development, you can modify Python files and they'll be reloaded:

```python
# models/hot_reload_model.py
import importlib
import sys

def get_move(board_state, player, valid_moves):
    # Reload the module for development
    if 'models.my_actual_model' in sys.modules:
        importlib.reload(sys.modules['models.my_actual_model'])
    
    from models.my_actual_model import get_move as actual_get_move
    return actual_get_move(board_state, player, valid_moves)
```

### Error Handling

The C++ integration includes robust error handling:

- Python exceptions are caught and logged
- Invalid moves trigger fallback to random selection
- Missing modules/functions are handled gracefully
- Initialization failures fall back to C++ AI

### Performance Tips

1. **Minimize Python object creation**: Reuse numpy arrays when possible
2. **Use vectorized operations**: NumPy operations are faster than Python loops
3. **Cache expensive computations**: Store model weights/embeddings globally
4. **Batch processing**: For multiple games, consider batching board states

## Integration Architecture

```
C++ Game Engine
       ↓
   PythonAI Class
       ↓
   pybind11 Bridge
       ↓
   Python Interpreter
       ↓
   Your Python Model
```

The integration:
- Embeds Python interpreter in C++ process
- Converts C++ data structures to NumPy arrays
- Calls your Python functions
- Converts results back to C++ actions
- Handles errors and provides fallbacks

## Troubleshooting

### Build Issues

**Problem**: `pybind11 not found`
```bash
# Solution: Install pybind11
pip install pybind11

# Or install via conda
conda install pybind11
```

**Problem**: Python headers not found
```bash
# Solution: Install Python development headers
# Ubuntu/Debian:
sudo apt-get install python3-dev

# macOS:
# Usually included with Python installation
```

### Runtime Issues

**Problem**: `ModuleNotFoundError`
- Ensure your Python file is in the `models/` directory
- Check that `models/__init__.py` exists
- Verify the module path matches your file structure

**Problem**: `Function not found`
- Ensure your Python file has the correct function name
- Check function signature matches the expected interface

**Problem**: Python AI falls back to random
- Check console output for specific error messages
- Verify your function returns a valid integer
- Ensure NumPy arrays are handled correctly

## File Structure

```
tictactoe-rl/
├── models/
│   ├── __init__.py
│   ├── random_model.py      # Simple random strategy
│   ├── strategy_model.py    # Rule-based strategy
│   └── your_model.py        # Your custom model
├── include/
│   └── player.h             # Contains PythonAI class
├── src/
│   └── player.cpp           # PythonAI implementation
├── test_python_ai.cpp       # Integration test
└── CMakeLists.txt           # Build configuration
```

## Next Steps

1. **Start simple**: Begin with the provided random/strategy models
2. **Add your logic**: Modify existing models or create new ones
3. **Test extensively**: Use the test program and interactive game
4. **Scale up**: Integrate with ML frameworks for sophisticated models
5. **Optimize**: Profile and optimize for performance when needed

The Python integration provides a flexible foundation for experimenting with different AI strategies while keeping the performance-critical game engine in C++. 