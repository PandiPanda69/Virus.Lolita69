#include <fstream>
#include <iostream>
#include <cstring>

#include "core.h"


#define ANTIVIR_PROCESSUS_REPLACEMENT "la_bite_a_lulu"

using namespace std;


typedef enum
{
	E_LEGITIMATE = 0,
	E_VIRUS,
	E_FAILURE,
	E_USAGE,
	E_FILE_NOT_FOUND,
	E_SIGNATURES_NOT_FOUND,
	E_UNKNOWN
} APP_RETURN_CODE;

namespace antivirus
{
	// Application name
	string APP_NAME;
	string TARGET_NAME;

	/**
	* Display how to use the app
	* @param filename Executable name
	*/
	void usage(char* filename)
	{	
		cout << "Usage :" << endl;
		cout << filename << " <file_to_check>" << endl;
		cout << "\tfile_to_check:\tFile to be checked by the anti-virus" << endl << endl;	
	}

	/**
	* Change the name of the processus, so it cannot be found when running a command such ps or top.
	* @param addr Address of the processus name (argv[0])
	*/
	void change_process_name(char* addr)
	{
		::strncpy(addr, ANTIVIR_PROCESSUS_REPLACEMENT, strlen(addr));
	}
}

/**
* Print passed result as a string
* @param res Result to be printed
* @return Corresponding code
*/
APP_RETURN_CODE print_check_result(antivirus::Core::E_RETURN_CODE res)
{
	switch(res)
	{
		case antivirus::Core::E_FAILED:
			cout << "Failed." << endl;
			return E_FAILURE;
		case antivirus::Core::E_VIRUS_SPOTTED:
			cout << "Virus" << endl;
			return E_VIRUS;
		case antivirus::Core::E_LOOKS_CLEAN:
			cout << "Legitimate" << endl;
			return E_LEGITIMATE;
	}

	return E_UNKNOWN;
}

/**
* Entry point
*/
int main(int argc, char** argv)
{
	// Check arguments count (the 3rd parameters allow to specify an external signatures db)
	if( argc < 2 || argc > 3)
	{
		antivirus::usage(argv[0]);
		return E_USAGE;
	}

	antivirus::APP_NAME    = argv[0];
	antivirus::TARGET_NAME = argv[1];

	// Change the process name not to be recognized by viruses.
	antivirus::change_process_name(argv[0]);

	// Check passed file exists
	ifstream file(argv[1]);
	if(!file)
	{
		cerr << "Cannot open file \"" << argv[1] << "\"." << endl;
		return E_FILE_NOT_FOUND;
	}

	file.close();

	// Initialize antivirus and run checks
	APP_RETURN_CODE ret;
	antivirus::Core::E_RETURN_CODE res;
	antivirus::Core* core;

	// If an external database has been specified
	if( argc == 3 )
		core = new antivirus::Core(argv[2]);
	else
		core = new antivirus::Core;

	try
	{
		res = core->perform_dynamic_check(argv[1]);
	}
	catch(...)
	{
		res = antivirus::Core::E_FAILED;
	}

	// If the file has not been statically detected as suspicious, performs a dynamic check.
	if( res == antivirus::Core::E_FAILED )
	{
		res = core->perform_static_check(argv[1]);
	}
	ret = print_check_result(res);

	// Release memory
	delete core;

	return ret;
}

