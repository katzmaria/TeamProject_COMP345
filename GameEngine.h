#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "Map.h"

class Player;         
class Deck;  
class GameEngine {
    private:
        std::string* currentState;
        int *playerCount;
        std::string mapSelect;
        Map* m_map = nullptr;
        std::vector<Player*> players;  // players in the game
        Deck* deck = nullptr;// deck in startup

    public: 
        GameEngine();
        GameEngine(const GameEngine& engine);
        GameEngine& operator=(const GameEngine& engine);
        friend std::ostream& operator<<(std::ostream& out, const GameEngine& engine);
        ~GameEngine();
        void setState(const std::string& newState);
        std::string getState() const;
        bool transition(const std::string& command);
        int getPlayerCount() const;
        bool addPlayer();
        void listMapsInCurrentDirectory() ;
        const std::string& getMapSelect() const ;
        void setMapSelect(const std::string& pathName);
        bool loadingMap(const std::string& path);
        bool validateLoadedMap();
        void startupPhase();

        void mainGameLoop();
        void reinforcementPhase();
        void issueOrdersPhase();
        void executeOrdersPhase();
};

