#include "GameEngine.h"
#include <string>
#include <iostream>

// default constructor will initialize the GameEngine object to the start state
GameEngine::GameEngine() {
    currentState = new std::string("start");
}

// copy constructor 
GameEngine::GameEngine(const GameEngine& engine){
    currentState = new std::string(*engine.currentState);
}

// Assignment operator
GameEngine& GameEngine::operator=(const GameEngine& engine){
    if (this != &engine){
        delete currentState;
        currentState = new std::string(*engine.currentState);
    }
    return *this;
}

// Stream insertion operator implementation
std::ostream& operator<<(std::ostream& out, const GameEngine& engine){
    out << "Current Game State: " << *engine.currentState;
    return out;
}

// destructor *(will have to be updated when we add more attributes to the class)
GameEngine::~GameEngine(){
    delete currentState;
}

// Setter method for currentState
void GameEngine::setState(const std::string& newState){
    delete currentState;
    currentState = new std::string(newState);
}

// Getter method for currentState
std::string GameEngine::getState() const {
    return *currentState;
}

