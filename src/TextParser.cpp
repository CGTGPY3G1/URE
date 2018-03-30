#include "TextParser.h"
#include <fstream>
#include <iostream>

std::string TextParser::ReadFile(const std::string & location) {
	std::string contents = ""; 
	std::string line = "";

	std::ifstream file(location, std::ios::in | std::ios::binary);
	if(file.is_open()) { 
		while(file.good()) { 
			getline(file, line); 
			contents.append(line + "\n");
		}
		file.close();
	}
	else std::cout << "File location " + location + " is invalid!" << std::endl;
	return contents;
}
