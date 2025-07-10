# Data Model for NxN Tic-Tac-Toe RL Environment

## Entities and Relationships

| Entity | Description | Relationships |
|--------|-------------|---------------|
| BoardState | Represents the current state of the Tic-Tac-Toe board | Standalone |
| Action | A discrete move on the board (0..N×N−1) | Produced by Agent |
| StepResult | Outcome of a step: next state, reward, done flag | Contains BoardState, reward, done flag |
| Episode | Sequence of StepResult entries for one playthrough | Aggregates StepResults |
| Agent | Learner using an RL algorithm (e.g., PPO or GRPO) | Consumes BoardState, emits Action |
| RewardCallback | User-provided reward function interface | Invoked by Environment at each step |
| LLMRequest | Payload sent when querying an LLM (prompt, state, options) | Triggered by Agent or Environment |
| KernelBatch | Batch of states or rewards for GPU kernel execution | Processed by GPUAccelerator |
| Config | Hyperparameters and environment settings | Consumed by all modules |

## Class Definitions (C++)

struct BoardState {
    std::vector<int> cells;  // size N*N: 0=empty, 1=player1, -1=player2
    int N;
    bool operator==(const BoardState &other) const;
};

struct Action {
    int index; // Valid range: 0..N*N-1
};

struct StepResult {
    BoardState next_state;
    float reward;
    bool done;
};

class Environment {
public:
    Environment(int N, std::shared_ptr<RewardCallback> reward_fn);
    BoardState reset();
    StepResult step(const Action &action);
    std::vector<float> get_flattened_state() const;
};

class RewardCallback {
public:
    virtual float operator()(const BoardState &state, const Action &action) = 0;
};

class LLMConnector {
public:
    LLMConnector(const std::string &endpoint);
    std::string query(const std::string &prompt, const BoardState &state);
};

class GPUAccelerator {
public:
    GPUAccelerator(const Config &cfg);
    void launch_kernel(const KernelBatch &batch);
};

## Data Serialization Formats
	•	State: JSON object { "cells": [...], "N": <int> }
	•	Action: JSON integer representing the cell index
	•	StepResult: JSON lines, e.g.:

{ "next_state": { ... }, "reward": 1.0, "done": false }


	•	Episode: Sequence (array) of StepResult JSON objects

## Configuration Schema (YAML)

environment:
  N: 3
  reward_fn: DefaultReward
ppo:
  learning_rate: 0.0003
  gamma: 0.99
  clip_range: 0.2
grpo:
  step_size: 0.1
  trust_region: 0.02
llm:
  endpoint: "http://localhost:5000/llm"
cuda:
  batch_size: 128
  kernels:
    - name: batched_loss
      path: kernels/batched_loss.cu