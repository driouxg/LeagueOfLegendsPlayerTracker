#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class FileManager {

public:
	inline void GetSumNames(std::string fileName, std::vector<std::string>& sumNames);
};


/**
 * Stores the names of players in a vector and returns the vector.
 */
void FileManager::GetSumNames(std::string fileName, std::vector<std::string>& sumNames) {

	std::string line;

	std::ifstream myfile(fileName);
	if (myfile.is_open()) {
		while (std::getline(myfile, line))
			sumNames.push_back(line);
	}
	else
		std::cout << fileName << " did not open correctly." << std::endl;
}