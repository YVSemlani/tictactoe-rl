# Backlog

## Epic 1: Core Environment Engine

Goal: Provide a C++-based, Gym-style RL environment for NxN Tic-Tac-Toe.
	1.	US1.1: Initialize Board
	•	As a developer, I want the environment to initialize an empty NxN board on reset() so that training always starts from a clean state.
	•	Acceptance:
	•	reset() returns a BoardState with all cells = 0.
	•	BoardState.N equals the configured board size.
	2.	US1.2: Apply Action & Transition
	•	As a developer, I want step(Action) to update the board and return a StepResult so that agents can interact with the environment.
	•	Acceptance:
	•	Applying a valid action sets the corresponding cell to the current player.
	•	StepResult.next_state reflects the new board.
	•	Invalid actions throw an exception or return a defined error code.
	3.	US1.3: Terminal Detection
	•	As a developer, I want the engine to detect wins, losses, and draws in step() so that episodes terminate correctly.
	•	Acceptance:
	•	Horizontal, vertical, diagonal N-in-a-row is detected as win/loss.
	•	Full board with no winner returns done = true, reward = 0.

## Epic 2: State & Action Representation

Goal: Expose board state and actions in formats usable by RL agents and LLMs.
	1.	US2.1: Flattened State Vector
	•	As a researcher, I want get_flattened_state() to return a 1D float vector so that I can feed it into neural networks.
	•	Acceptance:
	•	Length = N×N.
	•	Values in {–1, 0, +1} or normalized.
	2.	US2.2: One-Hot Encoding Option
	•	As an agent developer, I want an option to receive a one-hot encoded state tensor so that multi-channel models can be used.
	•	Acceptance:
	•	Two channels: one for each player.
	•	Shape = [2, N, N].
	3.	US2.3: Action Masking
	•	As a policy implementer, I want an action mask indicating legal moves so that the agent doesn’t select invalid actions.
	•	Acceptance:
	•	Returns a boolean vector length N×N with true on empty cells.

## Epic 3: Reward System & Custom Callbacks

Goal: Support default and user-provided reward logic.
	1.	US3.1: Default Win/Loss/Draw Rewards
	•	As a user, I want default rewards (+1, –1, 0) so I can get basic signal out of the box.
	•	Acceptance:
	•	Win → +1, Loss → –1, Draw → 0.
	2.	US3.2: Pluggable RewardCallback Interface
	•	As an advanced user, I want to inject my own RewardCallback functor so that I can define custom reward shaping.
	•	Acceptance:
	•	Environment constructor accepts std::shared_ptr<RewardCallback>.
	•	Callback invoked on each step().
	3.	US3.3: Python-Backed Reward Hooks
	•	As a Python user, I want to write reward functions in Python and bind via pybind11 so I can iterate faster on reward design.
	•	Acceptance:
	•	Python callback can be registered and invoked each step.
	•	No significant performance regression when using C++ callback.

## Epic 4: RL Algorithm Integration

Goal: Provide templates and examples for PPO and GRPO in C++.
	1.	US4.1: PPO Module Skeleton
	•	As a developer, I want a C++ PPO class template so I can plug in the environment state and action interfaces.
	•	Acceptance:
	•	Implements policy/value networks (e.g., via Eigen or libtorch).
	•	Exposes collect_trajectories() and update() methods.
	2.	US4.2: GRPO Module Skeleton
	•	As a developer, I want a GRPO template with trust-region updates so I can compare algorithms side by side.
	•	Acceptance:
	•	Implements conjugate-gradient trust-region solver.
	•	Configurable TR radius.
	3.	US4.3: Hyperparameter Configuration
	•	As an engineer, I want to read alpha, gamma, batch size, etc., from YAML so I can tune experiments without recompiling.
	•	Acceptance:
	•	YAML parser loads into a Config struct.
	•	Both PPO/GRPO modules consume the same config.

## Epic 5: LLM Interaction

Goal: Let the environment or agent query an LLM for move suggestions or commentary.
	1.	US5.1: HTTP LLM Connector
	•	As a user, I want LLMConnector to send JSON payloads to a REST endpoint so I can use any hosted LLM.
	•	Acceptance:
	•	Sends {"prompt": "...", "state": {...}} and parses a text response.
	2.	US5.2: gRPC LLM Connector (Optional)
	•	As an enterprise integrator, I want gRPC support so I can leverage low-latency LLM services.
	•	Acceptance:
	•	Supports proto-defined request/response.
	•	Fallback to HTTP if gRPC fails.
	3.	US5.3: Move Suggestion Story
	•	As a human-in-the-loop tester, I want to call query_llm() mid-game to get a suggested action so I can compare against policy.
	•	Acceptance:
	•	Returns a valid Action.
	•	LLM latency is logged but does not crash the environment.

## Epic 6: GPU Acceleration & Custom Kernels

Goal: Batch-process states/rewards on GPU for speed.
	1.	US6.1: KernelBatch Abstraction
	•	As a performance engineer, I want a KernelBatch type to collect arrays of states or reward deltas so I can launch batched kernels.
	•	Acceptance:
	•	Holds contiguous device memory pointers.
	•	Can be reused across iterations.
	2.	US6.2: Example CUDA Kernel
	•	As a GPU dev, I want a sample CUDA kernel that computes batched reward sums so I have a template for more complex ops.
	•	Acceptance:
	•	Kernel compiles without errors.
	•	Unit test verifies correctness vs. CPU version.
	3.	US6.3: Dynamic Kernel Loading
	•	As an advanced user, I want to load .cu files at runtime via NVRTC so I can experiment with kernels without recompiling the main binary.
	•	Acceptance:
	•	Supports NVRTC compile and launch.
	•	Errors are caught and surfaced gracefully.

## Epic 7: Configuration & Serialization

Goal: Persist and exchange environment data and settings.
	1.	US7.1: JSON State & Episode Logging
	•	As a researcher, I want to log every StepResult in JSONL so I can analyze runs offline.
	•	Acceptance:
	•	Each line is a valid JSON object.
	•	Schema includes timestamp, state, reward, done.
	2.	US7.2: YAML Config Loader
	•	As a devops engineer, I want a single YAML file to configure env size, RL hyperparams, LLM endpoint, and CUDA settings.
	•	Acceptance:
	•	Missing fields fallback to sensible defaults.
	•	Validation errors during startup.

## Epic 8: Testing, CI & Quality

Goal: Ensure reliability through automated tests and CI.
	1.	US8.1: Unit Tests for Core Engine
	•	As a maintainer, I want GoogleTest cases covering board reset, step transitions, and terminal conditions.
	•	Acceptance:
	•	≥90% coverage on core engine code.
	2.	US8.2: Integration Tests for PPO/GRPO
	•	As a QA engineer, I want a smoke test that runs 100 episodes of PPO on 3×3 and reports a non-crash.
	•	Acceptance:
	•	Test passes in under 30 seconds on CI.
	3.	US8.3: CI Pipeline
	•	As a team, I want GitHub Actions to build, test, and lint on every PR so that regressions are caught early.
	•	Acceptance:
	•	Workflow triggers on push/PR.
	•	Fails fast on build or test errors.

## Epic 9: Documentation & Examples

Goal: Provide clear guides, tutorials, and demos.
	1.	US9.1: README & Quickstart
	•	As a new user, I want a README showing how to clone, build, run a basic loop so I can get started immediately.
	•	Acceptance:
	•	Contains code snippets for CMake build and sample run.
	2.	US9.2: Jupyter-Style Tutorial
	•	As an educator, I want a notebook (via pybind11) that steps through environment usage and simple training so I can teach students.
	•	Acceptance:
	•	Notebook runs end-to-end in <5 minutes.
	3.	US9.3: API Reference
	•	As an integrator, I want Doxygen or Sphinx-generated docs for all public classes and methods so I can integrate without reading source.
	•	Acceptance:
	•	Published site link.
	•	≥80% of public methods documented.