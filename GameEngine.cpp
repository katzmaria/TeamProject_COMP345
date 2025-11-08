#include "GameEngine.h"
#include <string>
#include <iostream>

// default constructor will initialize the GameEngine object to the start state
GameEngine::GameEngine() {
    currentState = new std::string("start");
    playerCount = new int(0);
}

// copy constructor 
GameEngine::GameEngine(const GameEngine& engine){
    currentState = new std::string(*engine.currentState);
    playerCount = new int(*engine.playerCount);
}

// Assignment operator
GameEngine& GameEngine::operator=(const GameEngine& engine){
    if (this != &engine){
        delete currentState;
        currentState = new std::string(*engine.currentState);
        playerCount = new int(*engine.playerCount);
    }
    return *this;
}

// Stream insertion operator implementation
std::ostream& operator<<(std::ostream& out, const GameEngine& engine){
    out << "Current Game State: " << *engine.currentState
        << ", Player Count: " << *engine.playerCount;
    return out;
}

// destructor *(will have to be updated when we add more attributes to the class)
GameEngine::~GameEngine(){
    delete currentState;
    delete playerCount;
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
    return *playerCount;
}

// will add a single player to the game. Currently not maximum player limit
bool GameEngine::addPlayer() {
    (*playerCount)++;
    std::cout << "Player " << *playerCount << " added!" << std::endl;
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

// Startup phase implementation: Part 2 of the assignment
void GameEngine::startupPhase() {
    std::cout << "=== Game Startup Phase ===" << std::endl;

    CommandProcessor* cmdProcessor = new CommandProcessor();

    bool mapLoaded = false;
    bool mapValidated = false;
    bool playersAdded = false;

    while (true) {
        std::string command = cmdProcessor->getCommand();

        if (command.rfind("loadmap", 0) == 0) {
            if (transition("loadmap")) mapLoaded = true;
        }
        else if (command == "validatemap") {
            if (!mapLoaded) {
                std::cout << "Please load a map first.\n";
                continue;
            }
            if (transition("validatemap")) mapValidated = true;
        }
    
        // Adding players
        else if (command.rfind("addplayer", 0) == 0) {
            if (!mapValidated) {
                std::cout << "Please validate the map first.\n";
                continue;
            }
            if (getPlayerCount() >= 6) {
                std::cout << "Maximum 6 players allowed.\n";
                continue;
            }

            // Parse "addplayer <playername>"
            std::istringstream iss(command);
            std::string cmd, playerName;
            iss >> cmd >> playerName;

            if (playerName.empty()) {
                std::cout << "Usage: addplayer <playername>\n";
                continue;
            }

            // Create and store the player
            Player* newPlayer = new Player(playerName);
            players.push_back(newPlayer);

            (*playerCount)++;
            transition("addplayer");

            std::cout << "Player added: " << playerName << std::endl;
            playersAdded = (players.size() >= 2);
        }

      
        else if (command == "gamestart") {
            if (!playersAdded || getPlayerCount() < 2) {
                std::cout << "At least 2 players required.\n";
                continue;
            }

            std::cout << "Starting game setup..." << std::endl;

            // a) fair territory distribution
            if (m_map) {
                std::vector<Territory*> allTerritories = m_map->getTerritories();
                std::shuffle(allTerritories.begin(), allTerritories.end(),
                             std::default_random_engine(static_cast<unsigned>(time(nullptr))));

                int i = 0;
                for (auto* t : allTerritories) {
                    t->setOwner(players[i % getPlayerCount()]);
                    i++;
                }
                std::cout << "Territories distributed." << std::endl;
            }

            // b) randomize play order
            std::vector<Player*> temp = players;
            std::shuffle(temp.begin(), temp.end(),
                         std::default_random_engine(static_cast<unsigned>(time(nullptr))));
            players = temp;
            std::cout << "Random order of play determined." << std::endl;

            // c) give 50 initial army units
            for (auto* p : players) {
                p->setReinforcementPool(50);
            }

            // d) draw 2 cards each
            for (auto* p : players) {
                p->addCard(deck->draw());
                p->addCard(deck->draw());
            }

            // e) switch to play phase
            setState("play");
            std::cout << "Game state switched to PLAY phase.\n";
            break;
        }
        else if (command == "quit") {
            std::cout << "Startup phase terminated." << std::endl;
            break;
        }
        else {
            std::cout << "Unknown command.\n";
        }
    }

    delete cmdProcessor;
}