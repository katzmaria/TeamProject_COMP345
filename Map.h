#ifndef MAP_H
#define MAP_H

#include <string>
#include <vector>
#include <ostream>

struct Territory {
	int id = -1;
	int continentID = -1;
	std::string name = "";
	int x;
	int y;
	std::vector<std::string> neighborsNames;
};

struct Continent {
	int continentID = -1;
	std::string continentName = "";
	Continent(const Continent& other);
	Continent& operator=(const Continent& other); // explicit copy assign
	friend std::ostream& operator<<(std::ostream& os, const Continent& c);
};


class Map {
public:
	std::vector<Territory*> territories;
	std::vector<Continent*> continents;

	Map() = default; // This is the default constructor, basically use the compilers default values which is empty
	Map(const Map&); // This is the copy constructor, will run when you want to make a copy of a map
	~Map() // this is the destructor


public:
	int addContinent(const std::string& continentName); // here the & operator is used because we want to use the same string and not a copy

};


class MapLoader {
	void addContinentFromLine(Map* m, const std::string& line);
public:
	bool load(const std::string& filepath, Map*& outMap, std::ostream& diag);
	// we are adding a & to string because dont want to copy the string but just get the reference, this should be bit more efficient
	//Map* a pointer to a Map object (created in the heap) because we will use it later 
		// we add a & to refer to the caller's pointer variable itself (thus not a direct copy)
	// again the & is used because we want to write directly to the object itself and not just a copy
};



#endif

