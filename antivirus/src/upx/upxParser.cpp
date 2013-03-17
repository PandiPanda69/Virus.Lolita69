/*
 * upxParser.cpp
 *
 *  Created on: 14 mars 2013
 *      Author: remi
 */

#include <fstream>
#include <ios>
#include <iostream>
#include "upxParser.h"
#include "upxCsts.h"
#include <cstring>
#include <ucl/ucl.h>
#include <cstdlib>


// -------------------------------------------------------------------------
// --------------------------------------------------------------- constants

#define NO_COMP_FILE -1


// -------------------------------------------------------------------------
// ------------------------------------------ internal functions definitions

unsigned long _btol(int nbChar, const byte* byteNb);
void _generateFile(byte * byteArray, long byteArraySize, char * filename);


// -------------------------------------------------------------------------
// ------------------------------------------------------------ constructors

// Parses the file into an array of bytes, loaded into the attribute _file.
antivirus::UpxParser::UpxParser(const char * filename) {

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

antivirus::UpxParser::~UpxParser() {
	delete [] _file;
}


// -------------------------------------------------------------------------
// ----------------------------------------------------------- mains methods

// extracts the file from the upxParser object and if filename is
// different of NULL generates the result into the file [outFile]
byte * antivirus::UpxParser::extract(char * outFile) {

	// ---------------------------------------------- select packed file


	// index of the compressed file, located in the current read file (_file)
	long indStartCompFile = NO_COMP_FILE;

	// defines the location of the file compressed into the current upx file
	for (long i = 0; i < (long)_fileSize-3; i++) {
	// -2 because of the matching of the pattern if te if condition

		// looks for the pattern UPX1 or UPX2 or UPX!
		if (_file[i] == 'U'
				&& _file [i+1] == 'P'
				&& _file[i+2] == 'X'
				&& (_file[i+3] == '1'
						|| _file[i+3] == '2'
						|| _file[i+3] == '!')) {
			// we already know the 2 next values:
			i += 3;

			// looks for the beginning of the elf file contained inside
			while (i < (long)_fileSize-3) {
				if (_file[i] == 0x7F
						&& _file [i+1] == 'E'
						&& _file[i+2] == 'L'
						&& _file[i+3] == 'F') {

					indStartCompFile = i;
					break; // ".ELF" found: leave the while

				}
				i++;
			}
			break; // "UPX1" found: leave the for
		}
	}

	if (indStartCompFile == NO_COMP_FILE) {
		// there is no compressed .elf file within this upx file
		std::cout << "not a upx file\n";
		return NULL;
	}


	// ------------------------------------------------ find packed file

	// the calculated size of the compressed file
	long compFileSize = _fileSize - indStartCompFile;

	// select only the file compressed within the upx file
	byte compFile[compFileSize];
	memcpy(compFile, &_file[indStartCompFile], compFileSize);

// ==========================
	// TODO: DELETE
// ==========================
// displays 250 first buffer's chars
for (int j=0;j<250;++j){
	std::cout << compFile[j];
}
std::cout << std::endl;
// ==========================


	// ---------------------------------------------- unpack packed file

	// default (and currently stupid) value
	ucl_uint unpckFileSize = 18665536;//3*compFileSize;
	byte* unpckFile = (byte*) malloc(unpckFileSize*sizeof(byte));

std::cout << "size2: " << unpckFileSize << ";" << compFileSize << std::endl;

// ==========================
	// TODO: DELETE
// ==========================
// why do we have a core dumped there? :
// ==========================

	int res = ucl_nrv2d_decompress_le32(compFile,
								(ucl_uint)compFileSize,
								unpckFile,
								&(unpckFileSize),
								NULL);

std::cout << "size3: " << unpckFileSize << ", res:" << res << std::endl;

	// generates the file
	if (outFile != NULL) {
		_generateFile(unpckFile, (long)unpckFileSize, outFile);
	}

	return unpckFile;
}


// -------------------------------------------------------------------------
// --------------------------------------- internal functions implementation

// convert a word into long
unsigned long _btol(int nbChar, const byte* byteNb) {
	unsigned long ret = 0;
	for (int i = 0; i < nbChar; i++) {
		ret += (unsigned long)byteNb[i] * 256^i;
	}
	return ret;
}

// generates the file named filename, thanks to the  byte
// array byteArray which size is byteArraySize.
void _generateFile(byte * byteArray, long byteArraySize, char * filename) {
	std::ofstream myfile;
	myfile.open (filename);
	for (int i = 0; i < byteArraySize; i++) {
		myfile << byteArray[i];
	}
	myfile.close();
}
