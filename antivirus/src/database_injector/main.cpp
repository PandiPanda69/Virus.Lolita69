#include <fstream>
#include <iostream>

using namespace std;

// Error codes
enum
{
	E_OK = 0,
	E_USAGE,
	E_APP_NOT_FOUND,
	E_SIGNATURES_NOT_FOUND
};

namespace antivirus
{
	/**
	* Display help on how to use the command.
	* @param app	Application name
	*/
	void usage(const char* app)
	{
		cout << "Append signature database at the end of antivirus." << endl << endl;
		cout << app << " <signature_db> <antivirus_elf> <output>" << endl;
		cout << "\tsignature_db:\tSignature encrypted database file" << endl;
		cout << "\tantivirus_elf:\tAntivirus ELF file" << endl;
		cout << "\toutput:\tFilename where result must be output" << endl << endl;
	}
}

// Entry point
int main(int argc, char** argv)
{
	// Check there are enough parameters
	if( argc != 4 )
	{
		antivirus::usage(argv[0]);
		return E_USAGE;
	}


	// Proceed
	size_t db_length;
	size_t app_length;

	unsigned char* db_content = NULL;
	unsigned char* app_content = NULL;

	db_length = app_length = 0;

	// Read signatures database
	ifstream signatures(argv[1], ios_base::binary);
	if(signatures.is_open() == false)
	{
		cerr << "Cannot open signatures database." << endl;
		return E_SIGNATURES_NOT_FOUND;
	}

	signatures.seekg(0, ios::end);
	db_length = signatures.tellg();
	signatures.seekg(0, ios::beg);

	db_content = new unsigned char[db_length];
	signatures.read((char*) db_content, db_length);

	signatures.close();


	// Read antivirus program
	ifstream antivirus(argv[2], ios_base::binary);
	if(antivirus.is_open() == false)
	{
		cerr << "Cannot open antivirus ELF file." << endl;

		delete [] db_content;

		return E_APP_NOT_FOUND;
	}

	antivirus.seekg(0, ios::end);
	app_length = antivirus.tellg();
	antivirus.seekg(0, ios::beg);

	app_content = new unsigned char[app_length];
	antivirus.read((char*) app_content, app_length);

	antivirus.close();


	// Write antivirus binary data, then signatures database content ended by 0x00. Finally, write the original file length.
	ofstream final(argv[3], ios_base::binary);

	final.write((const char*) app_content, app_length);
	final.write((const char*) db_content, db_length);
	final.write("\0", 1);
	final.write((const char*) &app_length, sizeof(size_t));

	final.close();

	// Release memory
	delete [] db_content;
	delete [] app_content;

	return E_OK;
}
