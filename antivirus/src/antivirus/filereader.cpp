#include <fstream>

#include "filereader.h"

namespace antivirus
{
	/**
	* Constructor
	*/
	FileReader::FileReader(const std::string& filename) : _data( NULL ), _filename(filename)
	{
	}

	/**
	* Destructor
	*/
	FileReader::~FileReader()
	{
		// Release memory if needed
		if( _data != NULL )
			delete _data;
	}

	/**
	* Read file content
	*/
	void FileReader::read() throw (FileReaderException)
	{
		// Open file and tests it succeed
		std::ifstream is(_filename.c_str());
		if(is.is_open() == false)
		{
			throw FileReaderException("File cannot be accessed.");
		}

		// Get file length
		size_t length;

		is.seekg(0, std::ios::end);
                length = is.tellg();
                is.seekg(0, std::ios::beg);

                // Read file content
                unsigned char* content = new unsigned char[length];
                is.read((char*) content, length);

		is.close();

		// Initialize FileData
		_data = new FileData(content, length, true);

		// Release memory
		delete [] content;
	}
}
