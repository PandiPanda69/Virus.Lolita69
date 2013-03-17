/*
 * UpxScrambler.h
 *
 *  Created on: 17 mars 2013
 *      Author: remi
 */

#ifndef UPXSCRAMBLER_H_
#define UPXSCRAMBLER_H_

#include "fileLoader.h"

namespace antivirus {
	class UpxScrambler : public FileLoader {
	public:
		UpxScrambler(const char* filename);
		virtual ~UpxScrambler();

		/*
		 * Looks for the first tag 'UPX' in the file
		 * and writes 'A's instead of some random
		 * characters of the flag
		 */
		bool scramble();

		/*
		 * Generates the file loaded into the file whose
		 * name is given as a parameter
		 *
		 * @param name: the name of the file to generate.
		 */
		void generate(std::string);
	};
}

#endif /* UPXSCRAMBLER_H_ */
