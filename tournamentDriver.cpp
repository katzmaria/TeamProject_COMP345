#include "CommandProcessing.h"
#include "GameEngine.h"

#include <iostream>
#include <queue>
#include <string>

namespace {
class QueuedCommandProcessor final : public CommandProcessor {
public:
    void enqueue(const std::string& command) {
        queue_.push(command);
    }

protected:
    void readCommand() override {
        if (queue_.empty()) {
            CommandProcessor::readCommand();
            return;
        }
        saveCommand(queue_.front());
        queue_.pop();
    }

private:
    std::queue<std::string> queue_;
};
}

void testTournament() {
    std::cout << "=== testTournament() ===\n";

    QueuedCommandProcessor processor;
    GameEngine engine;

    const std::string demoCommand =
        "tournament -M 002_I72_X-29.map -P aggressive benevolent cheater neutral -G 2 -D 15";

    processor.enqueue(demoCommand);

    std::string processed = processor.getCommand("start");
    if (processed == "invalid" || processed.empty()) {
        std::cout << "Provided tournament command is invalid.\n";
        return;
    }

    std::cout << "Processed tournament command: " << processed << "\n\n";
    engine.tournamentMode(processed);
}
