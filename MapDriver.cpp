#include "Map.h"
#include <iostream>
#include <sstream>

void testLoadMaps() {
    MapLoader loader;
    Map* m = nullptr;

    // point this at one of your .map files
    const std::string path = "maps/002_I72_X-29.map";

    if (!loader.load(path, m, std::cout)) {
        std::cout << "load failed for: " << path << "\n";
        return;
    }

    std::ostringstream diag;
    bool ok = m->validate(diag);  // validate() should call graphConnectedAll() inside

    std::cout << "map fully connected ----> " << (ok ? "true" : "false") << "\n";
    if (!ok) std::cout << diag.str();

    std::cout << *m;

    delete m; // avoid leak
}
