#include "GameEngine.h"
#include <string>
#include <iostream>
#include <vector>
#include <filesystem>
#include "Map.h"
#include <sstream>
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

//getter and setter for mapSelect
const std::string& GameEngine::getMapSelect() const {
    return mapSelect;
}   
void GameEngine::setMapSelect(const std::string& pathName) {
    mapSelect = pathName;
}   



// Find all maps in current directory

void GameEngine::listMapsInCurrentDirectory() {
    namespace fs = std::filesystem;

    fs::path dir = fs::current_path() / "maps";
    std::cout << "Maps in '" << dir.string() << "':\n";

    std::error_code ec;
    if (!fs::exists(dir, ec) || !fs::is_directory(dir, ec)) {
        std::cout << "  (directory not found)\n";
        mapSelect.clear();
        return;
    }

    // Gather .map files
    std::vector<fs::path> entries;
    for (const auto& e : fs::directory_iterator(dir, ec)) {
        if (ec) break;
        if (e.is_regular_file() && e.path().extension() == ".map") {
            entries.push_back(e.path());
        }
    }

    if (entries.empty()) {
        std::cout << "  (no .map files found)\n";
        mapSelect.clear();
        return;
    }

    // Print numbered list
    for (size_t i = 0; i < entries.size(); ++i) {
        std::cout << "  [" << (i + 1) << "] " << entries[i].filename().string() << "\n";
    }

    // Prompt user for selection (by number OR exact filename — no trimming on filename)
    std::cout << "\nSelect a map by number or filename: "<< std::flush;
    if (std::cin.peek() == '\n') {
    std::cin.ignore(1); // discard exactly the pending '\n'
}

    std::string input;
    if (!std::getline(std::cin, input)) {
        std::cout << "\n(end of input)\n";
        mapSelect.clear();
        return;
    }

    if (input.empty()) {              // guard against empty line -> reprompt
        std::cout << "Please enter a number or filename.\n";
        mapSelect.clear();
        return; // or loop back to prompt again
    }

    // Helper: trim a COPY only for testing if it's a number
    auto trim_copy = [](std::string s) {
        auto sp = [](unsigned char c){ return std::isspace(c); };
        while (!s.empty() && sp(s.front())) s.erase(s.begin());
        while (!s.empty() && sp(s.back()))  s.pop_back();
        return s;
    };
    auto isNumber = [](const std::string& s){
        if (s.empty()) return false;
        for (unsigned char c : s) if (!std::isdigit(c)) return false;
        return true;
    };

    fs::path selected;
    std::string numProbe = trim_copy(input);  // safe to trim ONLY for numeric detection

    if (isNumber(numProbe)) {
        // Pick by index
        size_t idx = std::stoul(numProbe);
        if (idx == 0 || idx > entries.size()) {
            std::cout << "Invalid selection.\n";
            mapSelect.clear();
            return;
        }
        selected = entries[idx - 1];
    } else {
        // Use the filename EXACTLY as typed (no trimming)
        fs::path candidate = dir / input;
        if (!fs::exists(candidate, ec) || !fs::is_regular_file(candidate, ec)
            || candidate.extension() != ".map") {
            std::cout << "File not found or not a .map: " << input << "\n";
            mapSelect.clear();
            return;
        }
        selected = candidate;
    }

    setMapSelect(selected.string());
    std::cout << "Selected map: " << this->getMapSelect() << "\n";
    loadingMap(this->getMapSelect());
}

bool GameEngine::loadingMap(const std::string& path) {
    namespace fs = std::filesystem;

    // Optional sanity check (keeps full path exactly as given)
    std::error_code ec;
    if (!fs::exists(path, ec) || !fs::is_regular_file(path, ec)) {
        std::cout << "Load FAILED: path not found or not a file: " << path << "\n";
        return false;
    }

    MapLoader loader;
    Map* m = nullptr;
    std::ostringstream diag;

    if (!loader.load(path, m, diag)) {
        std::cout << "Load FAILED: " << path << "\n" << diag.str() << "\n";
        return false;
    }

    delete m_map;   // take ownership of the new map
    m_map = m;

    std::cout << "Load OK: " << path << "\n" << diag.str() << "\n";
    // If you manage states, do it here (e.g., setState("maploaded"));
    return true;
}

// Validate the currently loaded map    
bool GameEngine::validateLoadedMap() {
    if (m_map == nullptr) {
        std::cout << "No map loaded. Please load a map first.\n";
        return false;
    }

    std::ostringstream diag;

    // call Map::validate from your provided code
    bool valid = m_map->validate(diag);

    if (valid) {
        std::cout << "Map validation SUCCESS.\n";
        std::cout << diag.str(); // optional (if diag contains info)
        // If you use game states, update here:
        // setState("mapvalidated");
    } else {
        std::cout << "Map validation FAILED.\n";
        std::cout << diag.str();
    }

    return valid;
}



// Transition method will change the state of the game based on the command passed in and the 
// current state of the game. 
bool GameEngine::transition(const std::string& command) {
    if (*currentState == "start" && command == "loadmap"){
        std::cout << "======================================\n ";
        std::cout << "Available maps in current directory:\n";
        listMapsInCurrentDirectory();
        std::cout << "======================================\n ";
        setState("maploaded");
        return true;
    }

    if (*currentState == "maploaded" && command == "validatemap"){
        
        validateLoadedMap();
        
        setState("mapvalidated");
        return true;
    }
    else if (*currentState == "maploaded" && command == "loadmap") {
        std::cout << "======================================\n ";
        std::cout << "Available maps in current directory:\n";
        listMapsInCurrentDirectory();
        std::cout << "======================================\n ";

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