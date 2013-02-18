#include <iostream>

#include "core.h"

namespace antivirus
{
	/**
	* Constructor
	*/
	Core::Core()
	{
		bool res = _database.read("signatures.db"); // TODO Let ppl configures it
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
		return E_FAILED;
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
}
