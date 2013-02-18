#include <fstream>
	#include <iostream>

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

		// Add signatures into database
		return _parse_content(content);
	}

	/**
	* Decrypt database content
	* @param content Content to be decrypted
	* @param length Length of the passed content
	*/
	void Database::_decrypt_content(unsigned char* content, size_t length) const
	{
		for( size_t i = 0; i < length; i++ )
		{
			content[i] ^= _key[i%_key_length];
		}
	}

	/**
	* Populate database from plain text.
	* Format must respect i18n standard : key=value
	* @param content Content to be parsed
	* @return True if everything went ok, false otherwise
	*/
	bool Database::_parse_content(unsigned char* content)
	{
		string original((const char*) content);
		bool must_exit = false;
		size_t next_lf = 0;
		size_t next_eq = 0;

		string key, value;

		do
		{
			// Find the next =. If not found, line mays be empty or is invalid. Skip it!
			next_eq = original.find( '=', next_lf );
			if( next_eq == string::npos )
			{
				next_lf = original.find( '\n', next_lf );

				// Hmmm... it's the end of file so exit
				if( next_lf == string::npos )
					must_exit = true;
			}
			else
			{
				// Extract the key
				key = original.substr( next_lf, (next_eq - next_lf) );

				// Find the next LF. If not found, set it to the end of file.
				next_lf = original.find( '\n', next_lf );
				if( next_lf == string::npos )
				{
					next_lf = original.size();
					must_exit = true;
				}
			
				// Extract attached value, ignoring the = (next_eq+1) and the \n (next_lf-1)
				value = original.substr( next_eq+1, (next_lf - 1 - next_eq) );
	
				// Push it into the multimap!
				_signatures.insert( pair<string, string>( key, value ) );
			}

		} while( must_exit );

		return true;
	}
}
