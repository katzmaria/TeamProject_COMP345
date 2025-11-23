#include "CommandProcessing.h"
#include "GameEngine.h"

void testTournament() {
    // can test with this command: "tournament -M 002_I72_X-29.map 001_I72_Ghtroc 720.map -P aggressive cheater neutral benevolent -G 2 -D 20"
    GameEngine engine = GameEngine();
    std::cout << "Enter a valid tournament command: ";
    engine.startupPhase();
}
