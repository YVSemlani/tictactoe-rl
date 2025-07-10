# Sprint Goal

“By the end of this hour, have a runnable C++ project scaffold that can reset an NxN board, perform one step with a stubbed reward callback, and build via CMake.”

⸻


| Time | Activity | Description & Deliverables |
|------|----------|----------------------------|
| 0–5’ | Sprint Planning | - Review sprint goal & scope.<br>- Identify core user stories:<br>  1. As a developer, I can reset() and step() the environment.<br>  2. As a developer, I can plug in a RewardCallback stub.<br>  3. As a developer, I can build & run via CMake. |
| 5–15’ | Repo & Build Setup | - Initialize Git repo.<br>- Create CMakeLists.txt with targets:<br>  - env_core library<br>  - env_test executable.<br>- Commit “scaffold” snapshot. |
| 15–30’ | Core Engine Implementation | - In Environment class: implement reset() (zeroed board) and step(Action) that:<br>   - Updates BoardState.<br>  - Calls RewardCallback stub (returns 0).<br>  - Sets done = false always.<br>- Write minimal unit test to call reset() and step(). |
| 30–40’ | RewardCallback Integration | - Define abstract RewardCallback interface.<br>- Provide one stub implementation (DefaultReward) that always returns 0.<br>- In env_test, demonstrate swapping in the stub. |
| 40–50’ | CMake & CI Hook | - Ensure CMake builds both library and test.<br>- (Optional) Add a simple GitHub Actions workflow stub to compile on push. |
| 50–55’ | Demo & Validation | - Run env_test to show board state changes and zero reward.<br>- Fix any build/test breakages. |
| 55–60’ | Sprint Review & Retro | - Review: Confirm sprint goal achieved:<br>  - Can reset & step, stub reward, build.<br>- Retro (2–3 points):<br>  1. What went well?<br>  2. What to improve?<br>  3. Action item for next sprint. |

⸻

Quick Roles & Tips
	•	Developer: You’ll wear all hats—architect, coder, tester, reviewer.
	•	Timekeeper: Use a timer (or Pomodoro app) to stick to each slot.
	•	Definition of Done: Green build + one passing test + stub reward.
