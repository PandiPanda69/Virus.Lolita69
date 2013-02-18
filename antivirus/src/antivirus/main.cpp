#include <fstream>
#include <iostream>

#include "core.h"

using namespace std;

namespace antivirus
{
	/**
	* Display how to use the app
	* @param filename Executable name
	*/
	void usage(char* filename)
	{	
		cout << "Usage :" << endl;
		cout << filename << " <file_to_check>" << endl;
		cout << "\tfile_to_check: File to be checked by the anti-virus" << endl << endl;	
	}
}

/**
* Print passed result as a string
* @param res Result to be printed
*/
void print_check_result(antivirus::Core::E_RETURN_CODE res)
{
	switch(res)
	{
		case antivirus::Core::E_FAILED:
			cout << "Failed." << endl;
			break;
		case antivirus::Core::E_VIRUS_SPOTTED:
			cout << "Infected." << endl;
			break;
		case antivirus::Core::E_LOOKS_CLEAN:
			cout << "Clean." << endl;
			break;
		default:
			cout << "Weirdo." << endl;
	}
}

/**
* Entry point
*/
int main(int argc, char** argv)
{
	// Check arguments count
	if( argc != 2 )
	{
		antivirus::usage(argv[0]);
		return 1;
	}

	// Check passed file exists
	ifstream file(argv[1]);
	if(!file)
	{
		cerr << "Cannot open file \"" << argv[1] << "\"." << endl;
		return 2;
	}

	file.close();

	// Initialize antivirus and run checks
	antivirus::Core::E_RETURN_CODE res;
	antivirus::Core core;

	res = core.perform_static_check(argv[1]);
	print_check_result(res);


	res = core.perform_dynamic_check(argv[1]);
	print_check_result(res);

	return 0;
}

