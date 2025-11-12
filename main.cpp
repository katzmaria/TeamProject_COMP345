#include <iostream>

void testMainGameLoop();
void testOrderExecution();

int main() {
    std::cout << "========================================\n";
    std::cout << "COMP 345 - Warzone Game\n";
    std::cout << "========================================\n";
    std::cout << "Select test to run:\n";
    std::cout << "1. Test Order Execution (Part 4)\n";
    std::cout << "2. Test Main Game Loop\n";
    std::cout << "Enter choice (1 or 2): ";
    
    int choice;
    std::cin >> choice;
    
    if (choice == 1) {
        testOrderExecution();
    } else if (choice == 2) {
        testMainGameLoop();
    } else {
        std::cout << "Invalid choice. Running main game loop by default.\n";
        testMainGameLoop();
    }
    
    return 0;
}

