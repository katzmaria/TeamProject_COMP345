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

// Transition method will change the state of the game based on the command passed in and the 
// current state of the game. 
bool GameEngine::transition(const std::string& command) {
    if (*currentState == "start" && command == "loadmap"){
        setState("maploaded");
        return true;
    }

    if (*currentState == "maploaded" && command == "validatemap"){
        setState("mapvalidated");
        return true;
    }
    else if (*currentState == "maploaded" && command == "loadmap") {
        return true; // remain in maploaded state
    }

    if (*currentState == "mapvalidated" && command == "addplayer") {
        setState("playersadded");
        return true;
    }

    if (*currentState == "playersadded" && command == "assigncountries") {
        setState("assignreinforcement");
        return true;
    }
    else if (*currentState == "playersadded" && command == "addplayer") {
        return true; // remain in playersadded state
    }

    if (*currentState == "assignreinforcement" && command == "issueorder"){
        setState("issueorders");
        return true;
    }

    if (*currentState == "issueorders" && command == "endissueorders"){
        setState("executeorders");
        return true;
    }
    else if (*currentState == "issueorders" && command == "issueorder") {
        return true; // remain in issueorders state
    }
    
    if (*currentState == "executeorders" && command == "endexecuteorders"){
        setState("assignreinforcement");
        return true;
    }
    else if (*currentState == "executeorders" && command == "executeorder") {
        return true; // remain in the executeorders state
    }
    else if (*currentState == "executeorders" && command == "win") {
        setState("win");
        return true;
    }

    if (*currentState == "win" && command == "end"){
        setState("end");
        return true;
    }
    else if (*currentState == "win" && command == "play"){
        setState("start");
        return true;
    }

}