# Tic Tac Toe RL Environment 

Implemented as part of an AI tools training during my summer 2025 ML engineering internship at Careset.

## Goals

I wanted to explore a few skills with this project:
- C++ implementation of a quick game environment
- Using OSS LLMs to interact with real world environments
- Implementing verifiers/reward functions to post-train model
- Working with RL algorithms like GRPO, PPO, etc.
- Writing custom kernels to speed up various parts of the training process

Beyond these individual skills, tying them all together helped to understand integration challenges with creating real world ML for games pipelines.

## Tests

The project includes comprehensive test suites organized by functionality and Epic. Each test file can be built and run independently to verify specific components.

### test_core_engine.cpp - Epic 1: Core Environment Engine

Tests the fundamental game mechanics and environment functionality.

#### US1.1: Initialize Board (implicit testing)
- **Board Initialization Test**: Verifies that `reset()` creates an empty NxN board with all cells set to 0 and correct dimensions.
  - *How it tests*: Calls `reset()` and checks that `state.cells.size() == N*N`, `state.N == 3`, and all cells are 0
  - *Validation*: Board has correct size and is completely empty

#### US1.2: Apply Action & Transition  
- **Basic Reset and Step Test**: Validates basic environment operation
  - *How it tests*: Resets environment, takes one action, verifies cell update and reward/done state
  - *Validation*: Cell correctly shows Player 1 (value 1), reward is 0, game not done

- **Player Alternation Test**: Ensures players alternate correctly
  - *How it tests*: Takes two consecutive actions, verifies first cell has Player 1 (1), second has Player 2 (-1)
  - *Validation*: Players alternate as expected

- **Bounds Validation Test**: Verifies rejection of out-of-bounds actions
  - *How it tests*: Attempts action with index 9 on 3x3 board (valid: 0-8), expects exception
  - *Validation*: `std::invalid_argument` thrown for invalid action

- **Occupied Cell Validation Test**: Ensures occupied cells cannot be overwritten
  - *How it tests*: Places piece, attempts to place another at same location, expects exception
  - *Validation*: `std::invalid_argument` thrown for occupied cell

- **StepResult State Reflection Test**: Verifies returned state matches board changes
  - *How it tests*: Takes action at center, checks that `StepResult.next_state` shows Player 1 at center, all other cells empty
  - *Validation*: Returned state accurately reflects board changes

#### US1.3: Terminal Detection
- **Horizontal Win Detection Test**: Validates horizontal line wins
  - *How it tests*: Creates winning pattern X-X-X on top row, verifies `done == true`
  - *Validation*: Game correctly terminates on horizontal win

- **Vertical Win Detection Test**: Validates vertical line wins  
  - *How it tests*: Creates winning pattern in left column, verifies `done == true`
  - *Validation*: Game correctly terminates on vertical win

- **Main Diagonal Win Detection Test**: Validates main diagonal wins (top-left to bottom-right)
  - *How it tests*: Creates winning pattern on main diagonal, verifies `done == true`
  - *Validation*: Game correctly terminates on diagonal win

- **Anti-Diagonal Win Detection Test**: Validates anti-diagonal wins (top-right to bottom-left)
  - *How it tests*: Creates winning pattern on anti-diagonal, verifies `done == true`
  - *Validation*: Game correctly terminates on anti-diagonal win

- **Player 2 Win Detection Test**: Ensures Player 2 wins are detected
  - *How it tests*: Creates scenario where Player 2 wins horizontally, verifies `done == true`
  - *Validation*: Game correctly terminates when Player 2 wins

- **Draw Detection Test**: Validates draw conditions when board is full with no winner
  - *How it tests*: Fills board in specific pattern with no winner, verifies `done == true`
  - *Validation*: Game correctly terminates on draw

- **Game Continuation Test**: Ensures game continues when no terminal condition is met
  - *How it tests*: Takes a few moves with no winning pattern, verifies `done == false`
  - *Validation*: Game correctly continues when no terminal condition

### test_state_representation.cpp - Epic 2: State & Action Representation

Tests the three state representation methods individually to ensure each works correctly.

#### US2.3: Action Masking
- **Empty Board Action Mask Test**: Validates mask on fresh board
  - *How it tests*: Calls `get_action_mask()` on empty board, verifies all cells are `true`
  - *Validation*: All 9 cells available on empty 3x3 board

- **Single Move Action Mask Test**: Validates mask after one move
  - *How it tests*: Places piece at center, checks mask shows center as `false`, others as `true`
  - *Validation*: Occupied cell marked unavailable, empty cells available

- **Multiple Moves Action Mask Test**: Validates mask with several occupied cells
  - *How it tests*: Places multiple pieces, verifies only occupied cells marked `false`
  - *Validation*: All and only occupied cells marked unavailable

- **Nearly Full Board Action Mask Test**: Validates mask when board almost full
  - *How it tests*: Fills 8 of 9 cells, verifies only last cell marked `true`
  - *Validation*: Only empty cell available

- **Different Board Sizes Action Mask Test**: Validates mask scales with board size
  - *How it tests*: Tests 4x4 board, verifies mask size is 16 and all cells available on empty board
  - *Validation*: Mask size correctly matches board size (N²)

#### US2.1: Flattened State Vector
- **Empty Board Flattened State Test**: Validates empty board representation
  - *How it tests*: Calls `get_flattened_state()` on empty board, verifies all values are 0.0f
  - *Validation*: Vector size is N² and all values are 0.0f

- **Single Move Flattened State Test**: Validates state after one move
  - *How it tests*: Places Player 1 piece, verifies correct cell is 1.0f, others are 0.0f
  - *Validation*: Player 1 piece correctly represented as 1.0f

- **Both Players Flattened State Test**: Validates state with both players' pieces
  - *How it tests*: Places multiple pieces from both players, verifies Player 1 cells are 1.0f, Player 2 cells are -1.0f
  - *Validation*: Players correctly represented as +1.0f and -1.0f

- **Value Range Validation Test**: Ensures all values in valid range
  - *How it tests*: Creates complex board state, verifies all values are in {-1.0f, 0.0f, 1.0f}
  - *Validation*: No values outside expected range

- **Different Board Sizes Flattened State Test**: Validates scaling with board size
  - *How it tests*: Tests 5x5 board, verifies state size is 25 and all values 0.0f on empty board
  - *Validation*: State size correctly matches board size (N²)

- **Board Layout Preservation Test**: Validates state preserves spatial relationships
  - *How it tests*: Creates specific pattern, verifies flattened state matches expected layout
  - *Validation*: Spatial relationships correctly preserved in 1D representation

#### US2.2: One-Hot Encoding Option
- **Empty Board One-Hot Test**: Validates one-hot encoding of empty board
  - *How it tests*: Calls `get_one_hot_state()` on empty board, verifies all values are 0.0f
  - *Validation*: Vector size is 2*N² and all channels empty

- **Player 1 Move One-Hot Test**: Validates encoding of Player 1 move
  - *How it tests*: Places Player 1 piece, verifies Player 1 channel has 1.0f at correct position, Player 2 channel empty
  - *Validation*: Player 1 piece correctly encoded in first channel

- **Player 2 Move One-Hot Test**: Validates encoding of Player 2 move  
  - *How it tests*: After Player 1 move, places Player 2 piece, verifies both channels correctly populated
  - *Validation*: Player 2 piece correctly encoded in second channel

- **Multiple Moves One-Hot Test**: Validates encoding with multiple pieces from both players
  - *How it tests*: Places multiple pieces, verifies both channels have correct patterns
  - *Validation*: Complex patterns correctly encoded in separate channels

- **Different Board Sizes One-Hot Test**: Validates scaling with board size
  - *How it tests*: Tests 2x2 and 5x5 boards, verifies encoding sizes are 8 and 50 respectively
  - *Validation*: Encoding size correctly scales as 2*N²

- **Channel Organization Test**: Validates proper channel separation
  - *How it tests*: Places pieces from both players, verifies channel offsets and no overlap
  - *Validation*: Channels properly organized with correct offsets

### test_integration.cpp - Epic 2: State Representation Integration

Tests that all three state representation methods work together seamlessly and remain consistent.

#### Cross-Representation Consistency Tests
- **Flattened State and Action Mask Consistency Test**: Validates alignment between representations
  - *How it tests*: Compares empty cells in flattened state (0.0f) with available cells in action mask (`true`)
  - *Validation*: Empty/occupied status identical across representations

- **One-Hot State and Action Mask Consistency Test**: Validates one-hot encoding aligns with action mask
  - *How it tests*: Verifies cells empty in both one-hot channels correspond to available actions
  - *Validation*: Complex one-hot encoding correctly reflects action availability

- **Flattened State and One-Hot State Consistency Test**: Validates encoding equivalence
  - *How it tests*: Compares cell values across both representations for all combinations
  - *Validation*: Player 1 (1.0f) maps to first channel, Player 2 (-1.0f) to second, empty (0.0f) to neither

#### State Evolution and Game Progression Tests  
- **State Evolution Consistency Test**: Validates consistency throughout game progression
  - *How it tests*: Plays sequence of moves, verifies all representations remain consistent after each move
  - *Validation*: All representations evolve consistently throughout game

- **Cross-Validation Different Board Sizes Test**: Validates consistency across board sizes
  - *How it tests*: Tests 2x2, 4x4, and 5x5 boards with multiple moves on each
  - *Validation*: All representations work consistently regardless of board size

- **Action Mask Validity for Gameplay Test**: Validates action mask prevents invalid moves
  - *How it tests*: Uses action mask to guide move selection, verifies moves succeed and update mask correctly
  - *Validation*: Action mask correctly guides valid gameplay

#### Advanced Integration Tests
- **One-Hot Interpretability and Channel Separation Test**: Validates channel organization and separation
  - *How it tests*: Extracts separate channels, verifies no overlap and correct piece counting
  - *Validation*: Channels properly separated and interpretable

- **Full Game Simulation Test**: Validates consistency throughout complete game
  - *How it tests*: Plays complete game to termination, verifying consistency at each step
  - *Validation*: All representations remain consistent throughout entire game

- **Memory Efficiency Validation Test**: Validates no data corruption between calls
  - *How it tests*: Captures states before/after moves, verifies previous states unchanged
  - *Validation*: No memory corruption, all representations independent

## Running Tests

To build and run the tests:

```bash
# Build all tests
mkdir build && cd build
cmake ..
make

# Run individual test suites
./test_core_engine        # Epic 1: Core Environment Engine
./test_state_representation  # Epic 2: State & Action Representation  
./test_integration        # Epic 2: Integration Tests

# Or run all tests
./test_core_engine && ./test_state_representation && ./test_integration
```

Each test executable provides detailed output showing which tests pass/fail and what functionality is being validated.