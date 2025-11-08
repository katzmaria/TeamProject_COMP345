#include "GameEngine.h"
#include <iostream>

// function only used to test the GameEngine class
void testGameStates(){
    // create a new game
    GameEngine game;

    // print the initial state
    std::cout << game << std::endl;

    // continue asking user for commands until they enter "end"
    while (true) {
        std::string command;
        std::cout << "Enter command to transition the game state: ";
        std::cin >> command;

        // notify user of changed state
        if (game.transition(command)) {
            std::cout << "Transitioned to state: " << game.getState() << std::endl;
            
            // if user is in the "win" state and enters "end", break the loop
            if (game.getState() == "end"){
                std::cout << "Game has ended.\n" << std::endl;
                break;
            }
        }
        // notify user of invalid command
        else {
            std::cout << "Invalid command for current state " << game.getState() << "\n" << std::endl;
        }
    }
}

// function to test the startup phase of the GameEngine: Part 2 of the assignment
void testStartupPhase() {
    std::cout << "=== Testing Startup Phase ===" << std::endl;

    GameEngine engine;
    engine.startupPhase();

    std::cout << "\nStartup phase complete. Current state: "
              << engine.getState() << std::endl;
}


