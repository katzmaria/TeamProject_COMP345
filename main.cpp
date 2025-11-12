#include <iostream>
#include <string>
#include "GameEngine.h"

// Forward declarations for all test functions
void testLoadMaps();       // Part 1: Map
void testStartupPhase();   // Part 2: Game Startup Phase
void testMainGameLoop();   // Part 3: Main Game Loop
void testOrderExecution(); // Part 4: Order Execution
void testCards();          // Part 5: Cards
void testOrdersLists();    // Additional: Orders Lists
void testPlayers();        // Additional: Players
void testCommandProcessor(); // Additional: Command Processing

int main(int argc, char* argv[]) {
    // Check for command line argument --test
    bool runAllTests = false;
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "--test" || arg == "-test") {
            runAllTests = true;
        }
    }
    
    if (runAllTests) {
        // Run all 5 parts for assignment submission
        std::cout << "\n========================================\n";
        std::cout << "COMP 345 - Assignment 2\n";
        std::cout << "Running All Tests\n";
        std::cout << "========================================\n\n";
        
        std::cout << "\n=== PART 1: Map ===\n";
        testLoadMaps();
        
        std::cout << "\n=== PART 2: Game Startup Phase ===\n";
        testStartupPhase();
        
        std::cout << "\n=== PART 3: Main Game Loop ===\n";
        testMainGameLoop();
        
        std::cout << "\n=== PART 4: Order Execution ===\n";
        testOrderExecution();
        
        std::cout << "\n=== PART 5: Cards ===\n";
        testCards();
        
        std::cout << "\n========================================\n";
        std::cout << "All Tests Complete\n";
        std::cout << "========================================\n\n";
        
    } else {
        // Interactive menu
        std::cout << "========================================\n";
        std::cout << "COMP 345 - Warzone Game\n";
        std::cout << "========================================\n";
        std::cout << "Select option:\n";
        std::cout << "1. Test Game Startup Phase (Part 2)\n";
        std::cout << "2. Test Order Execution (Part 4)\n";
        std::cout << "3. Play Full Game\n";
        std::cout << "Enter choice (1-3): ";
        
        int choice;
        std::cin >> choice;
        
        switch (choice) {
            case 1:
                testStartupPhase();
                break;
                
            case 2:
                testOrderExecution();
                break;
                
            case 3:
                std::cout << "\n=== Starting Full Game ===\n";
                testMainGameLoop();
                break;
                
            default:
                std::cout << "Invalid choice.\n";
                break;
        }
    }
    
    return 0;
}

