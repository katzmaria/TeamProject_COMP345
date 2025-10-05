#pragma once
#include <string>
#include <iostream>

class GameEngine {
    private:
        std::string* currentState;

    public: 
        GameEngine();
        GameEngine(const GameEngine& engine);
        GameEngine& operator=(const GameEngine& engine);
        friend std::ostream& operator<<(std::ostream& out, const GameEngine& engine);
        ~GameEngine();
        void setState(const std::string& newState);
        std::string getState() const;
        bool transition(const std::string& command);
};