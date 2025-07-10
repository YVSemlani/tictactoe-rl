#pragma once

#include <vector>
#include <memory>
#include <stdexcept>

struct BoardState {
    std::vector<int> cells;
    int N;
    bool operator==(const BoardState& other) const {
        return N == other.N && cells == other.cells;
    }
};

struct Action {
    int index;
};

struct StepResult {
    BoardState next_state;
    float reward;
    bool done;
};

class RewardCallback {
public:
    virtual float operator()(const BoardState& state, const Action& action) = 0;
    virtual ~RewardCallback() = default;
};

class DefaultReward : public RewardCallback {
public:
    float operator()(const BoardState& state, const Action& action) override {
        return 0.0f;
    }
};

class Environment {
public:
    Environment(int N, std::shared_ptr<RewardCallback> reward_fn);
    BoardState reset();
    StepResult step(const Action& action);
    std::vector<bool> get_action_mask() const;
    std::vector<float> get_flattened_state() const;
    
    // US2.2: One-Hot Encoding Option
    std::vector<float> get_one_hot_state() const;
    
private:
    BoardState current_state;
    std::shared_ptr<RewardCallback> reward_fn;
    int current_player;  // 1 for player1, -1 for player2
    
    // Terminal detection helper methods
    bool check_win(const BoardState& state, int player) const;
    bool check_horizontal_win(const BoardState& state, int player) const;
    bool check_vertical_win(const BoardState& state, int player) const;
    bool check_diagonal_win(const BoardState& state, int player) const;
    bool is_board_full(const BoardState& state) const;
}; 