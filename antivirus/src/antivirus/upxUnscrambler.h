/*
 * upxUnscrambler.h
 *
 *  Created on: 17 mars 2013
 *      Author: remi
 */

#ifndef UPXUNSCRAMBLER_H_
#define UPXUNSCRAMBLER_H_

#include "fileLoader.h"

namespace antivirus {
	class UpxUnscrambler : public FileLoader {
	public:
		UpxUnscrambler(const char* filename);
		virtual ~UpxUnscrambler();
		bool unscramble();

		/*
		 * Generates the file loaded into the file whose
		 * name is given as a parameter
		 *
		 * @param name: the name of the file to generate.
		 */
		void generate(std::string);
	};
}

#endif /* UPXUNSCRAMBLER_H_ */
