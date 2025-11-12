#include "CommandProcessing.h"
#include "GameEngine.h"
#include "LoggingObserver.h"
#include <iostream>
#include <limits>  

void testCommandProcessor(){
    CommandProcessor processor = CommandProcessor();
    GameEngine engine = GameEngine();

    // needs an observer to write to the file
    logObserver observer;
    processor.attach(observer);
    // this code is currently un tested

    std::cout << "1. Command Line processing Test\n2. File processing test\nEnter choice: ";
    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (choice == 1) {
        std::cout << "Current state of engine: " << engine.getState() << std::endl;
        for (int i = 0; i < 5; i++){
            std::string command = processor.getCommand(engine.getState());
            std::cout << "Processing command: " << command << std::endl;
            bool transitioned = engine.transition(command);
            if (transitioned){
                std::cout << "State transitioned to: " << engine.getState() << std::endl;
            } else {
                std::cout << "State remains: " << engine.getState() << std::endl;
            }
        }
    }
    else if (choice == 2) {
        FileCommandProcessorAdapter fileProcessor = FileCommandProcessorAdapter("commands.txt");
        // attach an observer here too
        fileProcessor.attach(observer);
        std::cout << "Current state of engine: " << engine.getState() << std::endl;
        for (int i = 0; i < 5; i++) {
            std::string command = fileProcessor.getCommand(engine.getState());
            std::cout << command << std::endl;
            if (command.empty()) break;  // Exit if no more commands in file
            
            std::cout << "Processing command: " << command << std::endl;
            bool transitioned = engine.transition(command);
            
            if (transitioned) {
                std::cout << "State changed to: " << engine.getState() << std::endl;
            } else {
                std::cout << "State remains: " << engine.getState() << std::endl;
            }
        }
    }
}

