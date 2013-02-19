#include <iostream>

#include "core.h"
#include "filereader.h"

namespace antivirus
{
	/**
	* Constructor
	*/
	Core::Core()
	{
		bool res = _database.readAppendedData();
		if( res == false )
		{
			std::cerr << "Cannot read database." << std::endl;	
		}
	}

	/**
	* Constructor allowing user to specify database_file
	* @param database_file	File to be used as database
	*/
	Core::Core(const std::string& database_file)
	{
		bool res = _database.read(database_file);
		if( res == false )
		{
			std::cerr << "Cannot read database." << std::endl;	
		}
	}

	/**
	* Destructor
	*/
	Core::~Core()
	{
	}

	/**
	* Perform a static check, verifying signatures
	* @param filename Filename of the file to check
	* @return E_FAILED if something went wrong, E_VIRUS_SPOTTED if this is a virus, or E_LOOKS_CLEAN if it's a regular file
	*/
	Core::E_RETURN_CODE Core::perform_static_check(const std::string& filename)
	{
		// Read file content
		FileReader reader(filename);

		try {
			reader.read();
		}
		catch(FileReaderException ex)
		{
			cerr << ex.what() << endl;
			return E_FAILED;
		}

		// Check if signatures matches
		bool matches = _check_signatures(reader.getFileData());

		return matches ? E_VIRUS_SPOTTED : E_LOOKS_CLEAN;
	}

	/**
	* Perform a dynamic check, verifying behaviour at runtime
	* @param filename Filename of the file to check
	* @return E_FAILED if something went wrong, E_VIRUS_SPOTTED if this is a virus, or E_LOOKS_CLEAN if it's a regular file
	*/
	Core::E_RETURN_CODE Core::perform_dynamic_check(const std::string& filename)
	{
		return E_FAILED;
	}

	/**
	* Check if the file matches with the various signatures contained in database.
	* @param data	Data of the file to check
	* @return True if one matches, false otherwise
	*/
	bool Core::_check_signatures(FileData* data)
	{
		std::multimap<string, string> database;
		std::multimap<string, string>::const_iterator it;

		database = _database.getSignatures();

		bool matched = false;
		for( it = database.begin(); it != database.end() && !matched; it++ )
		{
			matched = data->check_signature((*it).second);
		}
		
		return matched;
	}
}
