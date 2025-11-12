#include "GameEngine.h"
#include <iostream>

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
        // notify change of state
        if (game.transition(command)) {
            std::cout << "Transitioned to state: " << game.getState() << std::endl;
            // if user wins or ends the game, break the loop
            if (game.getState() == "end"){
                std::cout << "Game has ended.\n" << std::endl;
                break;
            }
        }
        else {
            std::cout << "Invalid command for current state " << game.getState() << "\n" << std::endl;
        }
    }
}

// function to test the startup phase
void testStartupPhase() {
    std::cout << "=== Testing Startup Phase ===" << std::endl;

    GameEngine engine;
    engine.startupPhase();

    std::cout << "\nStartup phase complete. Current state: "
              << engine.getState() << std::endl;
}


