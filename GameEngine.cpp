#include "GameEngine.h"
#include "CommandProcessing.h"
#include "Player.h"
#include "Cards.h"
#include "Orders.h"
#include "Map.h"

#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <random>
#include <filesystem>

// basic GameEngine methods 


//creating the deck with cards
void seedDeck(Deck* deck) {
    
    for (int i = 0; i < 10; ++i) {
        deck->addCard(new Card("Airlift"));
    }
    for (int i = 0; i < 10; ++i){
        deck->addCard(new Card("Bomb"));
    }
    for (int i = 0; i < 10; ++i){
        deck->addCard(new Card("Blockade"));

    } 
    for (int i = 0; i < 10; ++i){

        deck->addCard(new Card("Negotiate")); 
    }  
}

GameEngine::GameEngine() {
    currentState = new std::string("start");
    playerCount  = new int(0);
    deck = new Deck();
    seedDeck(deck);
}

GameEngine::GameEngine(const GameEngine& engine) {
    currentState = new std::string(*engine.currentState);
    playerCount  = new int(*engine.playerCount);
    deck = new Deck();
    seedDeck(deck);
}

GameEngine& GameEngine::operator=(const GameEngine& engine) {
    if (this != &engine) {
        delete currentState;
        delete playerCount;
        currentState = new std::string(*engine.currentState);
        playerCount  = new int(*engine.playerCount);
    }
    return *this;
}

std::ostream& operator<<(std::ostream& out, const GameEngine& engine) {
    out << "Current Game State: " << *engine.currentState
        << ", Player Count: " << *engine.playerCount;
    return out;
}

GameEngine::~GameEngine() {
    delete currentState;
    delete playerCount;
    delete deck;
}

void GameEngine::setState(const std::string& newState) {
    delete currentState;
    currentState = new std::string(newState);
}

std::string GameEngine::getState() const {
    return *currentState;
}

int GameEngine::getPlayerCount() const {
    return *playerCount;
}

bool GameEngine::addPlayer() {
    (*playerCount)++;
    std::cout << "Player " << *playerCount << " added!" << std::endl;
    return true;
}

// mapSelect getter and setter
const std::string& GameEngine::getMapSelect() const {
    return mapSelect;
}

void GameEngine::setMapSelect(const std::string& pathName) {
    mapSelect = pathName;
}

// List all .map files in the "maps" directory and prompt user to select one 
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

    // Gathering .map files
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

    // Print list
    for (size_t i = 0; i < entries.size(); ++i) {
        std::cout << "  [" << (i + 1) << "] " << entries[i].filename().string() << "\n";
    }

    // Prompt user for selection 
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
        return; // or loop back
    }

    // Helper function to trim whitespace and check if numeric
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
    std::string numProbe = trim_copy(input);  // trim whitespace for number check
    
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
        // Use the filename EXACTLY as entered
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
    MapLoader loader;
    Map* newMap = nullptr;
    std::ostringstream diag;

    if (!loader.load(path, newMap, diag)) {
        std::cout << "Load FAILED: " << path << "\n" << diag.str() << "\n";
        return false;
    }

    if (m_map != nullptr) {
        delete m_map;   // free old map
    }
    m_map = newMap;

    std::cout << "Load OK: " << path << "\n" << diag.str() << "\n";
    return true;


// validateLoadedMap, transition, startupPhase

    delete m_map;  
    m_map = newMap;

    std::cout << "Load OK: " << path << "\n" << diag.str() << "\n";
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
        std::cout << diag.str(); 
    } else {
        std::cout << "Map validation FAILED.\n";
        std::cout << diag.str();
    }

    return valid;
}



// Transition method to change the game state based on commands,

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

        return true; // Stay  in maploaded state
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
        return true; // Stay in playersadded state
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
        return true; // Stay in issueorders state
    }
    
    if (*currentState == "executeorders" && command == "endexecuteorders"){
        setState("assignreinforcement");
        return true;
    }
    else if (*currentState == "executeorders" && command == "executeorder") {
        return true; //Stay  in the executeorders state
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

    return false; // not valid trransition state 
}

// Startup phase 
void GameEngine::startupPhase() {
    std::cout << "=== Game Startup Phase ===" << std::endl;

    CommandProcessor* cmdProcessor = new CommandProcessor();

    bool mapLoaded     = false;
    bool mapValidated  = false;
    bool playersAdded  = false;

    while (true) {
        std::string command = cmdProcessor->getCommand(getState());

        // loadmap 
        if (command.rfind("loadmap", 0) == 0) {
            if (transition("loadmap")) {
                mapLoaded = true;
            }
        }
        //  validatemap
        else if (command == "validatemap") {
            if (!mapLoaded) {
                std::cout << "Please load a map first.\n";
                continue;
            }
            if (transition("validatemap")) {
                mapValidated = true;
            }
        }
        //addplayer <name>
        else if (command.rfind("addplayer", 0) == 0) {
            if (!mapValidated) {
                std::cout << "Please validate the map first.\n";
                continue;
            }
            if (getPlayerCount() >= 6) {
                std::cout << "Maximum 6 players allowed.\n";
                continue;
            }

            // substr to get player name after "addplayer"
            std::string name = command.substr(9); // length("addplayer") == 9
            while (!name.empty() && std::isspace(static_cast<unsigned char>(name.front()))) {
                name.erase(name.begin());
            }
            if (name.empty()) {
                name = "Player" + std::to_string(getPlayerCount() + 1);
            }

            // update state / playerCount
            transition("addplayer");

            // create and store Player object
            players.push_back(new Player(name));

            playersAdded = getPlayerCount() >= 2;
        }
        // gamestart
        else if (command == "gamestart") {
            if (!playersAdded || getPlayerCount() < 2) {
                std::cout << "At least 2 players required.\n";
                continue;
            }

            std::cout << "Starting game setup..." << std::endl;

            // a) fair territory distribution
            if (m_map) {
                std::vector<Territory*> allTerritories = m_map->getTerritories();
                
                // std::cout << "DEBUG: Map has " << allTerritories.size() << " territories to distribute.\n";
                
                std::shuffle(allTerritories.begin(), allTerritories.end(),
                             std::default_random_engine(
                                 static_cast<unsigned>(std::time(nullptr))));

                int i = 0;
                for (auto* t : allTerritories) {
                    Player* currentPlayer = players[i % players.size()];
                    // std::cout << "DEBUG: Assigning territory " << t->name << " to " << currentPlayer->name() << "\n";
                    t->setOwner(currentPlayer);
                    currentPlayer->addTerritory(t);
                    ++i;
                }
                std::cout << "Territories distributed." << std::endl;
            } else {
                // std::cout << "DEBUG: m_map is NULL!\n";
            }

            // b) randomize order using random engine
            std::shuffle(players.begin(), players.end(),
                         std::default_random_engine(
                             static_cast<unsigned>(std::time(nullptr))));
            std::cout << "Random order of play determined." << std::endl;

            // c) 50 initial army units
            for (auto* p : players) {
                p->addReinforcements(50);
            }

            // d) Initialize deck with special order cards
            for (int i = 0; i < 10; ++i) {
                deck->addCard(new Card("bomb"));
                deck->addCard(new Card("blockade"));
                deck->addCard(new Card("airlift"));
                deck->addCard(new Card("negotiate"));
            }
            std::cout << "Deck initialized with 40 cards: (10 of each type).\n";

            // e) draw 2 cards each
            for (auto* p : players) {
                if (!p->hand()) {
                    p->setHand(new Hand());
                }
                deck->draw(p->hand());
                deck->draw(p->hand());
                std::cout << p->name() << " drew 2 initial cards.\n";
            }

            // e) switch to play phase
            setState("play");
            std::cout << "Game state switched to PLAY phase.\n";
            
            // Show each player their starting hand
            std::cout << "\n=== Starting Hands ===\n";
            for (auto* p : players) {
                if (p->hand()) {
                    std::cout << p->name() << "'s hand: ";
                    for (Card* c : p->hand()->getCards()) {
                        std::cout << c->getType() << " ";
                    }
                    std::cout << "\n";
                }
            }
            
            break;
        }
        // quit 
        else if (command == "quit") {
            std::cout << "Startup phase terminated." << std::endl;
            break;
        }
        //  unknown 
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
        
        // Change relations at start of turn
        p->clearDiplomaticRelations();

        const std::vector<Territory*>* ownedPtr = p->territories();
        if (!ownedPtr) {
            std::cout << p->name()
                      << " has no territories and receives 0 reinforcement armies.\n";
            continue;
        }
        const auto& owned = *ownedPtr;

        int numOwned = static_cast<int>(owned.size());
        // std::cout << "DEBUG: " << p->name() << " territories() returned vector with " 
        //           << numOwned << " territories.\n";
        
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
                // if this territory is in the player's owned list 
                if (std::find(owned.begin(), owned.end(), t) == owned.end()) {
                    ownsAll = false;
                    break;
                }
            }

            if (hasAny && ownsAll) {
                continentBonus += c->bonus;
            }
        }
        // theres minimum 3 armies if player owns at least 1 territory
        int total = base + continentBonus;
        if (total < 3) {
            total = 3;  
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


//issue order phase 
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
            std::cout << "Reinforcement pool:" << p->getReinforcementPool() << "\n";
            
            // Show each player's hand
            if (p->hand() && !p->hand()->getCards().empty()) {
                std::cout << "Cards in hand: ";
                for (Card* c : p->hand()->getCards()) {
                    std::cout << "[" << c->getType() << "] ";
                }
                std::cout << "\n";
            }
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
            if (p->getAvailableReinforcements() > 0) {
                // while you have reinforcements, you only issue Deploy orders
                kind = "deploy";
                std::cout << "You still have reinforcement armies, issuing a Deploy order.\n";
            } else {
                // After deploying all reinforcements, can use cards or advance
                std::cout << "Enter order type:\n";
                std::cout << "  'advance' - Move/attack with armies\n";
                std::cout << "  'card' - Play a card from your hand\n";
                std::cout << "Order type: ";
                std::cin >> kind;
                
                // If player wants to play a card, display their hand and let them choose what to play
                if (kind == "card") {
                    if (!p->hand() || p->hand()->getCards().empty()) {
                        std::cout << "You have no cards to play!\n";
                        continue;
                    }
                    
                    std::cout << "\nYour cards:\n";
                    std::vector<Card*> handCards = p->hand()->getCards();
                    for (size_t i = 0; i < handCards.size(); ++i) {
                        std::cout << "  [" << i << "] " << handCards[i]->getType() << "\n";
                    }
                    
                    std::cout << "Select card index to play: ";
                    size_t cardIdx;
                    std::cin >> cardIdx;
                    
                    if (cardIdx >= handCards.size()) {
                        std::cout << "Invalid card index.\n";
                        continue;
                    }
                    
                    // Set kind to the card type
                    kind = handCards[cardIdx]->getType();
                    std::cout << "Playing " << kind << " card!\n";
                    
                    // Remove card from hand after selecting it
                    // and add it back to the deck after playing
                }
            }

            Order* issued = p->issueOrder(kind, deck, &players);
            if (issued) {
                std::cout << "Issued a \"" << kind << "\" order.\n";
                anyIssuedThisRound = true;
            } else {
                std::cout << "No order issued, marking " << p->name()
                          << " done for this turn.\n";
                done[i] = true;
                --remaining;
            }

            std::cout << "\n" << p->name() << ", view your hand? (y/n): ";
            char view;
            std::cin >> view;
            view = static_cast<char>(std::tolower(view));
            if (view == 'y') {
                if (p->hand()) {
                    std::cout << "\n=== " << p->name() << "'s Hand ===\n";
                    std::cout << *(p->hand()) << "\n";   // relies on operator<<(std::ostream&, const Hand&)
                } else {
                    std::cout << p->name() << " has no hand.\n";
                }
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
    
    // Reset all players' committed reinforcements and conquered flags at start of next phase
    for (Player* p : players) {
        if (p) {
            p->resetCommitted();
        }
    }

    std::cout << "Orders were executed immediately as they were issued.\n";

    // Award cards to players who conquered at least one territory this turn
    std::cout << "\n--- Card Distribution ---\n";
    bool anyCardDrawn = false;
    for (Player* p : players) {
        if (!p) continue;
        if (p->hasConqueredThisTurn()) {
            if (!p->hand()) {
                p->setHand(new Hand());
            }
            if (deck) {
                deck->draw(p->hand());
                std::cout << p->name() << " conquered territory this turn and draws 1 card!\n";
                anyCardDrawn = true;
            }
      
            p->setConqueredThisTurn(false);
        }
    }
    if (!anyCardDrawn) {
        std::cout << "No territories conquered this turn. No cards drawn.\n";
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
                delete p;               
                it = players.erase(it);  
            } else {
                ++it;
            }
        }
        // win condition reached when one player has all territories
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
