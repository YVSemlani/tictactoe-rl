#include "environment.h"

Environment::Environment(int N, std::shared_ptr<RewardCallback> reward_fn) : reward_fn(reward_fn) {
    current_state.N = N;
    current_state.cells = std::vector<int>(N * N, 0);
}

BoardState Environment::reset() {
    current_state.cells.assign(current_state.N * current_state.N, 0);
    return current_state;
}

StepResult Environment::step(const Action& action) {
    // Simple update: set to 1 (player1)
    current_state.cells[action.index] = 1;
    // Call reward callback
    float reward = (*reward_fn)(current_state, action);
    bool done = false;
    StepResult result = {current_state, reward, done};
    return result;
} 