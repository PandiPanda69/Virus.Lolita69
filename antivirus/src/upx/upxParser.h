/*
 * upxParser.h
 *
 *  Created on: 14 mars 2013
 *      Author: remi
 */

#ifndef UPXPARSER_H_
#define UPXPARSER_H_

typedef unsigned char byte;

namespace antivirus {
	class UpxParser {
	public:
		UpxParser(const char * file);
		virtual ~UpxParser();

		// extract the file contained into a upx file and generate
		// it with the name given in parameter.
		byte* extract(char* filename);

	private:
		byte * _file;
		size_t _fileSize;

		// generates the file named filename, thanks to the  byte
		// array byteArray which size is byteArraySize.
	//	void _generateFile(byte * byteArray, long byteArraySize, char * filename);
	};
}

#endif /* UPXPARSER_H_ */
