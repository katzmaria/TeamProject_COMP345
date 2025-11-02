#include "Map.h"
#include <fstream>        // file reading (std::ifstream)
#include <string>         // std::string
#include <ostream>        // std::ostream
#include <iostream>       // std::cout
#include <algorithm>      // std::find
#include <unordered_map>  // hash maps
#include <queue>          // BFS
#include <unordered_set>  // visited sets
#include <sstream>


// flags
bool continentFlag = false;
bool TerritoryFlag = false;

// remember continent ids by name (so territories can map continentName -> id)
static std::unordered_map<std::string, Territory*> gTerrByName; 
static std::unordered_map<std::string, int> gContinentIdByName;

// this is the map constructor
Map::Map() {}

bool MapLoader::load(const std::string& path, Map*& outMap, std::ostream& diag) {
    outMap = nullptr;

    // reset per-load state so multiple loads in one run don’t leak
    gTerrByName.clear();
    gContinentIdByName.clear();
    continentFlag = false;
    TerritoryFlag = false;

    std::ifstream in(path);
    if (!in) {
        diag << "[loader] could not open: " << path << "\n";
        return false;
    }

    Map* m = new Map();
    size_t total = 0;

    // ... your existing parsing of [Continents] and [Territories] ...
    // increment `total` as you read lines, fill m->continents / m->territories, etc.

    // after parsing:
    m->resolveAllNeighbors();

    // validate BEFORE handing the map back
    std::ostringstream vdiag;
    bool ok = m->validate(vdiag);   // checks: global connectivity, continent connectivity, 1 continent per territory
    diag << vdiag.str();

    if (!ok) {
        diag << "[loader] validation failed. rejecting file.\n";
        delete m;            // avoid leaking an invalid map
        return false;        // outMap remains nullptr
    }

    outMap = m;
    diag << "[loader] opened OK. total lines = " << total << "\n";
    return true;
}

void MapLoader::addContinentFromLine(Map* m, const std::string& line) {
    std::string continentName;
    for (char ch : line) {
        if (ch != '=') continentName += ch;
        else break;
    }
    std::cout << "adding continent ----> " << continentName << " \n";
    int id = (*m).addContinent(continentName);
    gContinentIdByName[continentName] = id; // adding the continent name to the unordered map 
}

void MapLoader::addTerritoryFromeLine(Map* m, const std::string& line) {
    int splitter = 0;                     // 0=name, 1=x, 2=y, 3=continent, >=4 neighbors
    std::string territoryName;
    std::string Xcoord;
    std::string Ycoord;
    std::string continentName;
    std::vector<std::string> neighborTerr;
    std::string currentNeighbor;

    for (char ch : line) {
        if (ch == ',') {
            switch (splitter) {
            case 0: splitter = 1; break;                 // finished territoryName
            case 1: splitter = 2; break;                 // finished Xcoord
            case 2: splitter = 3; break;                 // finished Ycoord
            case 3: splitter = 4;                        // finished continentName
                if (!currentNeighbor.empty()) {
                    neighborTerr.push_back(currentNeighbor);
                    currentNeighbor.clear();
                }
                break;
            default:                                     // neighbors
                neighborTerr.push_back(currentNeighbor);
                currentNeighbor.clear();
                break;
            }
            continue;
        }

        switch (splitter) {
        case 0: territoryName += ch;   break;
        case 1: Xcoord += ch;   break;
        case 2: Ycoord += ch;   break;
        case 3: continentName += ch;   break;
        default: currentNeighbor += ch; break; // neighbors
        }
    }

    // push the last neighbor if present
    if (splitter >= 4 && !currentNeighbor.empty()) {
        neighborTerr.push_back(currentNeighbor);
    }

    // convert coords
    int x = 0, y = 0;
    try { x = std::stoi(Xcoord); y = std::stoi(Ycoord); }
    catch (...) {
        std::cout << "[loader] bad coords for: " << territoryName << "\n";
        return;
    }

    // checking to see if the continent name is inside the the continent unordered map
    if (!gContinentIdByName.count(continentName)) {   // 0 = not found, 1 = found
        std::cout << "[loader] unknown continent '" << continentName
            << "' for " << territoryName << "\n";
        return;
    }
    int contId = gContinentIdByName.at(continentName);   


    // create territory (stores neighbor names for later linking)

    m->addTerritory(territoryName, contId, x, y, neighborTerr);
}

int Map::addContinent(const std::string& name, int bonus) {
    int id = static_cast<int>(continents.size());
    Continent* c = new Continent(id, name, bonus);
    continents.push_back(c);
    return id;
}

// matches your header: create + set continentID + stash neighbor names
Territory* Map::addTerritory(const std::string& name, int continentId, int x, int y,
    const std::vector<std::string>& neighborsIn) {
    int id = static_cast<int>(territories.size());
    Territory* t = new Territory(id, name, x, y, neighborsIn); // ctor fills neighborsNames
    t->continentID = continentId;
    territories.push_back(t);

    gTerrByName[name] = t;  // This is to rememver where the territory lives

    return t;
}

// --- define Territory::addNeighbor (as declared in header) ---
void Territory::addNeighbor(Territory* t) {
    if (!t || t == this) return;
    if (std::find(neighbors.begin(), neighbors.end(), t) == neighbors.end())
        neighbors.push_back(t);
}

// Territory constructor (matches header)
Territory::Territory(int id_, const std::string& name_, int x_, int y_,
    const std::vector<std::string>& neighborsIn)
    : id(id_), name(name_), x(x_), y(y_) {
    // continentID set in Map::addTerritory
    neighborsNames = neighborsIn; // we�ll resolve names to pointers later
}

// implementation of the destructor
Map::~Map() {
    for (Territory* t : territories) delete t;
    territories.clear();

    for (Continent* c : continents) delete c;
    continents.clear();
}

// simple copy constructor for now
Map::Map(const Map& other) {
    continents.reserve(other.continents.size());
    for (const Continent* oc : other.continents) {
        continents.push_back(new Continent(*oc));
    }

    territories.reserve(other.territories.size());
    for (const Territory* ot : other.territories) {
        Territory* nt = new Territory(*ot);  // copies id, name, coords, continentID, neighborsNames
        nt->neighbors.clear();               // don�t point to other's objects
        territories.push_back(nt);
    }
}

// operators for continents
Continent::Continent(const Continent& other)
    : continentID(other.continentID),
    continentName(other.continentName),
    bonus(other.bonus) {
}

Continent& Continent::operator=(const Continent& other) {
    if (this != &other) {
        continentID = other.continentID;
        continentName = other.continentName;
        bonus = other.bonus;
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Continent& c) {
    os << "Continent{id=" << c.continentID
        << ", name=\"" << c.continentName << "\"}";
    return os;
}

Continent::Continent(int id, const std::string& name, int bonus_)
    : continentID(id), continentName(name), bonus(bonus_) {
}

void Map::resolveAllNeighbors() {
    // loop over all territories by index
    for (size_t i = 0; i < territories.size(); ++i) {
        Territory* t = territories[i];

        // loop over all neighbor NAMES for this territory
        for (size_t j = 0; j < t->neighborsNames.size(); ++j) {
            const std::string& nbName = t->neighborsNames[j];

            // does a territory with this name exist?
            if (!gTerrByName.count(nbName)) {
                std::cout << "[loader] unknown neighbor '" << nbName
                    << "' for " << t->name << "\n";
                continue; // skip bad name
            }

            // get the actual neighbor pointer and link both ways
            Territory* nb = gTerrByName.at(nbName);  // name -> Territory*
            linkNeighbors(t, nb);                    // adds t->nb and nb->t basically the undirected edges
        }
    }
}



bool Map::graphConnectedAll() const {
    if (territories.empty()) return true;
    std::unordered_set<const Territory*> seen;
    std::queue<const Territory*> q;

    q.push(territories.front());
    seen.insert(territories.front());

    while (!q.empty()) {
        auto* u = q.front(); q.pop();
        for (auto* v : u->neighbors) {
            if (v && !seen.count(v)) { seen.insert(v); q.push(v); }
        }
    }
    return (seen.size() == territories.size());
}


#include <algorithm> // ensure this is included for std::find

void Map::linkNeighbors(Territory* a, Territory* b) {

    // 1) reject any bad links
    if (a == nullptr || b == nullptr || a == b) return;

    // 2) add b to a's list only if it's not already there
    bool hasAB = false;
    for (size_t i = 0; i < a->neighbors.size(); ++i) {
        if (a->neighbors[i] == b) { hasAB = true; break; } // loop checks if it is already there
    }
    if (!hasAB) {
        a->neighbors.push_back(b); // here we are basically adding the pointer of b to the neighbors list of a 
    }

    // 3) add a to b's list only if it's not already there
    bool hasBA = false;
    for (size_t j = 0; j < b->neighbors.size(); ++j) {
        if (b->neighbors[j] == a) { hasBA = true; break; }
    }
    if (!hasBA) {
        b->neighbors.push_back(a);
    }
}


bool Map::validate(std::ostream& diag) const {
    // 1) whole-map connectivity
    if (!graphConnectedAll()) {
        diag << "Map not connected\n";
        return false;
    }
    // 2) each continent is a connected subgraph
    for (auto* c : continents) {
        if (!continentConnected(c->continentID)) {
            diag << "Continent not connected: " << c->continentName << "\n";
            return false;
        }
    }
    // 3) each territory has exactly one valid continent id
    for (auto* t : territories) {
        if (t->continentID < 0 || t->continentID >= (int)continents.size()) {
            diag << "Bad continent id for " << t->name << "\n";
            return false;
        }
    }
    return true;
}



// BFS restricted to a single continent
bool Map::continentConnected(int cid) const {
    // collect nodes in this continent
    std::vector<const Territory*> nodes;
    for (size_t i = 0; i < territories.size(); ++i) {
        const Territory* t = territories[i];
        if (t->continentID == cid) nodes.push_back(t);
    }
    if (nodes.empty()) return false;

    // put the same nodes into a hash set for fast membership tests
    std::unordered_set<const Territory*> in;
    for (size_t i = 0; i < nodes.size(); ++i) {
        in.insert(nodes[i]);
    }

    std::unordered_set<const Territory*> seen;
    std::queue<const Territory*> q;

    q.push(nodes[0]);
    seen.insert(nodes[0]);

    while (!q.empty()) {
        const Territory* u = q.front(); q.pop();
        const std::vector<Territory*>& nbrs = u->neighbors;

        for (size_t j = 0; j < nbrs.size(); ++j) {
            const Territory* v = nbrs[j];
            if (in.count(v) && !seen.count(v)) {
                seen.insert(v);
                q.push(v);
            }
        }
    }
    return seen.size() == nodes.size();
}



std::ostream& operator<<(std::ostream& os, const Map& m) {
    os << "Map has " << m.continents.size() << " continents and "
        << m.territories.size() << " territories\n";

    for (const Territory* t : m.territories) {
        os << "  - " << t->name << " (id=" << t->id
            << ", contId=" << t->continentID
            << ", x=" << t->x << ", y=" << t->y << ")";
        if (!t->neighbors.empty()) {
            os << "  neighbors=[";
            for (size_t i = 0; i < t->neighbors.size(); ++i) {
                if (i) os << ", ";
                os << t->neighbors[i]->name;
            }
            os << "]";
        }
        os << "\n";
    }
    return os;
}

