#pragma once
#include <string>
#include <vector>
#include <iosfwd>

// ---------- Territory ----------
struct Territory {
    int id = -1;
    int continentID = -1;
    std::string name;
    int x = 0, y = 0;

    std::vector<std::string> neighborsNames;
    std::vector<Territory*> neighbors;

    Territory(int id, const std::string& name, int x, int y, const std::vector<std::string>& neighbors);

    void addNeighbor(Territory* t);
};

// ---------- Continent ----------
struct Continent {
    int continentID = -1;
    std::string continentName;
    int bonus = 0;

    Continent(int id, const std::string& name, int bonus_);
    Continent(const Continent& other);
    Continent& operator=(const Continent& other);
    ~Continent() = default;
    friend std::ostream& operator<<(std::ostream& os, const Continent& c);
};

// ---------- Map ----------
class Map {
public:
    Map();
    Map(const Map& other);
    ~Map();

    int addContinent(const std::string& name, int bonus);
    int addContinent(const std::string& name) { return addContinent(name, 0); }

    Territory* addTerritory(const std::string& name, int continentId, int x, int y,
        const std::vector<std::string>& neighbors);

    bool validate(std::ostream& diag) const;
    void linkNeighbors(Territory* a, Territory* b);

    void resolveAllNeighbors();
    friend std::ostream& operator<<(std::ostream& os, const Map& m);

private:
    std::vector<Continent*> continents;
    std::vector<Territory*> territories;

    bool graphConnectedAll() const;
    bool continentConnected(int continentId) const;
};

// ---------- MapLoader ----------
class MapLoader {
public:
    bool load(const std::string& filepath, Map*& outMap, std::ostream& diag);
private:
    void addContinentFromLine(Map* m, const std::string& line);
    void addTerritoryFromeLine(Map* m, const std::string& line);
};
