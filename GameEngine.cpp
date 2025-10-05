#include "GameEngine.h"
#include <string>
#include <iostream>

// default constructor will initialize the GameEngine object to the start state
GameEngine::GameEngine() {
    currentState = new std::string("start");
    playerCount = 0;
}

// copy constructor 
GameEngine::GameEngine(const GameEngine& engine){
    currentState = new std::string(*engine.currentState);
    playerCount = engine.getPlayerCount();
}

// Assignment operator
GameEngine& GameEngine::operator=(const GameEngine& engine){
    if (this != &engine){
        delete currentState;
        currentState = new std::string(*engine.currentState);
        playerCount = engine.getPlayerCount();
    }
    return *this;
}

// Stream insertion operator implementation
std::ostream& operator<<(std::ostream& out, const GameEngine& engine){
    out << "Current Game State: " << *engine.currentState
        << ", Player Count: " << engine.playerCount;
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

// This method gets the current player count
int GameEngine::getPlayerCount() const {
    return playerCount;
}

// will add a single player to the game. Currently not maximum player limit
bool GameEngine::addPlayer() {
    playerCount++;
    std::cout << "Player " << playerCount << " added!" << std::endl;
    return true;
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
        addPlayer();
        return true;
    }

    if (*currentState == "playersadded" && command == "assigncountries") {
        setState("assignreinforcement");
        return true;
    }
    else if (*currentState == "playersadded" && command == "addplayer") {
        addPlayer();
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

    return false; // invalid transition
}