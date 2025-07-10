#include "environment.h"

Environment::Environment(int N, std::shared_ptr<RewardCallback> reward_fn) : reward_fn(reward_fn) {
    current_state.N = N;
    current_state.cells = std::vector<int>(N * N, 0);
    current_player = 1;  // Start with player 1
}

BoardState Environment::reset() {
    current_state.cells.assign(current_state.N * current_state.N, 0);
    current_player = 1;  // Reset to player 1
    return current_state;
}

StepResult Environment::step(const Action& action) {
    // Validate action index bounds
    if (action.index < 0 || action.index >= current_state.N * current_state.N) {
        throw std::invalid_argument("Action index out of bounds");
    }
    
    // Validate that the target cell is empty
    if (current_state.cells[action.index] != 0) {
        throw std::invalid_argument("Action targets an occupied cell");
    }
    
    // Apply the action - set cell to current player
    current_state.cells[action.index] = current_player;
    
    // Check for terminal conditions
    bool done = false;
    if (check_win(current_state, current_player)) {
        done = true;  // Current player wins
    } else if (check_win(current_state, -current_player)) {
        done = true;  // Other player wins  
    } else if (is_board_full(current_state)) {
        done = true;  // Draw - board is full with no winner
    }
    
    // Call reward callback
    float reward = (*reward_fn)(current_state, action);
    StepResult result = {current_state, reward, done};
    
    // Alternate to the next player
    current_player = -current_player;  // Switch between 1 and -1
    
    return result;
}

// Terminal detection helper methods
bool Environment::check_win(const BoardState& state, int player) const {
    return check_horizontal_win(state, player) || 
           check_vertical_win(state, player) || 
           check_diagonal_win(state, player);
}

bool Environment::check_horizontal_win(const BoardState& state, int player) const {
    // Check each row for N consecutive player marks
    for (int row = 0; row < state.N; ++row) {
        bool win = true;
        for (int col = 0; col < state.N; ++col) {
            if (state.cells[row * state.N + col] != player) {
                win = false;
                break;
            }
        }
        if (win) return true;
    }
    return false;
}

bool Environment::check_vertical_win(const BoardState& state, int player) const {
    // Check each column for N consecutive player marks
    for (int col = 0; col < state.N; ++col) {
        bool win = true;
        for (int row = 0; row < state.N; ++row) {
            if (state.cells[row * state.N + col] != player) {
                win = false;
                break;
            }
        }
        if (win) return true;
    }
    return false;
}

bool Environment::check_diagonal_win(const BoardState& state, int player) const {
    // Check main diagonal (top-left to bottom-right)
    bool main_diagonal = true;
    for (int i = 0; i < state.N; ++i) {
        if (state.cells[i * state.N + i] != player) {
            main_diagonal = false;
            break;
        }
    }
    if (main_diagonal) return true;
    
    // Check anti-diagonal (top-right to bottom-left)
    bool anti_diagonal = true;
    for (int i = 0; i < state.N; ++i) {
        if (state.cells[i * state.N + (state.N - 1 - i)] != player) {
            anti_diagonal = false;
            break;
        }
    }
    return anti_diagonal;
}

bool Environment::is_board_full(const BoardState& state) const {
    // Check if all cells are occupied (no 0s remaining)
    for (int cell : state.cells) {
        if (cell == 0) {
            return false;
        }
    }
    return true;
}

std::vector<bool> Environment::get_action_mask() const {
    std::vector<bool> mask(current_state.N * current_state.N);
    for (int i = 0; i < current_state.N * current_state.N; ++i) {
        mask[i] = (current_state.cells[i] == 0);  // true for empty cells, false for occupied
    }
    return mask;
}

std::vector<float> Environment::get_flattened_state() const {
    std::vector<float> flattened_state(current_state.N * current_state.N);
    for (int i = 0; i < current_state.N * current_state.N; ++i) {
        flattened_state[i] = static_cast<float>(current_state.cells[i]);
    }
    return flattened_state;
} 

// US2.2: One-Hot Encoding Option
std::vector<float> Environment::get_one_hot_state() const {
    const int N = current_state.N;
    const int board_size = N * N;
    
    // Create one-hot tensor with shape [2, N, N] flattened to 2*N*N
    std::vector<float> one_hot(2 * board_size, 0.0f);
    
    for (int i = 0; i < board_size; ++i) {
        int cell_value = current_state.cells[i];
        
        if (cell_value == 1) {
            // Player 1 channel (first N*N elements)
            one_hot[i] = 1.0f;
        } else if (cell_value == -1) {
            // Player 2 channel (second N*N elements)
            one_hot[board_size + i] = 1.0f;
        }
        // Empty cells (cell_value == 0) remain 0.0f in both channels
    }
    
    return one_hot;
} 