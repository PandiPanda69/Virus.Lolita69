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
		_key_length = 11;

		_key = new unsigned char[_key_length];

		_key[0]  = 0xB;
		_key[1]  = 0xA;
		_key[2]  = 0xD;
		_key[3]  = 0xD;
		_key[4]  = 0xE;
		_key[5]  = 0xA;
		_key[6]  = 0xD;;
		_key[7]  = 0xB;;
		_key[8]  = 0xA;;
		_key[9]  = 0xB;;
		_key[10] = 0xE;;
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
	* @throw DatabaseException Thrown if database cannot be read
	*/
	void Database::read(const string& filename) throw(DatabaseException)
	{
		ifstream stream(filename.c_str(), ios_base::binary );

		// Check file has been opened
		if(stream.is_open() == false)
			throw DatabaseException();

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
		_parse_content(content);

		// Release memory
		delete [] content;
	}

	/**
        * Read database appended at the end of the application binary
        * @throw DatabaseException Thrown if database cannot be read
	*/
	void Database::readAppendedData() throw(DatabaseException)
	{
		extern std::string APP_NAME;

		// Open the current running application
		std::ifstream stream(APP_NAME.c_str(), std::ios_base::binary);
		if(stream.is_open() == false)
			throw DatabaseException();		

		// Get theorical end of file using a marker written at the EoF by the injector
		size_t size_length = sizeof(size_t);
		size_t antivirus_length;

		stream.seekg(-((signed int) size_length), std::ios::end);
		stream.read((char*) &antivirus_length, size_length);

		// Determine database size, determining current app length, and applying the simple formula:
		// db_size = total_app_len - antivirus_length - size_length
		size_t current_length;

		stream.seekg(0, std::ios::end);
		current_length = stream.tellg();

		size_t database_length = current_length - antivirus_length - size_length;

		// Memory allocation
		unsigned char* database_content = new unsigned char[database_length];

		// Places cursor at the theorical end to read database (do not re-read length at the end)
		stream.seekg(antivirus_length, std::ios::beg);
		stream.read((char*) database_content, database_length);

		stream.close();

		// Decrypt file content
		_decrypt_content(database_content, database_length);

		// Add signatures into database
		_parse_content(database_content);


		// Release memory
		delete [] database_content;
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
	*/
	void Database::_parse_content(unsigned char* content)
	{
		string original((const char*) content);
		bool must_exit = false;
		size_t next_lf = 0;
		size_t next_eq = 0;

		string key, value;

		do
		{
			// Find the next =. If not found, no interesting lines left so exits
			next_eq = original.find( '=', next_lf );
			if( next_eq == string::npos )
			{
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

				// Why adding 1? Since next_lf currently refers to the last '\n' found, 
				// increments it allows to start seeking in the next line
				next_lf++;
			}

		} while( !must_exit );
	}
}
