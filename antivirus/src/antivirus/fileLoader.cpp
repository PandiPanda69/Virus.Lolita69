/*
 * fileLoader.cpp
 *
 *  Created on: 17 mars 2013
 *      Author: remi
 */

#include "fileLoader.h"

FileLoader::FileLoader(const char * filename) {

	// opens the file
	std::ifstream file;
	file.open(filename);

	if(file.is_open()) {
		// gets the length of the file
		file.seekg(0, std::ios::end);
		_fileSize = file.tellg();
		file.seekg(0, std::ios::beg);

		// initiates the attribute _file
		_file = new byte[_fileSize];

		// reads the file
		file.read((char*)_file, _fileSize);
	}
}

FileLoader::~FileLoader() {
	// TODO Auto-generated destructor stub
}

int FileLoader::getIndex(const char * wToFind,
						int wToFindSize,
						unsigned int from) {
	// the limit the method needs to get throw if it doesn't
	// want to to a index out of bounds exception
	int limit = _fileSize - wToFindSize;

	if (from >= limit) {
		// wrong arguments
		return WRONG_ARGUMENTS;
	}

	// checks for the word:
	for (unsigned int i = from; i < _fileSize; i++) {
		bool matches = true;
		int j=0;

		// while it matches, reading the next char of the test word...
		while (matches && j < wToFindSize) {
			matches &= (_file[i+j] == wToFind[j]);
			j++;
		}
		// ... let's keep going

		// has everything matched?
		if (matches) {
			// yes!
			return i;
		}
		// no -> keepturning in the loop
	}

	// nothing found
	return NO_W_FOUND ;
}

void FileLoader::generate(std::string name) {
	std::ofstream myfile;
	myfile.open (name.c_str());

	for (unsigned int i = 0; i < _fileSize; i++) {
		myfile << _file[i];
	}
	myfile.close();
}
