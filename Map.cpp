#include "Map.h"
#include <fstream>
#include <string>
#include <ostream>


bool MapLoader::load(const std::string& filepath, Map*& outMap, std::ostream& diag) {
	outMap = nullptr; // we dont have anything to build right now 

	std::ifstream in(filepath); //here we are using the ifstream version because we will only read

	if (!in) {
		diag << "[loader] could not open your file: " << filepath << "\n"; 
		return false;
	} 

	int total = 0; 
	std::string line; 

	while (std::getline(in, line)) { // here we are using the getLine() method to read line by line what is happening
		++total;
	}

	diag << "[loader] opened OK. total lines = " << total << "\n";
	return true;


}
