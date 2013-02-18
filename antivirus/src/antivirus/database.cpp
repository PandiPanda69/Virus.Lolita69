#include <fstream>

#include "database.h"

namespace antivirus
{
	/**
	* Constructor
	*/
	Database::Database()
	{
		// Initializes key
		_key_length = 3;

		_key = new unsigned char[_key_length];

		_key[0] = 0xB;
		_key[1] = 0xA;
		_key[2] = 0xD;
	}

	/**
	* Destructor
	*/
	Database::~Database()
	{
		delete [] _key;
	}

	/**
	* Read database from a physical file
	* @param filename File to read
	* @return True if database has been read, false otherwise
	*/
	bool Database::read(const string& filename)
	{
		ifstream stream(filename.c_str(), ios_base::binary );

		// Check file has been opened
		if(stream.is_open() == false)
			return false;

		// Get file length
		int length;

		stream.seekg(0, ios::end);
		length = stream.tellg();
		stream.seekg(0, ios::beg);

		// Read file content
		unsigned char* content = new unsigned char[length];
		stream.read((char*) content, length);

		stream.close();

		// Decrypt file content
		_decrypt_content(content, length);

		// TODO Add signatures into database


		return true;
	}

	/**
	* Decrypt database content
	* @param content Content to be decrypted
	* @param length Length of the passed content
	*/
	void Database::_decrypt_content(unsigned char* content, size_t length)
	{
		for( size_t i = 0; i < length; i++ )
		{
			content[i] ^= _key[i%_key_length];
		}
	}
}
