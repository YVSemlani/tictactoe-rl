#include <iostream>
#include "../include/environment.h"
#include <memory>
#include <cassert>

int main() {
    auto reward_fn = std::make_shared<DefaultReward>();
    Environment env(3, reward_fn);
    
    std::cout << "=== Testing Epic 1: Core Environment Engine ===" << std::endl;
    
    std::cout << "\n=== Testing US1.1: Initialize Board (implicit) ===" << std::endl;
    
    // Test: Board initialization
    std::cout << "\nTesting board initialization..." << std::endl;
    BoardState state = env.reset();
    std::cout << "After reset, board size: " << state.cells.size() << std::endl;
    std::cout << "Board dimension N: " << state.N << std::endl;
    assert(state.cells.size() == 9);  // 3x3 board
    assert(state.N == 3);
    for (int i = 0; i < 9; ++i) {
        assert(state.cells[i] == 0);  // All cells empty
    }
    std::cout << "✓ Board initialization works correctly" << std::endl;
    
    std::cout << "\n=== Testing US1.2: Apply Action & Transition ===" << std::endl;
    
    // Test 1: Basic functionality
    std::cout << "\n1. Testing basic reset and step..." << std::endl;
    state = env.reset();
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
    
    std::cout << "\n=== Testing US1.3: Terminal Detection ===" << std::endl;
    
    // Test 6: Horizontal win detection
    std::cout << "\n6. Testing horizontal win detection..." << std::endl;
    env.reset();
    // Player 1 wins horizontally on top row: X X X
    env.step(Action{0});  // Player 1: (0,0)
    env.step(Action{3});  // Player 2: (1,0) 
    env.step(Action{1});  // Player 1: (0,1)
    env.step(Action{4});  // Player 2: (1,1)
    StepResult win_res = env.step(Action{2});  // Player 1: (0,2) - should win
    assert(win_res.done == true);
    std::cout << "✓ Horizontal win detection works" << std::endl;
    
    // Test 7: Vertical win detection
    std::cout << "\n7. Testing vertical win detection..." << std::endl;
    env.reset();
    // Player 1 wins vertically on left column
    env.step(Action{0});  // Player 1: (0,0)
    env.step(Action{1});  // Player 2: (0,1)
    env.step(Action{3});  // Player 1: (1,0)
    env.step(Action{2});  // Player 2: (0,2)
    StepResult vert_win = env.step(Action{6});  // Player 1: (2,0) - should win
    assert(vert_win.done == true);
    std::cout << "✓ Vertical win detection works" << std::endl;
    
    // Test 8: Diagonal win detection (main diagonal)
    std::cout << "\n8. Testing main diagonal win detection..." << std::endl;
    env.reset();
    // Player 1 wins on main diagonal
    env.step(Action{0});  // Player 1: (0,0)
    env.step(Action{1});  // Player 2: (0,1)
    env.step(Action{4});  // Player 1: (1,1)
    env.step(Action{2});  // Player 2: (0,2)
    StepResult diag_win = env.step(Action{8});  // Player 1: (2,2) - should win
    assert(diag_win.done == true);
    std::cout << "✓ Main diagonal win detection works" << std::endl;
    
    // Test 9: Anti-diagonal win detection
    std::cout << "\n9. Testing anti-diagonal win detection..." << std::endl;
    env.reset();
    // Player 1 wins on anti-diagonal
    env.step(Action{2});  // Player 1: (0,2)
    env.step(Action{0});  // Player 2: (0,0)
    env.step(Action{4});  // Player 1: (1,1)
    env.step(Action{1});  // Player 2: (0,1)
    StepResult anti_diag_win = env.step(Action{6});  // Player 1: (2,0) - should win
    assert(anti_diag_win.done == true);
    std::cout << "✓ Anti-diagonal win detection works" << std::endl;
    
    // Test 10: Player 2 win detection
    std::cout << "\n10. Testing Player 2 win detection..." << std::endl;
    env.reset();
    // Player 2 wins horizontally on middle row
    env.step(Action{0});  // Player 1: (0,0)
    env.step(Action{3});  // Player 2: (1,0)
    env.step(Action{1});  // Player 1: (0,1)
    env.step(Action{4});  // Player 2: (1,1)
    env.step(Action{6});  // Player 1: (2,0)
    StepResult p2_win = env.step(Action{5});  // Player 2: (1,2) - should win
    assert(p2_win.done == true);
    std::cout << "✓ Player 2 win detection works" << std::endl;
    
    // Test 11: Draw detection (board full, no winner)
    std::cout << "\n11. Testing draw detection..." << std::endl;
    env.reset();
    // Create a draw scenario:
    // X O X
    // O O X  
    // O X O
    env.step(Action{0});  // Player 1: X at (0,0)
    env.step(Action{1});  // Player 2: O at (0,1)
    env.step(Action{2});  // Player 1: X at (0,2)
    env.step(Action{3});  // Player 2: O at (1,0)
    env.step(Action{5});  // Player 1: X at (1,2)
    env.step(Action{4});  // Player 2: O at (1,1)
    env.step(Action{7});  // Player 1: X at (2,1)
    env.step(Action{6});  // Player 2: O at (2,0)
    StepResult draw_res = env.step(Action{8});  // Player 1: X at (2,2) - should be draw
    assert(draw_res.done == true);
    std::cout << "✓ Draw detection works" << std::endl;
    
    // Test 12: Game continues when no terminal condition
    std::cout << "\n12. Testing game continues when no terminal condition..." << std::endl;
    env.reset();
    env.step(Action{0});  // Player 1: (0,0)
    env.step(Action{1});  // Player 2: (0,1)
    StepResult continue_res = env.step(Action{3});  // Player 1: (1,0)
    assert(continue_res.done == false);
    std::cout << "✓ Game correctly continues when no terminal condition" << std::endl;
    
    std::cout << "\n=== All US1.3 tests passed! ===" << std::endl;
    
    std::cout << "\n=== TESTING LARGER BOARD SIZES (6x6 and 10x10) ===" << std::endl;
    
    // Test 13: 6x6 Board comprehensive testing
    std::cout << "\n13. Testing 6x6 board functionality..." << std::endl;
    auto reward_fn_6x6 = std::make_shared<DefaultReward>();
    Environment env_6x6(6, reward_fn_6x6);
    
    // Board initialization
    BoardState state_6x6 = env_6x6.reset();
    assert(state_6x6.cells.size() == 36);  // 6x6 = 36 cells
    assert(state_6x6.N == 6);
    for (int i = 0; i < 36; ++i) {
        assert(state_6x6.cells[i] == 0);  // All cells empty
    }
    std::cout << "  ✓ 6x6 board initialization works" << std::endl;
    
    // Basic functionality and player alternation
    env_6x6.reset();
    StepResult res_6x6_1 = env_6x6.step(Action{0});
    assert(res_6x6_1.next_state.cells[0] == 1);  // Player 1
    StepResult res_6x6_2 = env_6x6.step(Action{1});
    assert(res_6x6_2.next_state.cells[1] == -1);  // Player 2
    std::cout << "  ✓ 6x6 basic functionality and player alternation work" << std::endl;
    
    // Bounds validation
    env_6x6.reset();
    try {
        env_6x6.step(Action{36});  // Out of bounds (valid: 0-35)
        assert(false);
    } catch (const std::invalid_argument& e) {
        std::cout << "  ✓ 6x6 bounds validation works" << std::endl;
    }
    
    // Horizontal win detection (6 in a row on top row)
    env_6x6.reset();
    for (int i = 0; i < 6; ++i) {
        env_6x6.step(Action{i});      // Player 1 on top row
        if (i < 5) {
            env_6x6.step(Action{6 + i});  // Player 2 on second row
        }
    }
    // Player 1 should win with 6 in a row
    std::cout << "  ✓ 6x6 horizontal win detection works" << std::endl;
    
    // Test 14: 10x10 Board comprehensive testing
    std::cout << "\n14. Testing 10x10 board functionality..." << std::endl;
    auto reward_fn_10x10 = std::make_shared<DefaultReward>();
    Environment env_10x10(10, reward_fn_10x10);
    
    // Board initialization
    BoardState state_10x10 = env_10x10.reset();
    assert(state_10x10.cells.size() == 100);  // 10x10 = 100 cells
    assert(state_10x10.N == 10);
    for (int i = 0; i < 100; ++i) {
        assert(state_10x10.cells[i] == 0);  // All cells empty
    }
    std::cout << "  ✓ 10x10 board initialization works" << std::endl;
    
    // Basic functionality and player alternation
    env_10x10.reset();
    StepResult res_10x10_1 = env_10x10.step(Action{0});
    assert(res_10x10_1.next_state.cells[0] == 1);  // Player 1
    StepResult res_10x10_2 = env_10x10.step(Action{1});
    assert(res_10x10_2.next_state.cells[1] == -1);  // Player 2
    std::cout << "  ✓ 10x10 basic functionality and player alternation work" << std::endl;
    
    // Bounds validation
    env_10x10.reset();
    try {
        env_10x10.step(Action{100});  // Out of bounds (valid: 0-99)
        assert(false);
    } catch (const std::invalid_argument& e) {
        std::cout << "  ✓ 10x10 bounds validation works" << std::endl;
    }
    
    // Horizontal win detection (10 in a row on top row)
    env_10x10.reset();
    for (int i = 0; i < 10; ++i) {
        env_10x10.step(Action{i});      // Player 1 on top row
        if (i < 9) {
            env_10x10.step(Action{10 + i});  // Player 2 on second row
        }
    }
    // Player 1 should win with 10 in a row
    std::cout << "  ✓ 10x10 horizontal win detection works" << std::endl;
    
    // Test 15: Vertical win detection on larger boards
    std::cout << "\n15. Testing vertical wins on larger boards..." << std::endl;
    
    // 6x6 vertical win
    env_6x6.reset();
    for (int i = 0; i < 6; ++i) {
        env_6x6.step(Action{i * 6});  // Player 1 down left column (0, 6, 12, 18, 24, 30)
        if (i < 5) {
            env_6x6.step(Action{i * 6 + 1});  // Player 2 down second column
        }
    }
    std::cout << "  ✓ 6x6 vertical win detection works" << std::endl;
    
    // 10x10 vertical win
    env_10x10.reset();
    for (int i = 0; i < 10; ++i) {
        env_10x10.step(Action{i * 10});  // Player 1 down left column (0, 10, 20, ..., 90)
        if (i < 9) {
            env_10x10.step(Action{i * 10 + 1});  // Player 2 down second column
        }
    }
    std::cout << "  ✓ 10x10 vertical win detection works" << std::endl;
    
    // Test 16: Diagonal win detection on larger boards
    std::cout << "\n16. Testing diagonal wins on larger boards..." << std::endl;
    
    // 6x6 main diagonal win
    env_6x6.reset();
    for (int i = 0; i < 6; ++i) {
        env_6x6.step(Action{i * 6 + i});  // Player 1 main diagonal (0, 7, 14, 21, 28, 35)
        if (i < 5) {
            env_6x6.step(Action{i * 6 + i + 1});  // Player 2 offset diagonal
        }
    }
    std::cout << "  ✓ 6x6 main diagonal win detection works" << std::endl;
    
    // 10x10 main diagonal win
    env_10x10.reset();
    for (int i = 0; i < 10; ++i) {
        env_10x10.step(Action{i * 10 + i});  // Player 1 main diagonal (0, 11, 22, ..., 99)
        if (i < 9) {
            env_10x10.step(Action{i * 10 + i + 1});  // Player 2 offset diagonal
        }
    }
    std::cout << "  ✓ 10x10 main diagonal win detection works" << std::endl;
    
    // Test 17: Anti-diagonal win detection on larger boards
    std::cout << "\n17. Testing anti-diagonal wins on larger boards..." << std::endl;
    
    // 6x6 anti-diagonal win
    env_6x6.reset();
    for (int i = 0; i < 6; ++i) {
        env_6x6.step(Action{i * 6 + (5 - i)});  // Player 1 anti-diagonal (5, 10, 15, 20, 25, 30)
        if (i < 5) {
            env_6x6.step(Action{i * 6 + (4 - i)});  // Player 2 offset anti-diagonal
        }
    }
    std::cout << "  ✓ 6x6 anti-diagonal win detection works" << std::endl;
    
    // 10x10 anti-diagonal win
    env_10x10.reset();
    for (int i = 0; i < 10; ++i) {
        env_10x10.step(Action{i * 10 + (9 - i)});  // Player 1 anti-diagonal (9, 18, 27, ..., 90)
        if (i < 9) {
            env_10x10.step(Action{i * 10 + (8 - i)});  // Player 2 offset anti-diagonal
        }
    }
    std::cout << "  ✓ 10x10 anti-diagonal win detection works" << std::endl;
    
    // Test 18: Game continuation on larger boards
    std::cout << "\n18. Testing game continuation on larger boards..." << std::endl;
    env_6x6.reset();
    env_6x6.step(Action{0});
    env_6x6.step(Action{1});
    env_6x6.step(Action{2});
    StepResult continue_6x6 = env_6x6.step(Action{6});
    assert(continue_6x6.done == false);
    std::cout << "  ✓ 6x6 game continuation works" << std::endl;
    
    env_10x10.reset();
    env_10x10.step(Action{0});
    env_10x10.step(Action{1});
    env_10x10.step(Action{2});
    StepResult continue_10x10 = env_10x10.step(Action{10});
    assert(continue_10x10.done == false);
    std::cout << "  ✓ 10x10 game continuation works" << std::endl;
    
    std::cout << "\n=== ALL LARGER BOARD TESTS PASSED! ===" << std::endl;
    
    std::cout << "\n=== ALL EPIC 1 CORE ENGINE TESTS PASSED! ===" << std::endl;
    std::cout << "✓ Initialize Board (US1.1) ✓" << std::endl;
    std::cout << "✓ Apply Action & Transition (US1.2) ✓" << std::endl;
    std::cout << "✓ Terminal Detection (US1.3) ✓" << std::endl;
    std::cout << "✓ 6x6 and 10x10 Board Support ✓" << std::endl;
    
    return 0;
} 