# Test Architecture and Construction Guide

## Overview

This document provides comprehensive information about the test architecture for the Tic-Tac-Toe RL Environment project. It serves as a guide for future agents and developers to understand the test organization, design principles, and extension patterns.

## Test File Organization

The test suite is organized into three main files, each focusing on a specific Epic and functionality area:

### 1. `test_core_engine.cpp` - Epic 1: Core Environment Engine
**Purpose**: Tests fundamental game mechanics and environment functionality  
**Coverage**: US1.1 (Initialize Board), US1.2 (Apply Action & Transition), US1.3 (Terminal Detection)  
**Dependencies**: Only requires `environment.h` and basic C++ libraries  
**Test Count**: 13 tests (1 implicit board initialization + 5 action/transition + 7 terminal detection)

### 2. `test_state_representation.cpp` - Epic 2: State & Action Representation  
**Purpose**: Tests individual state representation methods in isolation  
**Coverage**: US2.1 (Flattened State Vector), US2.2 (One-Hot Encoding), US2.3 (Action Masking)  
**Dependencies**: Requires all Epic 1 functionality to be working  
**Test Count**: 17 tests (5 action masking + 6 flattened state + 6 one-hot encoding)

### 3. `test_integration.cpp` - Epic 2: State Representation Integration  
**Purpose**: Tests cross-feature validation and consistency between all state representations  
**Coverage**: Integration testing to ensure all Epic 2 features work together seamlessly  
**Dependencies**: Requires all Epic 1 and Epic 2 individual features to be working  
**Test Count**: 9 comprehensive integration tests

## Design Principles

### 1. **Epic-Based Organization**
Tests are grouped by Epic rather than by individual User Stories to provide logical cohesion and clear separation of concerns. This allows for:
- Independent testing of major functionality areas
- Clear dependency chains (Epic 1 → Epic 2 individual → Epic 2 integration)
- Easier debugging when specific areas fail

### 2. **Hierarchical Testing Approach**
```
Epic 1 (Core Engine) 
    ↓ (dependency)
Epic 2 Individual Features (State Representations)
    ↓ (dependency)
Epic 2 Integration (Cross-Feature Consistency)
```

### 3. **Comprehensive Coverage Strategy**
Each test file follows this pattern:
- **Unit Tests**: Test individual functions/methods
- **Edge Case Tests**: Test boundary conditions and error cases
- **Integration Tests**: Test how features work together
- **Scaling Tests**: Test behavior with different board sizes

### 4. **Self-Documenting Test Structure**
Each test includes:
- Clear console output describing what is being tested
- Descriptive test names explaining the purpose
- Inline comments explaining the test logic
- Assert statements with clear validation criteria

## Test Construction Patterns

### Standard Test Structure
```cpp
// Test X: Description of what's being tested
std::cout << "\nX. Testing [functionality]..." << std::endl;
[test setup code]
[test execution]
[assertions with clear validation]
std::cout << "✓ [Success message]" << std::endl;
```

### Common Testing Patterns

#### 1. **State Validation Pattern**
Used throughout to verify environment state:
```cpp
Environment env(3, reward_fn);
env.reset();
// Perform actions
BoardState state = env.get_current_state(); // or from StepResult
// Validate state matches expectations
assert(state.cells[i] == expected_value);
```

#### 2. **Exception Testing Pattern** 
Used for validation of error conditions:
```cpp
try {
    env.step(invalid_action);
    assert(false);  // Should not reach here
} catch (const std::invalid_argument& e) {
    // Expected exception caught
    std::cout << "✓ Exception correctly thrown" << std::endl;
}
```

#### 3. **Multi-Board Size Testing Pattern**
Used to ensure functionality scales properly:
```cpp
std::vector<int> board_sizes = {2, 4, 5};
for (int N : board_sizes) {
    Environment env_N(N, reward_fn_N);
    // Test functionality on this board size
}
```

#### 4. **Consistency Validation Pattern**
Used in integration tests to ensure representations align:
```cpp
std::vector<float> flat = env.get_flattened_state();
std::vector<bool> mask = env.get_action_mask();
for (int i = 0; i < N*N; ++i) {
    bool is_empty_flat = (flat[i] == 0.0f);
    bool is_available_mask = mask[i];
    assert(is_empty_flat == is_available_mask);
}
```

## Validation Criteria Standards

### Core Engine Tests (Epic 1)
- **Board State**: All cells have valid values {-1, 0, 1}
- **Player Alternation**: Strict alternation between Player 1 (1) and Player 2 (-1)
- **Action Validation**: Invalid actions throw `std::invalid_argument`
- **Terminal Detection**: All win patterns (horizontal, vertical, diagonal) correctly detected
- **Game Flow**: `done` flag correctly reflects game state

### State Representation Tests (Epic 2)
- **Size Validation**: All representations have correct dimensions (N², 2*N², N²)
- **Value Range**: Flattened state in {-1.0f, 0.0f, 1.0f}, one-hot in {0.0f, 1.0f}, action mask boolean
- **Empty Board**: All representations correctly show empty state
- **Player Encoding**: Player 1 → 1.0f/first channel, Player 2 → -1.0f/second channel
- **Spatial Preservation**: Board layout correctly preserved in all representations

### Integration Tests (Epic 2)
- **Cross-Consistency**: Empty cells consistent across all representations
- **Player Mapping**: Player pieces correctly mapped across representations
- **Evolution Consistency**: All representations evolve consistently throughout game
- **Memory Independence**: No data corruption between representation calls
- **Scale Validation**: Consistency maintained across different board sizes

## Extension Guidelines for Future Agents

### Adding New Tests

#### 1. **Adding Tests to Existing Files**
When adding tests for existing functionality:
```cpp
// Follow the numbering sequence
std::cout << "\nX. Testing [new functionality]..." << std::endl;
// Use consistent assertion patterns
// Add success message
std::cout << "✓ [New functionality] works correctly" << std::endl;
```

#### 2. **Adding New Test Files**
When implementing new Epics:
1. Create new test file: `test_[epic_name].cpp`
2. Update `CMakeLists.txt` with new executable
3. Follow existing file structure and patterns
4. Update README.md with new test documentation
5. Update this architecture document

#### 3. **Test Dependencies**
When adding tests that depend on other functionality:
- Place higher-level tests in later files (following dependency chain)
- Use integration files for cross-feature validation
- Ensure tests can run independently when possible

### Common Extension Scenarios

#### Adding New State Representation Methods
1. Add unit tests to `test_state_representation.cpp`
2. Add integration tests to `test_integration.cpp` for consistency validation
3. Test scaling with different board sizes
4. Validate consistency with existing representations

#### Adding New Game Mechanics
1. Add basic functionality tests to `test_core_engine.cpp`
2. If mechanics affect state representation, add integration tests
3. Ensure terminal detection logic handles new mechanics

#### Adding New Environment Features
1. Determine appropriate test file based on Epic mapping
2. Follow established testing patterns
3. Add both positive and negative test cases
4. Include edge case testing

## Build System Integration

### CMakeLists.txt Pattern
```cmake
add_executable(test_[name] tests/test_[name].cpp)
target_link_libraries(test_[name] env_core)
```

### Running Tests
Tests are designed to be run independently:
```bash
./test_core_engine        # Foundation tests
./test_state_representation  # Individual feature tests  
./test_integration        # Cross-feature validation
```

## Debugging and Troubleshooting

### Test Failure Analysis
1. **Core Engine Failures**: Usually indicate fundamental game logic issues
2. **State Representation Failures**: May indicate implementation bugs in specific representation methods
3. **Integration Failures**: Often indicate consistency issues between representations

### Common Debugging Patterns
1. **Verbose Output**: Tests include detailed console output for debugging
2. **Assertion Messages**: Use descriptive assertion failures
3. **State Printing**: Add temporary state printing for complex debugging
4. **Incremental Testing**: Run tests in dependency order

### Performance Considerations
- Tests use simple 3x3 boards for speed
- Larger board tests (4x4, 5x5) used sparingly for scaling validation
- Integration tests may be slower due to comprehensive validation

## Future Evolution

This test architecture is designed to scale with the project:

### Short Term (Epics 3-4)
- Add `test_reward_system.cpp` for Epic 3
- Add `test_rl_integration.cpp` for Epic 4
- Maintain current patterns and consistency

### Medium Term (Epics 5-6)
- Add `test_llm_integration.cpp` for Epic 5
- Add `test_gpu_acceleration.cpp` for Epic 6
- May require new testing patterns for asynchronous/GPU code

### Long Term (Epics 7-9)
- Add configuration and serialization tests
- Add comprehensive system integration tests
- Consider performance benchmarking tests

## Best Practices Summary

1. **Follow Epic-based organization** - Keep tests logically grouped
2. **Maintain dependency chain** - Core → Individual Features → Integration
3. **Use consistent patterns** - Follow established test structure and validation criteria
4. **Include comprehensive validation** - Test both positive and negative cases
5. **Document thoroughly** - Update README and this guide when adding tests
6. **Ensure independence** - Tests should be able to run in isolation
7. **Validate scaling** - Include different board sizes where appropriate
8. **Maintain consistency** - Integration tests are crucial for multi-representation systems

This architecture provides a solid foundation for comprehensive testing while remaining maintainable and extensible as the project grows. 