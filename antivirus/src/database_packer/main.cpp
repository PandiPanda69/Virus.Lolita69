#include <iostream>
#include <fstream>


using namespace std;

enum 
{
	E_OK = 0,
	E_USAGE,
	E_INPUT_NOT_FOUND
};

namespace antivirus
{
	// Key used to (de)crypt data
	const unsigned char KEY[] = { 0xB, 0xA, 0xD };
	const size_t KEY_LENGTH = 3;

	/**
	* Display usage
	* @param app Application file name
	*/
	void usage(char* app)
	{
		cout << "Tools (de)crypting signatures database using a simple XOR." << endl << endl;

		cout << "Usage:" << endl;
		cout << app << " <input_db> <output_db>" << endl;
		cout << "\tinput_db:\tDatabase that must be (de)crypted" << endl;
		cout << "\toutput_db:\tResult of the operation" << endl;

		cout << endl;
	}

	/**
	* Encrypt/Decrypt content
	* @param content Content to be encrypted/decrypted
	* @param length Length of content array
	*/
	void crypt(unsigned char* content, size_t length)
	{
		for(size_t i = 0; i < length; i++)
			content[i] ^= KEY[i%KEY_LENGTH];
	}
}

/**
* Read a file content
* @param is File stream
* @return Pointer to allocated content
*/
size_t read_file(ifstream& is, unsigned char** content)
{
	size_t length;

	// Get file length;
 	is.seekg(0, ios::end);
      	length = is.tellg();
	is.seekg(0, ios::beg);

	// Read file content
	*content = new unsigned char[length];
	is.read((char*) *content, length);

	return length;
}

/**
* Entry point
*/
int main(int argc, char** argv)
{
	if( argc != 3 )
	{
		antivirus::usage(argv[0]);
		return E_USAGE;
	}

	// Processing
	cout << "Opening " << argv[1] << "... ";
	ifstream is(argv[1], ios_base::binary );

	if(is.is_open() == false)
	{
		cerr << "Input cannot be found." << endl;
		return E_INPUT_NOT_FOUND;
	}

	cout << "[OK]" << endl;
	cout << "Reading content... ";

	unsigned char* content = NULL;
	size_t length = read_file(is, &content);

	is.close();

	cout << "[OK]" << endl;
	cout << "(De)crypt... ";

	// Encrypt/Decrypt
	antivirus::crypt(content, length);

	cout << "[OK]" << endl;
	cout << "Save content... ";
	
	// Writes output
	ofstream os(argv[2], ios_base::binary);
	os.write((const char*) content, length);
	os.close();

	cout << "[OK]" << endl;

	delete [] content;

	return E_OK;
}
