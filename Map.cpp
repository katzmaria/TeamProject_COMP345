#include "Map.h"
#include <fstream>
#include <string>
#include <ostream>
#include <iostream>


bool continentFlag = false;

bool MapLoader::load(const std::string& filepath, Map*& outMap, std::ostream& diag) {
	outMap = nullptr; // we dont have anything to build right now 

	std::ifstream in(filepath); //here we are using the ifstream version because we will only read

	if (!in) {
		diag << "[loader] could not open your file: " << filepath << "\n"; 
		return false;
	} 

	int total = 0; 
	std::string line; 

	Map* m = new Map();
	int nextContId = 0;

	while (std::getline(in, line)) { // here we are using the getLine() method to read line by line what is happening
		if (line == "[Continents]"){
			std::cout << "we found the continent! \n \n";
			continentFlag = true;
			continue;
		}

		if (continentFlag == true && !line.empty()) {
			addContinentFromLine(m, line);
		}
		else {
			continentFlag = false;
		}

		++total;
	}

	outMap = m;

  // std::cout << outMap->continents[0].continentName << "\n";


	diag << "[loader] opened OK. total lines = " << total << "\n";
	return true;
}

void MapLoader::addContinentFromLine(Map* m, const std::string& line) {
	std::string continentName = "";

	for (char ch : line) {
		if (ch != '=') {
			continentName += ch;
		}
		else {
			break;
		}

	}
	// continent should be good
	std::cout << "adding continent ----> " << continentName << " \n";
	int id = (*m).addContinent(continentName);
}

int Map::addContinent(const std::string& continentName) {
	int id = static_cast<int>(continents.size());

	Continent c; 
	c.continentID = id; 
	c.continentName = continentName;

	continents.push_back(c);
	return id;
}

// implementation of the destructor
Map::~Map() {
	for (Territory* t : territories) {
		delete t;
	}
	territories.clear(); 

	// Free every Continent we own
	for (Continent* c : continents) {
		delete c;           // release the heap object
	}
	continents.clear();      // drop the pointer entries
}



// simple copy constructor for now
Map::Map(const Map& other) {
	continents.reserve(other.continents.size()); // here we are reserving the exact space needed to create a deep copy

	//oc is the local var that is holding the pointer to the current continent being looped through
	for (const Continent* oc : other.continents) { 
		continents.push_back(new Continent(*oc)); //copie the values for the given continent, oc is pointing to the obj and we taking obj directly because of *
	}

	territories.reserve(other.territories.size());

	for (const Territory* ot : other.territories) {
		Territory* nt = new Territory(*ot);  // copies id, name, coords, continentID
		nt->neighbors.clear();               // don’t point to other's objects
		territories.push_back(nt);
	}

}