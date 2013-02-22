#include "zipreader.h"

namespace antivirus
{
	/**
	* Constructor
	* @param filename Zip filename to read
	* @throw ZipReaderException Thrown if the file is not a zip file
	*/
	ZipReader::ZipReader(const std::string& filename) throw(ZipReaderException) : _zip_desc(NULL)
	{
		int err = 0;
		_zip_desc = zip_open(filename.c_str(), 0, &err);

		// If file cannot be opened as a zip, throw an exception
		if(!_zip_desc)
			throw ZipReaderException();

		// Then read every contained filenames and add it to the list
		long num = zip_get_num_files(_zip_desc);

		for(long i = 0; i < num; i++)
        	{
                	const char* current_filename = zip_get_name(_zip_desc, i, 0);
			_files.push_back(current_filename);
		}
	}

	/**
	* Destructor
	*/
	ZipReader::~ZipReader()
	{
		// If zip was still opened, close it
		if(_zip_desc)
		{
			zip_close(_zip_desc);
		}
	}

	/**
	* Read a file into a zip
	* @param filename File name of the contained file to read
	* @return A pointer to the correctly filled File Data
	* @thrown FileReaderException Thrown when the file cannot be read
	*/
	FileData* ZipReader::read_zipped_file(const char* filename) throw(ZipReaderException)
	{
		struct zip_stat st;
                zip_stat_init(&st);
		int ret;

		// Get information about the file, and if it failed, raise an exception
                ret = zip_stat(_zip_desc, filename, 0, &st);
		if( ret == -1 )
			throw ZipReaderException();

		// Allocate memory to read content
                unsigned char* da_file_content = new unsigned char[st.size];

                // And read the file
                zip_file* da_file = zip_fopen(_zip_desc, filename, 0);
		if( !da_file )
		{
			delete [] da_file_content;
			throw ZipReaderException();
		}

                zip_fread(da_file, da_file_content, st.size);
                zip_fclose(da_file);

		// And then, generate FileData
		FileData* data = new FileData(da_file_content, st.size, true);

		// Release memory
                delete [] da_file_content;

		return data;
	}
}
