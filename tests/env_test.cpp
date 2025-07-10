#include <iostream>
#include "../include/environment.h"
#include <memory>

int main() {
    auto reward_fn = std::make_shared<DefaultReward>();
    Environment env(3, reward_fn);
    BoardState state = env.reset();
    std::cout << "After reset, cell 0: " << state.cells[0] << std::endl;
    Action act{0};
    StepResult res = env.step(act);
    std::cout << "After step, cell 0: " << res.next_state.cells[0] << std::endl;
    std::cout << "Reward: " << res.reward << std::endl;
    std::cout << "Done: " << res.done << std::endl;
    // Simple check
    if (res.next_state.cells[0] == 1 && res.reward == 0 && !res.done) {
        std::cout << "Test passed" << std::endl;
    } else {
        std::cout << "Test failed" << std::endl;
    }
    return 0;
} 