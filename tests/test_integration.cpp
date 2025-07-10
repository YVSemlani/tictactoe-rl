#include <iostream>
#include "../include/environment.h"
#include <memory>
#include <cassert>

int main() {
    auto reward_fn = std::make_shared<DefaultReward>();
    Environment env(3, reward_fn);
    
    std::cout << "=== Testing Epic 2: State Representation Integration ===" << std::endl;
    std::cout << "=== Validating All State Representations Work Together ===" << std::endl;
    
    // Test 1: Consistency between flattened state and action mask
    std::cout << "\n1. Testing consistency between flattened state and action mask..." << std::endl;
    env.reset();
    env.step(Action{0});  // Player 1 at (0,0)
    env.step(Action{4});  // Player 2 at (1,1)
    env.step(Action{8});  // Player 1 at (2,2)
    
    std::vector<float> flattened = env.get_flattened_state();
    std::vector<bool> action_mask = env.get_action_mask();
    
    // Verify that empty cells (0.0) in flattened correspond to true in action mask
    // and occupied cells (non-zero) correspond to false in action mask
    for (int i = 0; i < 9; ++i) {
        bool is_empty = (flattened[i] == 0.0f);
        bool is_available = action_mask[i];
        assert(is_empty == is_available);
    }
    std::cout << "✓ Flattened state and action mask are perfectly consistent" << std::endl;
    
    // Test 2: Consistency between one-hot state and action mask
    std::cout << "\n2. Testing consistency between one-hot state and action mask..." << std::endl;
    std::vector<float> one_hot = env.get_one_hot_state();
    
    for (int i = 0; i < 9; ++i) {
        // A cell is empty if both channels are 0 at that position
        bool is_empty_in_one_hot = (one_hot[i] == 0.0f && one_hot[9 + i] == 0.0f);
        bool is_available = action_mask[i];
        assert(is_empty_in_one_hot == is_available);
        
        // A cell is occupied if exactly one channel has 1.0 at that position
        bool has_player1 = (one_hot[i] == 1.0f);
        bool has_player2 = (one_hot[9 + i] == 1.0f);
        bool is_occupied_in_one_hot = has_player1 || has_player2;
        assert(is_occupied_in_one_hot == !is_available);
        
        // Verify no cell has both players (should be impossible)
        assert(!(has_player1 && has_player2));
    }
    std::cout << "✓ One-hot state and action mask are perfectly consistent" << std::endl;
    
    // Test 3: Consistency between flattened state and one-hot state
    std::cout << "\n3. Testing consistency between flattened state and one-hot state..." << std::endl;
    for (int i = 0; i < 9; ++i) {
        float flattened_value = flattened[i];
        bool has_player1_one_hot = (one_hot[i] == 1.0f);
        bool has_player2_one_hot = (one_hot[9 + i] == 1.0f);
        bool is_empty_one_hot = (!has_player1_one_hot && !has_player2_one_hot);
        
        if (flattened_value == 1.0f) {
            // Player 1 in flattened should correspond to Player 1 channel in one-hot
            assert(has_player1_one_hot && !has_player2_one_hot);
        } else if (flattened_value == -1.0f) {
            // Player 2 in flattened should correspond to Player 2 channel in one-hot
            assert(!has_player1_one_hot && has_player2_one_hot);
        } else if (flattened_value == 0.0f) {
            // Empty in flattened should correspond to both channels being 0 in one-hot
            assert(is_empty_one_hot);
        } else {
            // Flattened values should only be -1, 0, or 1
            assert(false);
        }
    }
    std::cout << "✓ Flattened state and one-hot state are perfectly consistent" << std::endl;
    
    // Test 4: State evolution consistency across all representations
    std::cout << "\n4. Testing state evolution consistency across game progression..." << std::endl;
    env.reset();
    
    // Verify initial state consistency
    std::vector<float> initial_flattened = env.get_flattened_state();
    std::vector<float> initial_one_hot = env.get_one_hot_state();
    std::vector<bool> initial_mask = env.get_action_mask();
    
    // All should indicate empty board
    for (int i = 0; i < 9; ++i) {
        assert(initial_flattened[i] == 0.0f);
        assert(initial_one_hot[i] == 0.0f);
        assert(initial_one_hot[9 + i] == 0.0f);
        assert(initial_mask[i] == true);
    }
    
    // Test consistency after each move in a sequence
    std::vector<int> moves = {1, 3, 5, 7, 0, 2, 4, 6, 8};  // Sequence that fills board
    for (int move_idx = 0; move_idx < moves.size(); ++move_idx) {
        try {
            env.step(Action{moves[move_idx]});
            
            std::vector<float> current_flattened = env.get_flattened_state();
            std::vector<float> current_one_hot = env.get_one_hot_state();
            std::vector<bool> current_mask = env.get_action_mask();
            
            // Verify consistency for each cell
            for (int i = 0; i < 9; ++i) {
                float flat_val = current_flattened[i];
                bool has_p1_oh = (current_one_hot[i] == 1.0f);
                bool has_p2_oh = (current_one_hot[9 + i] == 1.0f);
                bool is_available = current_mask[i];
                
                // Consistency checks
                if (flat_val == 0.0f) {
                    assert(!has_p1_oh && !has_p2_oh && is_available);
                } else if (flat_val == 1.0f) {
                    assert(has_p1_oh && !has_p2_oh && !is_available);
                } else if (flat_val == -1.0f) {
                    assert(!has_p1_oh && has_p2_oh && !is_available);
                }
            }
        } catch (const std::exception& e) {
            // Game may end before board is full due to wins - this is expected
            break;
        }
    }
    std::cout << "✓ All representations remain consistent throughout game progression" << std::endl;
    
    // Test 5: Cross-validation using different board sizes
    std::cout << "\n5. Testing cross-validation with different board sizes..." << std::endl;
    std::vector<int> board_sizes = {2, 4, 5};
    
    for (int N : board_sizes) {
        auto reward_fn_N = std::make_shared<DefaultReward>();
        Environment env_N(N, reward_fn_N);
        env_N.reset();
        
        std::vector<float> flat_N = env_N.get_flattened_state();
        std::vector<float> one_hot_N = env_N.get_one_hot_state();
        std::vector<bool> mask_N = env_N.get_action_mask();
        
        // Verify sizes
        assert(flat_N.size() == N * N);
        assert(one_hot_N.size() == 2 * N * N);
        assert(mask_N.size() == N * N);
        
        // Test with a few moves
        int max_moves = std::min(3, N * N);
        for (int move = 0; move < max_moves; ++move) {
            env_N.step(Action{move});
            
            std::vector<float> current_flat = env_N.get_flattened_state();
            std::vector<float> current_oh = env_N.get_one_hot_state();
            std::vector<bool> current_mask = env_N.get_action_mask();
            
            // Verify consistency for this board size
            for (int i = 0; i < N * N; ++i) {
                bool is_empty = (current_flat[i] == 0.0f);
                bool is_empty_oh = (current_oh[i] == 0.0f && current_oh[N*N + i] == 0.0f);
                bool is_available = current_mask[i];
                
                assert(is_empty == is_empty_oh);
                assert(is_empty == is_available);
            }
        }
    }
    std::cout << "✓ All representations work consistently across different board sizes" << std::endl;
    
    // Test 6: Action mask validity for actual gameplay
    std::cout << "\n6. Testing action mask validity for preventing invalid moves..." << std::endl;
    env.reset();
    
    for (int move_count = 0; move_count < 5; ++move_count) {
        std::vector<bool> mask = env.get_action_mask();
        
        // Find first available action
        int valid_action = -1;
        for (int i = 0; i < 9; ++i) {
            if (mask[i]) {
                valid_action = i;
                break;
            }
        }
        
        assert(valid_action != -1);  // Should always find a valid action in first 5 moves
        
        // This move should succeed
        env.step(Action{valid_action});
        
        // Verify the cell is now marked as unavailable
        std::vector<bool> updated_mask = env.get_action_mask();
        assert(updated_mask[valid_action] == false);
    }
    std::cout << "✓ Action mask correctly guides valid gameplay" << std::endl;
    
    // Test 7: One-hot state interpretability and channel separation
    std::cout << "\n7. Testing one-hot state interpretability and channel separation..." << std::endl;
    env.reset();
    env.step(Action{0});  // Player 1
    env.step(Action{1});  // Player 2  
    env.step(Action{2});  // Player 1
    env.step(Action{3});  // Player 2
    
    std::vector<float> oh_test = env.get_one_hot_state();
    
    // Extract channels
    std::vector<float> player1_channel(oh_test.begin(), oh_test.begin() + 9);
    std::vector<float> player2_channel(oh_test.begin() + 9, oh_test.end());
    
    // Verify channel contents
    assert(player1_channel[0] == 1.0f && player1_channel[2] == 1.0f);  // Player 1 at positions 0,2
    assert(player2_channel[1] == 1.0f && player2_channel[3] == 1.0f);  // Player 2 at positions 1,3
    
    // Verify no overlap
    for (int i = 0; i < 9; ++i) {
        assert(!(player1_channel[i] == 1.0f && player2_channel[i] == 1.0f));
    }
    
    // Count total pieces
    int p1_count = 0, p2_count = 0;
    for (int i = 0; i < 9; ++i) {
        if (player1_channel[i] == 1.0f) p1_count++;
        if (player2_channel[i] == 1.0f) p2_count++;
    }
    assert(p1_count == 2 && p2_count == 2);  // 2 moves each
    
    std::cout << "✓ One-hot state channels are properly separated and interpretable" << std::endl;
    
    // Test 8: Full game simulation with all representations tracking
    std::cout << "\n8. Testing full game simulation with consistent state tracking..." << std::endl;
    env.reset();
    
    // Play a complete game and verify all representations stay consistent
    std::vector<int> game_moves = {4, 0, 1, 2, 7};  // Player 1 should win with this sequence
    for (int move_idx = 0; move_idx < game_moves.size(); ++move_idx) {
        int move = game_moves[move_idx];
        
        // Verify move is valid according to action mask
        std::vector<bool> pre_mask = env.get_action_mask();
        assert(pre_mask[move] == true);
        
        // Make the move
        StepResult result = env.step(Action{move});
        
        // Verify all representations are consistent after move
        std::vector<float> post_flat = env.get_flattened_state();
        std::vector<float> post_oh = env.get_one_hot_state();
        std::vector<bool> post_mask = env.get_action_mask();
        
        // The move should now be marked as unavailable
        assert(post_mask[move] == false);
        
        // Verify state representations are consistent
        for (int i = 0; i < 9; ++i) {
            bool is_empty_flat = (post_flat[i] == 0.0f);
            bool is_empty_oh = (post_oh[i] == 0.0f && post_oh[9 + i] == 0.0f);
            bool is_available = post_mask[i];
            
            assert(is_empty_flat == is_empty_oh);
            assert(is_empty_flat == is_available);
        }
        
        if (result.done) {
            std::cout << "  Game ended after " << (move_idx + 1) << " moves" << std::endl;
            break;
        }
    }
    std::cout << "✓ All representations remained consistent throughout complete game" << std::endl;
    
    // Test 9: Memory efficiency validation
    std::cout << "\n9. Testing memory efficiency and no data corruption..." << std::endl;
    env.reset();
    
    // Get initial states
    std::vector<float> state1_flat = env.get_flattened_state();
    std::vector<float> state1_oh = env.get_one_hot_state();
    std::vector<bool> state1_mask = env.get_action_mask();
    
    // Make a move
    env.step(Action{4});
    
    // Get updated states
    std::vector<float> state2_flat = env.get_flattened_state();
    std::vector<float> state2_oh = env.get_one_hot_state();
    std::vector<bool> state2_mask = env.get_action_mask();
    
    // Verify initial states haven't been corrupted
    for (int i = 0; i < 9; ++i) {
        assert(state1_flat[i] == 0.0f);  // Initial state should still be all zeros
        assert(state1_mask[i] == true);  // Initial mask should still be all true
    }
    for (int i = 0; i < 18; ++i) {
        assert(state1_oh[i] == 0.0f);  // Initial one-hot should still be all zeros
    }
    
    // Verify new states are correct
    assert(state2_flat[4] == 1.0f);  // Center should have Player 1
    assert(state2_oh[4] == 1.0f);    // Player 1 channel should have 1 at center
    assert(state2_mask[4] == false); // Center should be unavailable
    
    std::cout << "✓ No memory corruption detected, all representations independent" << std::endl;
    
    std::cout << "\n=== ALL EPIC 2 INTEGRATION TESTS PASSED! ===" << std::endl;
    std::cout << "✓ Flattened State Vector (US2.1) ✓" << std::endl;
    std::cout << "✓ One-Hot Encoding Option (US2.2) ✓" << std::endl;  
    std::cout << "✓ Action Masking (US2.3) ✓" << std::endl;
    std::cout << "✓ All three features work together seamlessly! ✓" << std::endl;
    
    return 0;
} 