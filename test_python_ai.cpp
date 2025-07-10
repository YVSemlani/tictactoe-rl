#include "include/game_session.h"
#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Python AI Integration Test ===" << std::endl;
    
#ifdef WITH_PYTHON_AI
    std::cout << "Python AI support is ENABLED" << std::endl;
    
    try {
        // Create a 3x3 game session
        GameSession session(3);
        
        // Test Python Random AI vs Python Strategy AI
        std::cout << "\nCreating Python AI players..." << std::endl;
        auto python_random = session.create_ai_player("PyRandom", "PythonRandom");
        auto python_strategy = session.create_ai_player("PyStrategy", "PythonStrategy");
        
        session.set_players(python_random, python_strategy);
        
        std::cout << "\nStarting game: Python Random vs Python Strategy" << std::endl;
        bool game_completed = session.play_single_game_with_result();
        
        if (game_completed) {
            std::cout << "Game completed successfully!" << std::endl;
            
            // Show statistics
            const SessionStats& stats = session.get_statistics();
            std::cout << "\nGame Statistics:" << std::endl;
            std::cout << "Games played: " << stats.games_played << std::endl;
            std::cout << "Player 1 wins: " << stats.player1_wins << std::endl;
            std::cout << "Player 2 wins: " << stats.player2_wins << std::endl;
            std::cout << "Draws: " << stats.draws << std::endl;
        } else {
            std::cout << "Game did not complete properly" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error during Python AI test: " << e.what() << std::endl;
        return 1;
    }
    
#else
    std::cout << "Python AI support is DISABLED" << std::endl;
    std::cout << "To enable Python AI support:" << std::endl;
    std::cout << "1. Install pybind11: pip install pybind11" << std::endl;
    std::cout << "2. Rebuild with: mkdir build && cd build && cmake .. && make" << std::endl;
#endif
    
    std::cout << "\n=== Test Complete ===" << std::endl;
    return 0;
} 