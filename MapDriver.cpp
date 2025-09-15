#include "Map.h"
#include <sstream>
#include <iostream>


void testLoadMaps() {
	MapLoader loader; 
	Map* map = nullptr;
	std::ostringstream diag;

	const char* path = "maps/001_I72_Ghtroc 720.map";

	bool ok = loader.load(path, map, diag);
	std::cout << (ok ? "[OK] " : "[FAIL] ") << diag.str();

}