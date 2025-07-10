#include <iostream>
#include "../include/environment.h"
#include <memory>
#include <cassert>

int main() {
    auto reward_fn = std::make_shared<DefaultReward>();
    Environment env(3, reward_fn);
    
    std::cout << "=== Testing Epic 2: State & Action Representation ===" << std::endl;
    
    std::cout << "\n=== Testing US2.3: Action Masking ===" << std::endl;
    
    // Test 1: Basic action mask functionality - empty board
    std::cout << "\n1. Testing action mask on empty board..." << std::endl;
    env.reset();
    std::vector<bool> empty_mask = env.get_action_mask();
    assert(empty_mask.size() == 9);  // 3x3 board
    for (int i = 0; i < 9; ++i) {
        assert(empty_mask[i] == true);  // All cells should be available
    }
    std::cout << "✓ Action mask correctly shows all cells available on empty board" << std::endl;
    
    // Test 2: Action mask after single move
    std::cout << "\n2. Testing action mask after single move..." << std::endl;
    env.reset();
    env.step(Action{4});  // Place piece at center (index 4)
    std::vector<bool> single_move_mask = env.get_action_mask();
    assert(single_move_mask.size() == 9);
    for (int i = 0; i < 9; ++i) {
        if (i == 4) {
            assert(single_move_mask[i] == false);  // Center cell should be unavailable
        } else {
            assert(single_move_mask[i] == true);   // All other cells should be available
        }
    }
    std::cout << "✓ Action mask correctly shows occupied cell as unavailable" << std::endl;
    
    // Test 3: Action mask with multiple moves
    std::cout << "\n3. Testing action mask with multiple moves..." << std::endl;
    env.reset();
    env.step(Action{0});  // Player 1 at (0,0)
    env.step(Action{1});  // Player 2 at (0,1)
    env.step(Action{4});  // Player 1 at (1,1)
    std::vector<bool> multi_move_mask = env.get_action_mask();
    assert(multi_move_mask.size() == 9);
    std::vector<int> occupied_cells = {0, 1, 4};
    for (int i = 0; i < 9; ++i) {
        bool should_be_available = true;
        for (int occupied : occupied_cells) {
            if (i == occupied) {
                should_be_available = false;
                break;
            }
        }
        assert(multi_move_mask[i] == should_be_available);
    }
    std::cout << "✓ Action mask correctly handles multiple occupied cells" << std::endl;
    
    // Test 4: Action mask on nearly full board
    std::cout << "\n4. Testing action mask on nearly full board..." << std::endl;
    env.reset();
    // Fill all cells except the last one
    for (int i = 0; i < 8; ++i) {
        env.step(Action{i});
    }
    std::vector<bool> nearly_full_mask = env.get_action_mask();
    assert(nearly_full_mask.size() == 9);
    for (int i = 0; i < 8; ++i) {
        assert(nearly_full_mask[i] == false);  // First 8 cells should be unavailable
    }
    assert(nearly_full_mask[8] == true);  // Last cell should be available
    std::cout << "✓ Action mask correctly shows only last empty cell as available" << std::endl;
    
    // Test 5: Action mask size matches board size for different N
    std::cout << "\n5. Testing action mask size for different board sizes..." << std::endl;
    auto reward_fn_4x4 = std::make_shared<DefaultReward>();
    Environment env_4x4(4, reward_fn_4x4);
    env_4x4.reset();
    std::vector<bool> mask_4x4 = env_4x4.get_action_mask();
    assert(mask_4x4.size() == 16);  // 4x4 board should have 16 cells
    for (int i = 0; i < 16; ++i) {
        assert(mask_4x4[i] == true);  // All cells should be available on empty 4x4 board
    }
    std::cout << "✓ Action mask correctly handles different board sizes" << std::endl;
    
    std::cout << "\n=== All US2.3 tests passed! ===" << std::endl;
    
    std::cout << "\n=== Testing US2.1: Flattened State Vector ===" << std::endl;
    
    // Test 6: Flattened state on empty board
    std::cout << "\n6. Testing flattened state on empty board..." << std::endl;
    env.reset();
    std::vector<float> empty_state = env.get_flattened_state();
    assert(empty_state.size() == 9);  // 3x3 board = 9 elements
    for (int i = 0; i < 9; ++i) {
        assert(empty_state[i] == 0.0f);  // All cells should be 0 (empty)
    }
    std::cout << "✓ Flattened state correctly represents empty board" << std::endl;
    
    // Test 7: Flattened state after single move
    std::cout << "\n7. Testing flattened state after single move..." << std::endl;
    env.reset();
    env.step(Action{4});  // Player 1 at center (index 4)
    std::vector<float> single_move_state = env.get_flattened_state();
    assert(single_move_state.size() == 9);
    for (int i = 0; i < 9; ++i) {
        if (i == 4) {
            assert(single_move_state[i] == 1.0f);  // Player 1 piece
        } else {
            assert(single_move_state[i] == 0.0f);  // Empty cells
        }
    }
    std::cout << "✓ Flattened state correctly represents single player 1 move" << std::endl;
    
    // Test 8: Flattened state with both players
    std::cout << "\n8. Testing flattened state with both players..." << std::endl;
    env.reset();
    env.step(Action{0});  // Player 1 at (0,0)
    env.step(Action{1});  // Player 2 at (0,1)
    env.step(Action{4});  // Player 1 at (1,1)
    std::vector<float> both_players_state = env.get_flattened_state();
    assert(both_players_state.size() == 9);
    assert(both_players_state[0] == 1.0f);   // Player 1
    assert(both_players_state[1] == -1.0f);  // Player 2
    assert(both_players_state[4] == 1.0f);   // Player 1
    for (int i = 2; i < 9; ++i) {
        if (i != 4) {
            assert(both_players_state[i] == 0.0f);  // Empty cells
        }
    }
    std::cout << "✓ Flattened state correctly represents both players" << std::endl;
    
    // Test 9: Flattened state values in correct range
    std::cout << "\n9. Testing flattened state values are in {-1, 0, +1}..." << std::endl;
    env.reset();
    // Create a complex board state
    env.step(Action{0});  // Player 1: +1
    env.step(Action{1});  // Player 2: -1
    env.step(Action{2});  // Player 1: +1
    env.step(Action{3});  // Player 2: -1
    env.step(Action{5});  // Player 1: +1
    std::vector<float> complex_state = env.get_flattened_state();
    for (int i = 0; i < 9; ++i) {
        assert(complex_state[i] == -1.0f || complex_state[i] == 0.0f || complex_state[i] == 1.0f);
    }
    std::cout << "✓ All flattened state values are in the correct range {-1, 0, +1}" << std::endl;
    
    // Test 10: Flattened state size for different board sizes
    std::cout << "\n10. Testing flattened state size for different board sizes..." << std::endl;
    auto reward_fn_5x5 = std::make_shared<DefaultReward>();
    Environment env_5x5(5, reward_fn_5x5);
    env_5x5.reset();
    std::vector<float> state_5x5 = env_5x5.get_flattened_state();
    assert(state_5x5.size() == 25);  // 5x5 = 25 elements
    for (int i = 0; i < 25; ++i) {
        assert(state_5x5[i] == 0.0f);  // All empty on reset
    }
    std::cout << "✓ Flattened state correctly handles different board sizes" << std::endl;
    
    // Test 11: Flattened state matches board layout
    std::cout << "\n11. Testing flattened state matches board layout..." << std::endl;
    env.reset();
    // Create specific pattern: X in corners, O in edges
    env.step(Action{0});  // X at (0,0) -> index 0
    env.step(Action{1});  // O at (0,1) -> index 1  
    env.step(Action{8});  // X at (2,2) -> index 8
    env.step(Action{3});  // O at (1,0) -> index 3
    std::vector<float> pattern_state = env.get_flattened_state();
    assert(pattern_state[0] == 1.0f);   // X at top-left
    assert(pattern_state[1] == -1.0f);  // O at top-center
    assert(pattern_state[3] == -1.0f);  // O at middle-left
    assert(pattern_state[8] == 1.0f);   // X at bottom-right
    // Check other positions are empty
    std::vector<int> empty_positions = {2, 4, 5, 6, 7};
    for (int pos : empty_positions) {
        assert(pattern_state[pos] == 0.0f);
    }
    std::cout << "✓ Flattened state correctly preserves board layout" << std::endl;
    
    std::cout << "\n=== All US2.1 tests passed! ===" << std::endl;
    
    std::cout << "\n=== Testing US2.2: One-Hot Encoding Option ===" << std::endl;
    
    // Test 12: One-hot encoding on empty board
    std::cout << "\n12. Testing one-hot encoding on empty board..." << std::endl;
    env.reset();
    std::vector<float> empty_one_hot = env.get_one_hot_state();
    assert(empty_one_hot.size() == 18);  // 2 * 3 * 3 = 18 for 3x3 board
    for (int i = 0; i < 18; ++i) {
        assert(empty_one_hot[i] == 0.0f);  // All should be zero on empty board
    }
    std::cout << "✓ One-hot encoding correctly returns all zeros for empty board" << std::endl;
    
    // Test 13: One-hot encoding with single Player 1 move
    std::cout << "\n13. Testing one-hot encoding with Player 1 move..." << std::endl;
    env.reset();
    env.step(Action{4});  // Player 1 places at center (index 4)
    std::vector<float> p1_one_hot = env.get_one_hot_state();
    assert(p1_one_hot.size() == 18);
    
    // Check Player 1 channel (indices 0-8): should have 1.0 at index 4
    for (int i = 0; i < 9; ++i) {
        if (i == 4) {
            assert(p1_one_hot[i] == 1.0f);  // Player 1 at center
        } else {
            assert(p1_one_hot[i] == 0.0f);  // All other cells empty in Player 1 channel
        }
    }
    
    // Check Player 2 channel (indices 9-17): should be all zeros
    for (int i = 9; i < 18; ++i) {
        assert(p1_one_hot[i] == 0.0f);  // Player 2 channel should be empty
    }
    std::cout << "✓ One-hot encoding correctly encodes Player 1 move" << std::endl;
    
    // Test 14: One-hot encoding with single Player 2 move
    std::cout << "\n14. Testing one-hot encoding with Player 2 move..." << std::endl;
    env.reset();
    env.step(Action{0});  // Player 1 at (0,0)
    env.step(Action{4});  // Player 2 at center (index 4)
    std::vector<float> p2_one_hot = env.get_one_hot_state();
    assert(p2_one_hot.size() == 18);
    
    // Check Player 1 channel (indices 0-8): should have 1.0 at index 0
    assert(p2_one_hot[0] == 1.0f);  // Player 1 at (0,0)
    for (int i = 1; i < 9; ++i) {
        assert(p2_one_hot[i] == 0.0f);  // All other cells empty in Player 1 channel
    }
    
    // Check Player 2 channel (indices 9-17): should have 1.0 at index 13 (9+4)
    for (int i = 9; i < 18; ++i) {
        if (i == 13) {  // 9 + 4 = 13 (Player 2 at center)
            assert(p2_one_hot[i] == 1.0f);  // Player 2 at center
        } else {
            assert(p2_one_hot[i] == 0.0f);  // All other cells empty in Player 2 channel
        }
    }
    std::cout << "✓ One-hot encoding correctly encodes Player 2 move" << std::endl;
    
    // Test 15: One-hot encoding with multiple moves from both players
    std::cout << "\n15. Testing one-hot encoding with multiple moves..." << std::endl;
    env.reset();
    env.step(Action{0});  // Player 1 at (0,0) - index 0
    env.step(Action{1});  // Player 2 at (0,1) - index 1  
    env.step(Action{4});  // Player 1 at (1,1) - index 4
    env.step(Action{8});  // Player 2 at (2,2) - index 8
    std::vector<float> multi_one_hot = env.get_one_hot_state();
    assert(multi_one_hot.size() == 18);
    
    // Check Player 1 channel (indices 0-8): should have 1.0 at indices 0 and 4
    std::vector<int> p1_positions = {0, 4};
    for (int i = 0; i < 9; ++i) {
        bool should_be_one = false;
        for (int pos : p1_positions) {
            if (i == pos) {
                should_be_one = true;
                break;
            }
        }
        assert(multi_one_hot[i] == (should_be_one ? 1.0f : 0.0f));
    }
    
    // Check Player 2 channel (indices 9-17): should have 1.0 at indices 10 (9+1) and 17 (9+8)
    std::vector<int> p2_positions = {10, 17};  // 9+1=10, 9+8=17
    for (int i = 9; i < 18; ++i) {
        bool should_be_one = false;
        for (int pos : p2_positions) {
            if (i == pos) {
                should_be_one = true;
                break;
            }
        }
        assert(multi_one_hot[i] == (should_be_one ? 1.0f : 0.0f));
    }
    std::cout << "✓ One-hot encoding correctly handles multiple moves from both players" << std::endl;
    
    // Test 16: One-hot encoding tensor shape for different board sizes
    std::cout << "\n16. Testing one-hot encoding shape for different board sizes..." << std::endl;
    auto reward_fn_2x2_oh = std::make_shared<DefaultReward>();
    Environment env_2x2_oh(2, reward_fn_2x2_oh);
    env_2x2_oh.reset();
    std::vector<float> one_hot_2x2 = env_2x2_oh.get_one_hot_state();
    assert(one_hot_2x2.size() == 8);  // 2 * 2 * 2 = 8 for 2x2 board
    
    auto reward_fn_5x5_oh = std::make_shared<DefaultReward>();
    Environment env_5x5_oh(5, reward_fn_5x5_oh);
    env_5x5_oh.reset();
    std::vector<float> one_hot_5x5 = env_5x5_oh.get_one_hot_state();
    assert(one_hot_5x5.size() == 50);  // 2 * 5 * 5 = 50 for 5x5 board
    std::cout << "✓ One-hot encoding correctly handles different board sizes" << std::endl;
    
    // Test 17: One-hot encoding channel organization verification
    std::cout << "\n17. Testing one-hot encoding channel organization..." << std::endl;
    env.reset();
    env.step(Action{2});  // Player 1 at index 2
    env.step(Action{5});  // Player 2 at index 5
    std::vector<float> channel_test = env.get_one_hot_state();
    
    // Verify Player 1 channel: first N*N elements (0-8 for 3x3)
    assert(channel_test[2] == 1.0f);  // Player 1 piece at index 2
    assert(channel_test[5] == 0.0f);  // No Player 1 piece at index 5
    
    // Verify Player 2 channel: second N*N elements (9-17 for 3x3)  
    assert(channel_test[9 + 2] == 0.0f);  // No Player 2 piece at index 2 (channel offset + 2)
    assert(channel_test[9 + 5] == 1.0f);  // Player 2 piece at index 5 (channel offset + 5)
    
    std::cout << "✓ One-hot encoding channels are correctly organized" << std::endl;
    
    std::cout << "\n=== All US2.2 tests passed! ===" << std::endl;
    
    std::cout << "\n=== TESTING LARGER BOARD SIZES (6x6 and 10x10) STATE REPRESENTATIONS ===" << std::endl;
    
    // Test 18: 6x6 Board State Representations
    std::cout << "\n18. Testing 6x6 board state representations..." << std::endl;
    auto reward_fn_6x6 = std::make_shared<DefaultReward>();
    Environment env_6x6(6, reward_fn_6x6);
    
    // Action masking for 6x6
    env_6x6.reset();
    std::vector<bool> mask_6x6_empty = env_6x6.get_action_mask();
    assert(mask_6x6_empty.size() == 36);  // 6x6 = 36 cells
    for (int i = 0; i < 36; ++i) {
        assert(mask_6x6_empty[i] == true);  // All cells available on empty board
    }
    std::cout << "  ✓ 6x6 action mask on empty board works (36 cells all available)" << std::endl;
    
    // Test action mask after moves on 6x6
    env_6x6.step(Action{0});  // Player 1 at (0,0)
    env_6x6.step(Action{35}); // Player 2 at (5,5) - bottom right
    env_6x6.step(Action{18}); // Player 1 at center (3,0)
    std::vector<bool> mask_6x6_partial = env_6x6.get_action_mask();
    assert(mask_6x6_partial[0] == false);   // Occupied by Player 1
    assert(mask_6x6_partial[35] == false);  // Occupied by Player 2
    assert(mask_6x6_partial[18] == false);  // Occupied by Player 1
    assert(mask_6x6_partial[1] == true);    // Should be available
    assert(mask_6x6_partial[17] == true);   // Should be available
    std::cout << "  ✓ 6x6 action mask correctly tracks occupied cells" << std::endl;
    
    // Flattened state for 6x6
    env_6x6.reset();
    std::vector<float> flat_6x6_empty = env_6x6.get_flattened_state();
    assert(flat_6x6_empty.size() == 36);  // 6x6 = 36 elements
    for (int i = 0; i < 36; ++i) {
        assert(flat_6x6_empty[i] == 0.0f);  // All empty
    }
    std::cout << "  ✓ 6x6 flattened state on empty board works (36 elements all 0.0)" << std::endl;
    
    // Test flattened state with moves on 6x6
    env_6x6.step(Action{0});   // Player 1 at (0,0)
    env_6x6.step(Action{35});  // Player 2 at (5,5)
    env_6x6.step(Action{18});  // Player 1 at (3,0)
    std::vector<float> flat_6x6_partial = env_6x6.get_flattened_state();
    assert(flat_6x6_partial[0] == 1.0f);   // Player 1
    assert(flat_6x6_partial[35] == -1.0f); // Player 2
    assert(flat_6x6_partial[18] == 1.0f);  // Player 1
    assert(flat_6x6_partial[1] == 0.0f);   // Empty
    assert(flat_6x6_partial[17] == 0.0f);  // Empty
    std::cout << "  ✓ 6x6 flattened state correctly represents player pieces" << std::endl;
    
    // One-hot encoding for 6x6
    env_6x6.reset();
    std::vector<float> oh_6x6_empty = env_6x6.get_one_hot_state();
    assert(oh_6x6_empty.size() == 72);  // 2 * 6 * 6 = 72 for 6x6 board
    for (int i = 0; i < 72; ++i) {
        assert(oh_6x6_empty[i] == 0.0f);  // All channels empty
    }
    std::cout << "  ✓ 6x6 one-hot encoding on empty board works (72 elements all 0.0)" << std::endl;
    
    // Test one-hot encoding with moves on 6x6
    env_6x6.step(Action{0});   // Player 1 at (0,0)
    env_6x6.step(Action{35});  // Player 2 at (5,5)
    env_6x6.step(Action{18});  // Player 1 at (3,0)
    std::vector<float> oh_6x6_partial = env_6x6.get_one_hot_state();
    // Player 1 channel (0-35): should have 1.0 at indices 0 and 18
    assert(oh_6x6_partial[0] == 1.0f);   // Player 1 at (0,0)
    assert(oh_6x6_partial[18] == 1.0f);  // Player 1 at (3,0)
    assert(oh_6x6_partial[35] == 0.0f);  // No Player 1 at (5,5)
    // Player 2 channel (36-71): should have 1.0 at index 71 (36+35)
    assert(oh_6x6_partial[36 + 0] == 0.0f);  // No Player 2 at (0,0)
    assert(oh_6x6_partial[36 + 35] == 1.0f); // Player 2 at (5,5)
    assert(oh_6x6_partial[36 + 18] == 0.0f); // No Player 2 at (3,0)
    std::cout << "  ✓ 6x6 one-hot encoding correctly separates player channels" << std::endl;
    
    // Test 19: 10x10 Board State Representations
    std::cout << "\n19. Testing 10x10 board state representations..." << std::endl;
    auto reward_fn_10x10 = std::make_shared<DefaultReward>();
    Environment env_10x10(10, reward_fn_10x10);
    
    // Action masking for 10x10
    env_10x10.reset();
    std::vector<bool> mask_10x10_empty = env_10x10.get_action_mask();
    assert(mask_10x10_empty.size() == 100);  // 10x10 = 100 cells
    for (int i = 0; i < 100; ++i) {
        assert(mask_10x10_empty[i] == true);  // All cells available on empty board
    }
    std::cout << "  ✓ 10x10 action mask on empty board works (100 cells all available)" << std::endl;
    
    // Test action mask after moves on 10x10
    env_10x10.step(Action{0});   // Player 1 at (0,0)
    env_10x10.step(Action{99});  // Player 2 at (9,9) - bottom right
    env_10x10.step(Action{44});  // Player 1 at center-ish (4,4)
    env_10x10.step(Action{55});  // Player 2 at (5,5)
    std::vector<bool> mask_10x10_partial = env_10x10.get_action_mask();
    assert(mask_10x10_partial[0] == false);   // Occupied by Player 1
    assert(mask_10x10_partial[99] == false);  // Occupied by Player 2
    assert(mask_10x10_partial[44] == false);  // Occupied by Player 1
    assert(mask_10x10_partial[55] == false);  // Occupied by Player 2
    assert(mask_10x10_partial[1] == true);    // Should be available
    assert(mask_10x10_partial[50] == true);   // Should be available
    std::cout << "  ✓ 10x10 action mask correctly tracks occupied cells" << std::endl;
    
    // Flattened state for 10x10
    env_10x10.reset();
    std::vector<float> flat_10x10_empty = env_10x10.get_flattened_state();
    assert(flat_10x10_empty.size() == 100);  // 10x10 = 100 elements
    for (int i = 0; i < 100; ++i) {
        assert(flat_10x10_empty[i] == 0.0f);  // All empty
    }
    std::cout << "  ✓ 10x10 flattened state on empty board works (100 elements all 0.0)" << std::endl;
    
    // Test flattened state with moves on 10x10
    env_10x10.step(Action{0});   // Player 1 at (0,0)
    env_10x10.step(Action{99});  // Player 2 at (9,9)
    env_10x10.step(Action{44});  // Player 1 at (4,4)
    env_10x10.step(Action{55});  // Player 2 at (5,5)
    std::vector<float> flat_10x10_partial = env_10x10.get_flattened_state();
    assert(flat_10x10_partial[0] == 1.0f);   // Player 1
    assert(flat_10x10_partial[99] == -1.0f); // Player 2
    assert(flat_10x10_partial[44] == 1.0f);  // Player 1
    assert(flat_10x10_partial[55] == -1.0f); // Player 2
    assert(flat_10x10_partial[1] == 0.0f);   // Empty
    assert(flat_10x10_partial[50] == 0.0f);  // Empty
    std::cout << "  ✓ 10x10 flattened state correctly represents player pieces" << std::endl;
    
    // One-hot encoding for 10x10
    env_10x10.reset();
    std::vector<float> oh_10x10_empty = env_10x10.get_one_hot_state();
    assert(oh_10x10_empty.size() == 200);  // 2 * 10 * 10 = 200 for 10x10 board
    for (int i = 0; i < 200; ++i) {
        assert(oh_10x10_empty[i] == 0.0f);  // All channels empty
    }
    std::cout << "  ✓ 10x10 one-hot encoding on empty board works (200 elements all 0.0)" << std::endl;
    
    // Test one-hot encoding with moves on 10x10
    env_10x10.step(Action{0});   // Player 1 at (0,0)
    env_10x10.step(Action{99});  // Player 2 at (9,9)
    env_10x10.step(Action{44});  // Player 1 at (4,4)
    env_10x10.step(Action{55});  // Player 2 at (5,5)
    std::vector<float> oh_10x10_partial = env_10x10.get_one_hot_state();
    // Player 1 channel (0-99): should have 1.0 at indices 0 and 44
    assert(oh_10x10_partial[0] == 1.0f);   // Player 1 at (0,0)
    assert(oh_10x10_partial[44] == 1.0f);  // Player 1 at (4,4)
    assert(oh_10x10_partial[99] == 0.0f);  // No Player 1 at (9,9)
    assert(oh_10x10_partial[55] == 0.0f);  // No Player 1 at (5,5)
    // Player 2 channel (100-199): should have 1.0 at indices 199 (100+99) and 155 (100+55)
    assert(oh_10x10_partial[100 + 0] == 0.0f);  // No Player 2 at (0,0)
    assert(oh_10x10_partial[100 + 99] == 1.0f); // Player 2 at (9,9)
    assert(oh_10x10_partial[100 + 44] == 0.0f); // No Player 2 at (4,4)
    assert(oh_10x10_partial[100 + 55] == 1.0f); // Player 2 at (5,5)
    std::cout << "  ✓ 10x10 one-hot encoding correctly separates player channels" << std::endl;
    
    // Test 20: Cross-representation consistency on larger boards
    std::cout << "\n20. Testing cross-representation consistency on larger boards..." << std::endl;
    
    // Test 6x6 consistency
    for (int i = 0; i < 36; ++i) {
        bool is_empty_flat = (flat_6x6_partial[i] == 0.0f);
        bool is_empty_oh = (oh_6x6_partial[i] == 0.0f && oh_6x6_partial[36 + i] == 0.0f);
        bool is_available = mask_6x6_partial[i];
        assert(is_empty_flat == is_empty_oh);
        assert(is_empty_flat == is_available);
    }
    std::cout << "  ✓ 6x6 board: all representations are consistent" << std::endl;
    
    // Test 10x10 consistency
    for (int i = 0; i < 100; ++i) {
        bool is_empty_flat = (flat_10x10_partial[i] == 0.0f);
        bool is_empty_oh = (oh_10x10_partial[i] == 0.0f && oh_10x10_partial[100 + i] == 0.0f);
        bool is_available = mask_10x10_partial[i];
        assert(is_empty_flat == is_empty_oh);
        assert(is_empty_flat == is_available);
    }
    std::cout << "  ✓ 10x10 board: all representations are consistent" << std::endl;
    
    // Test 21: Performance validation on larger boards
    std::cout << "\n21. Testing performance characteristics on larger boards..." << std::endl;
    
    // Fill significant portion of 6x6 board and verify all representations scale
    env_6x6.reset();
    for (int i = 0; i < 20; ++i) {  // Fill about half the board
        env_6x6.step(Action{i});
    }
    std::vector<bool> mask_6x6_half = env_6x6.get_action_mask();
    std::vector<float> flat_6x6_half = env_6x6.get_flattened_state();
    std::vector<float> oh_6x6_half = env_6x6.get_one_hot_state();
    
    // Verify sizes
    assert(mask_6x6_half.size() == 36);
    assert(flat_6x6_half.size() == 36);
    assert(oh_6x6_half.size() == 72);
    
    // Count occupied cells
    int occupied_count = 0;
    for (int i = 0; i < 36; ++i) {
        if (!mask_6x6_half[i]) occupied_count++;
    }
    assert(occupied_count == 20);  // Should have 20 occupied cells
    std::cout << "  ✓ 6x6 board handles partial fills correctly (20/36 cells occupied)" << std::endl;
    
    // Fill significant portion of 10x10 board
    env_10x10.reset();
    for (int i = 0; i < 50; ++i) {  // Fill half the board
        env_10x10.step(Action{i});
    }
    std::vector<bool> mask_10x10_half = env_10x10.get_action_mask();
    std::vector<float> flat_10x10_half = env_10x10.get_flattened_state();
    std::vector<float> oh_10x10_half = env_10x10.get_one_hot_state();
    
    // Verify sizes
    assert(mask_10x10_half.size() == 100);
    assert(flat_10x10_half.size() == 100);
    assert(oh_10x10_half.size() == 200);
    
    // Count occupied cells
    occupied_count = 0;
    for (int i = 0; i < 100; ++i) {
        if (!mask_10x10_half[i]) occupied_count++;
    }
    assert(occupied_count == 50);  // Should have 50 occupied cells
    std::cout << "  ✓ 10x10 board handles partial fills correctly (50/100 cells occupied)" << std::endl;
    
    std::cout << "\n=== ALL LARGER BOARD STATE REPRESENTATION TESTS PASSED! ===" << std::endl;
    
    std::cout << "\n=== ALL EPIC 2 STATE REPRESENTATION TESTS PASSED! ===" << std::endl;
    std::cout << "✓ Action Masking (US2.3) ✓" << std::endl;
    std::cout << "✓ Flattened State Vector (US2.1) ✓" << std::endl;
    std::cout << "✓ One-Hot Encoding Option (US2.2) ✓" << std::endl;
    std::cout << "✓ 6x6 and 10x10 Board State Representations ✓" << std::endl;
    
    return 0;
} 