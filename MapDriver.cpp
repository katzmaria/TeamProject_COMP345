// MapDriver.cpp
#include "Map.h"
#include <iostream>
#include <sstream>

void testLoadMaps() {
    MapLoader loader;
    Map* m = nullptr;

    const std::string path = "maps/002_I72_X-29.map";

    std::ostringstream diag;                
    if (!loader.load(path, m, diag)) {       
        std::cout << "Load FAILED: " << path << "\n" << diag.str() << "\n";
        return;
    }

    std::cout << "Load OK: " << path << "\n" << diag.str() << "\n";

    delete m;
}
