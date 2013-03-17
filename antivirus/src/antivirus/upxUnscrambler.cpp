/*
 * upxUnscrambler.cpp
 *
 *  Created on: 17 mars 2013
 *      Author: remi
 */

#include "upxUnscrambler.h"
#include "fileLoader.h"
#include <cstdlib>
#include <string>

antivirus::UpxUnscrambler::UpxUnscrambler(const char* filename)
		: FileLoader(filename) {
}

antivirus::UpxUnscrambler::~UpxUnscrambler() {
}

#define UPX_HEADER_LENGH 36 // 4 for UPX1 and 32 that we don't know the meaning

bool antivirus::UpxUnscrambler::unscramble() {
	// looks for the string "(0x7F)ELF" in the file
#define ELF_SIZE 4
	char elf[ELF_SIZE] = {0x7F, 'E', 'L', 'F'};

	// Find the location of the string (starting after the potential upx header)
	int indexInternElf = getIndex(elf, ELF_SIZE, UPX_HEADER_LENGH);

	// check a elf header is packed inside of the current file
	if (indexInternElf != NO_W_FOUND
			&& indexInternElf != WRONG_ARGUMENTS) {
		int upx = indexInternElf - UPX_HEADER_LENGH;
		_file[upx] = 'U';
		_file[upx+1] = 'P';
		_file[upx+2] = 'X';

		return true;
	}

	// "ELF" not found
	return false;
}

void antivirus::UpxUnscrambler::generate(std::string name) {
	FileLoader::generate(name);
	std::string command = "chmod +x " + (name);
	system(command.c_str());
}

