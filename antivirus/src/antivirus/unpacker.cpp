/*
 * unpacker.cpp
 *
 *  Created on: 10 mars 2013
 *      Author: Clément, remi, Sébastien, Vincent
 *
 *      Implementation of the class Unpacker
 */

#include "unpacker.h"

namespace antivirus {
	Unpacker::Unpacker(std::string filename) :
			_filename(filename) { }

	Unpacker::~Unpacker() { }

	void Unpacker::execute(std::string output) {

		// generating default name if output is empty
		if (output.empty())
			output = _filename + UNPACKED_SUFFIX;

		// unpacking file
		std::string command = "upx -d " + _filename + " -o " + output;
		system(command.c_str());
	}
}
