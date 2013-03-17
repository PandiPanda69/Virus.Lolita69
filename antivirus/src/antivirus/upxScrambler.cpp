/*
 * UpxScrambler.cpp
 *
 *  Created on: 17 mars 2013
 *      Author: remi
 */

#include "upxScrambler.h"
#include "fileLoader.h"
#include <cstdlib>
#include <string>


antivirus::UpxScrambler::UpxScrambler(const char* filename)
		: FileLoader(filename) {
}

antivirus::UpxScrambler::~UpxScrambler() {
}

bool antivirus::UpxScrambler::scramble() {
	int indexUPX = getIndex("UPX1", 4, 4);
	if (indexUPX == NO_W_FOUND
			|| indexUPX == WRONG_ARGUMENTS) {
		indexUPX = getIndex("UPX2", 4, 4);
		if (indexUPX == NO_W_FOUND
				|| indexUPX == WRONG_ARGUMENTS) {
			indexUPX = getIndex("UPX!", 4, 4);
		}
	}
	// if UPX* found
	if (indexUPX != NO_W_FOUND
			|| indexUPX != WRONG_ARGUMENTS) {
		// define the number of letters to change
		int nbLetters = rand()%3;
		for (int i = 0; i <= nbLetters; i++){
			// erase a letter:
			_file[indexUPX+i] = 'A';
		}
		return true;
	}
	return false;
}

void antivirus::UpxScrambler::generate(std::string name) {
	FileLoader::generate(name);
	std::string command = "chmod +x " + (name);
	system(command.c_str());
}

