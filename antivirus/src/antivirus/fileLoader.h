/*
 * fileLoader.h
 *
 *  Created on: 17 mars 2013
 *      Author: remi
 */

#include "fileCommon.h"
#include <fstream>
#include <cstddef>

#ifndef FILELOADER_H_
#define FILELOADER_H_

#define WRONG_ARGUMENTS -2
#define NO_W_FOUND -1

class FileLoader {
public:
	FileLoader(const char* file);
	virtual ~FileLoader();

	/*
	 * looks for the string wToFind (whose size is given by wToFindSize)
	 * from the character which index is given as a parameter [from] and
	 * returns the value of this index within the loaded file.
	 *
	 * @param wToFind: the word to find in the file'c content.
	 * @param wToFindSize: the length of wToFind
	 * @param from: the index in the file the word has to be found from.
	 */
	int getIndex(const char * wToFind, int wToFindSize, unsigned int from);

	/*
	 * Generates the file from the _file loaded
	 *
	 * @param filename: name of the file generated
	 */
	virtual void generate(std::string name);

protected:
	// the loaded file
	byte * _file;
	// size of _file
	size_t _fileSize;

};

#endif /* FILELOADER_H_ */
