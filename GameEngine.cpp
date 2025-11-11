#include "GameEngine.h"
#include <string>
#include <iostream>
#include <vector>
#include "Map.h"
#include <sstream>
#include <algorithm>

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
    std::cout << "Available maps are in the 'maps' folder.\n";
    std::cout << "Enter map filename (e.g., world.map): ";
    std::string filename;
    std::cin >> filename;


    mapSelect = "maps/" + filename;
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
        else if (command.rfind("addplayer", 0) == 0) {
            if (!mapValidated) {
                std::cout << "Please validate the map first.\n";
                continue;
            }
            if (getPlayerCount() >= 6) {
                std::cout << "Maximum 6 players allowed.\n";
                continue;
            }
            transition("addplayer");
            playersAdded = getPlayerCount() >= 2;
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

void GameEngine::reinforcementPhase() {
    std::cout << "\n=== Reinforcement Phase ===\n";

    if (!m_map) {
        std::cout << "No map loaded – skipping reinforcement.\n";
        return;
    }

    const std::vector<Continent*>& continents     = m_map->getContinents();
    const std::vector<Territory*>& allTerritories = m_map->getTerritories();

    for (Player* p : players) {
        if (!p) continue;

        const std::vector<Territory*>* ownedPtr = p->territories();
        const auto& owned = ownedPtr ? *ownedPtr : std::vector<Territory*>();

        int numOwned = static_cast<int>(owned.size());
        if (numOwned == 0) {
            std::cout << p->name()
                      << " has no territories and receives 0 reinforcement armies.\n";
            continue;
        }

        // base = floor(#territories / 3)
        int base = numOwned / 3;

       
        int continentBonus = 0;
        for (Continent* c : continents) {
            if (!c) continue;

            bool ownsAll = true;
            bool hasAny  = false;

            for (Territory* t : allTerritories) {
                if (!t) continue;
                if (t->continentID != c->continentID) continue;

                hasAny = true;
                // if this territory of that continent isnt owned player doesn't own the continent
                if (std::find(owned.begin(), owned.end(), t) == owned.end()) {
                    ownsAll = false;
                    break;
                }
            }

            if (hasAny && ownsAll) {
                continentBonus += c->bonus;
            }
        }

        int total = base + continentBonus;
        if (total < 3) {
            total = 3;  // minimum 3 armies if player owns at least 1 territory
        }

        p->addReinforcements(total);

        std::cout << p->name()
                  << " owns " << numOwned << " territories and receives "
                  << total << " reinforcement armies (" << base
                  << " base + " << continentBonus
                  << " continent bonus). Pool now: "
                  << p->getReinforcementPool() << "\n";
    }
}

void GameEngine::issueOrdersPhase() {
    std::cout << "\n=== Issue Orders Phase ===\n";

    if (players.empty()) {
        std::cout << "No players in the game.\n";
        return;
    }

    const std::size_t n = players.size();
    std::vector<bool> done(n, false);
    std::size_t remaining = n;

    while (remaining > 0) {
        bool anyIssuedThisRound = false;

        for (std::size_t i = 0; i < n; ++i) {
            if (done[i]) continue;

            Player* p = players[i];
            if (!p) {
                done[i] = true;
                --remaining;
                continue;
            }

            const auto* terrs = p->territories();
            if (!terrs || terrs->empty()) {
                std::cout << p->name()
                          << " has no territories and cannot issue orders this turn.\n";
                done[i] = true;
                --remaining;
                continue;
            }

            std::cout << "\n--- " << p->name() << "'s turn to issue orders ---\n";
            std::cout << "Reinforcement pool: " << p->getReinforcementPool() << "\n";

            std::cout << "Issue an order? (y/n): ";
            char choice;
            std::cin >> choice;
            choice = static_cast<char>(std::tolower(choice));
            if (choice != 'y') {
                done[i] = true;
                --remaining;
                continue;
            }

            std::string kind;
            if (p->getReinforcementPool() > 0) {
                // Assignment rule: while you have reinforcements, you only issue Deploy orders
                kind = "deploy";
                std::cout << "You still have reinforcement armies, issuing a Deploy order.\n";
            } else {
                std::cout << "Enter order type (advance, bomb, blockade, airlift, negotiate, deploy): ";
                std::cin >> kind;
            }

            Order* issued = p->issueOrder(kind);
            if (issued) {
                std::cout << "Issued a \"" << kind << "\" order.\n";
                anyIssuedThisRound = true;
            } else {
                std::cout << "No order issued, marking " << p->name()
                          << " done for this turn.\n";
                done[i] = true;
                --remaining;
            }
        }

        //if nobody issued anything 
        if (!anyIssuedThisRound) {
            break;
        }
    }

    std::cout << "\n=== End of Issue Orders Phase ===\n";
}

void GameEngine::executeOrdersPhase() {
    std::cout << "\n=== Execute Orders Phase ===\n";

    if (players.empty()) {
        std::cout << "No players in the game.\n";
        return;
    }

    //  1: Execute all DEPLOY orders first (round-robin) 
    bool executedSomething = true;
    while (executedSomething) {
        executedSomething = false;

        for (Player* p : players) {
            if (!p) continue;

            OrdersList* ol = p->orders();
            if (!ol) continue;

            int sz = ol->size();
            int deployIndex = -1;

            // find first Deploy order in this player's list
            for (int i = 0; i < sz; ++i) {
                Order* o = ol->get(i);
                if (o && o->getOrderName() == "Deploy") {
                    deployIndex = i;
                    break;
                }
            }

            if (deployIndex != -1) {
                Order* o = ol->get(deployIndex);
                std::cout << "Executing Deploy order...\n";
                o->execute();
                std::cout << "  -> " << o->getAction() << "\n";

                // remove executed Deploy from the list
                ol->remove(deployIndex);
                executedSomething = true;
            }
        }
    }

    // 2: Execute remaining orders round-robin 
    bool ordersRemain = true;
    while (ordersRemain) {
        ordersRemain = false;

        for (Player* p : players) {
            if (!p) continue;

            OrdersList* ol = p->orders();
            if (!ol) continue;

            int sz = ol->size();
            if (sz == 0) continue;   // this player has no more orders

            ordersRemain = true;     // at least one player still has orders

            Order* o = ol->get(0);   // execute from the front
            if (!o) {
                ol->remove(0);
                continue;
            }

            std::cout << "Executing order: " << o->getOrderName() << "\n";
            o->execute();
            std::cout << "  -> " << o->getAction() << "\n";

            // remove executed order 
            ol->remove(0);
        }
    }

    std::cout << "=== End of Execute Orders Phase ===\n";
}

void GameEngine::mainGameLoop() {
    std::cout << "\n=== Main Game Loop ===\n";

    bool gameOver = false;
    while (!gameOver) {
        reinforcementPhase();
        issueOrdersPhase();
        executeOrdersPhase();

        // remove players with no territories
        for (auto it = players.begin(); it != players.end(); ) {
            Player* p = *it;
            const auto* terrs = p->territories();
            if (!terrs || terrs->empty()) {
                std::cout << p->name() << " has been eliminated.\n";
                delete p;                // if you own the pointer
                it = players.erase(it);  // remove from vector
            } else {
                ++it;
            }
        }

        // win condition: one player owns all territories
        if (players.size() == 1) {
            std::cout << "\n*** " << players[0]->name()
                      << " controls all territories and wins the game! ***\n";
            gameOver = true;
        } else if (players.empty()) {
            std::cout << "\nNo players remain. Game over.\n";
            gameOver = true;
        }
    }
}

void testMainGameLoop() {
    std::cout << "=== TEST: Main Game Loop (Part 3) ===" << std::endl;

    GameEngine engine;

  
    engine.startupPhase();

    std::cout << "\nStarting main game loop...\n" << std::endl;

   
    engine.mainGameLoop();

    std::cout << "\n=== END TEST: Main Game Loop (Part 3) ===" << std::endl;
}
