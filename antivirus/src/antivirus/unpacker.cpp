/*
 * unpacker.cpp
 *
 *  Created on: 10 mars 2013
 *      Author: Clément, remi, Sébastien, Vincent
 *
 *      Implementation of the class Unpacker
 */

#include "unpacker.h"

antivirus::Unpacker::Unpacker(std::string filename) :
		_filename(filename) { }

antivirus::Unpacker::~Unpacker() { }

void antivirus::Unpacker::execute(std::string output, bool unscramble) {

	std::string tempFile = _filename;

	// tries to convert file if it is a modified upx
	if (unscramble) {
		UpxUnscrambler * unsc = new UpxUnscrambler(_filename.c_str());
		unsc->unscramble();
		// redefine the file to convert
		tempFile = _filename + "-temp";
		unsc->generate(tempFile.c_str());
		delete unsc;
	}

	// generating default name if output is empty
	if (output.empty())
		output = _filename + UNPACKED_SUFFIX;

	// unpacking file
	std::string command = "upx -d " + tempFile + " -o " + output;
	system(command.c_str());

	if (unscramble) {
		std::string remove = "rm " + tempFile;
		system(remove.c_str());
	}
}

