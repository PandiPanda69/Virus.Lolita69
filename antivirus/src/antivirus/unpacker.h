/*
 * unpacker.h
 *
 *  Created on: 10 mars 2013
 *      Author: Clément, remi, Sébastien, Vincent
 *
 *      The purpose of this class is to unpack files packed using upx.
 *      upx web site: http://upx.sourceforge.net/
 */
#include <stdlib.h>
#include <iostream>
#include <string.h>

#include "upxUnscrambler.h"


#ifndef UNPACKER_H_
#define UNPACKER_H_

// default suffix when generating a unpacked file
#define UNPACKED_SUFFIX "-unpacked"

namespace antivirus {
	class Unpacker {
		private:
			/* the name of the file the unpacker will unpack */
			std::string _filename;
		public:
			/*
			 * Creates a upx Unpacker object.
			 *
			 * @param filename: the name of the file the Unpacker will
			 * unpack.
			 */
			Unpacker(std::string filename);
			virtual ~Unpacker();

			/*
			 * Unpacks of the unpacker's file and generates the result
			 * into the file which name is given in the parameter output.
			 *
			 * @param output: the name of the output file the unpacker
			 * 		will unpack in.
			 * 		If output is empty, the generated file is
			 * 		_filename + UNPACKED_SUFFIX.
			 * @param unscramble: define whether the execute will make sure
			 * that the file is an upx file or not.
			 */
			void execute(std::string output, bool unscrambled = false);
	};
}
#endif /* UNPACKER_H_ */
