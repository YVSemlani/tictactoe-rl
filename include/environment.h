#pragma once

#include <vector>
#include <memory>

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
private:
    BoardState current_state;
    std::shared_ptr<RewardCallback> reward_fn;
}; 