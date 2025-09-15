#ifndef MAP_H
#define MAP_H

#include <string>
#include <ostream>

class Map; // here we have a forward declaration basically telling the compiler we have a class named Map
class MapLoader {
public:
	bool load(const std::string& filepath, Map*& outMap, std::ostream& diag);
	// we are adding a & to string because dont want to copy the string but just get the reference, this should be bit more efficient
	//Map* a pointer to a Map object (created in the heap) because we will use it later 
		// we add a & to refer to the caller's pointer variable itself (thus not a direct copy)
	// again the & is used because we want to write directly to the object itself and not just a copy
};


#endif

