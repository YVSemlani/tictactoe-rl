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
    
    // Call reward callback
    float reward = (*reward_fn)(current_state, action);
    bool done = false;
    StepResult result = {current_state, reward, done};
    
    // Alternate to the next player
    current_player = -current_player;  // Switch between 1 and -1
    
    return result;
} 