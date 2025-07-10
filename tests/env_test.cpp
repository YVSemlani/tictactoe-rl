#include <iostream>
#include "../include/environment.h"
#include <memory>
#include <cassert>

int main() {
    auto reward_fn = std::make_shared<DefaultReward>();
    Environment env(3, reward_fn);
    
    std::cout << "=== Testing US1.2: Apply Action & Transition ===" << std::endl;
    
    // Test 1: Basic functionality
    std::cout << "\n1. Testing basic reset and step..." << std::endl;
    BoardState state = env.reset();
    std::cout << "After reset, cell 0: " << state.cells[0] << std::endl;
    assert(state.cells[0] == 0);
    
    Action act{0};
    StepResult res = env.step(act);
    std::cout << "After step, cell 0: " << res.next_state.cells[0] << std::endl;
    std::cout << "Reward: " << res.reward << std::endl;
    std::cout << "Done: " << res.done << std::endl;
    assert(res.next_state.cells[0] == 1);  // Player 1
    assert(res.reward == 0);
    assert(!res.done);
    std::cout << "✓ Basic functionality works" << std::endl;
    
    // Test 2: Player alternation
    std::cout << "\n2. Testing player alternation..." << std::endl;
    env.reset();
    Action act1{0};  // Player 1 move
    Action act2{1};  // Player 2 move
    
    StepResult res1 = env.step(act1);
    std::cout << "Player 1 move at cell 0: " << res1.next_state.cells[0] << std::endl;
    assert(res1.next_state.cells[0] == 1);  // Player 1
    
    StepResult res2 = env.step(act2);
    std::cout << "Player 2 move at cell 1: " << res2.next_state.cells[1] << std::endl;
    assert(res2.next_state.cells[1] == -1);  // Player 2
    std::cout << "✓ Player alternation works" << std::endl;
    
    // Test 3: Invalid action - out of bounds
    std::cout << "\n3. Testing bounds validation..." << std::endl;
    env.reset();
    Action invalid_act{9};  // Out of bounds for 3x3 board (valid: 0-8)
    
    try {
        env.step(invalid_act);
        assert(false);  // Should not reach here
    } catch (const std::invalid_argument& e) {
        std::cout << "Caught expected exception: " << e.what() << std::endl;
        std::cout << "✓ Bounds validation works" << std::endl;
    }
    
    // Test 4: Invalid action - occupied cell
    std::cout << "\n4. Testing occupied cell validation..." << std::endl;
    env.reset();
    Action act_same{0};
    
    env.step(act_same);  // First move to cell 0
    try {
        env.step(act_same);  // Try to move to same cell
        assert(false);  // Should not reach here
    } catch (const std::invalid_argument& e) {
        std::cout << "Caught expected exception: " << e.what() << std::endl;
        std::cout << "✓ Occupied cell validation works" << std::endl;
    }
    
    // Test 5: StepResult.next_state reflects new board
    std::cout << "\n5. Testing StepResult.next_state..." << std::endl;
    env.reset();
    Action act_test{4};  // Center cell
    StepResult res_test = env.step(act_test);
    
    // Verify the returned state matches what we expect
    assert(res_test.next_state.cells[4] == 1);  // Player 1 at center
    for (int i = 0; i < 9; ++i) {
        if (i != 4) {
            assert(res_test.next_state.cells[i] == 0);  // All other cells empty
        }
    }
    std::cout << "✓ StepResult.next_state correctly reflects board changes" << std::endl;
    
    std::cout << "\n=== All US1.2 tests passed! ===" << std::endl;
    return 0;
} 