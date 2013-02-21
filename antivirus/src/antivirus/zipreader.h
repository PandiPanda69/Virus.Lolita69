#ifndef __ANTIVIRUS__ZIP_READER_H__
#define __ANTIVIRUS__ZIP_READER_H__

#include <exception>
#include <string>
#include <list>
#include <zip.h>

#include "filedata.h"


namespace antivirus
{
	/**
	* Exception thrown by ZipReader class
	*/
	class ZipReaderException: public std::exception
	{
		public:
			ZipReaderException() throw() {}
			ZipReaderException(const std::string& msg) throw() : _msg(msg) {}
			~ZipReaderException() throw() {}
			const std::string& what() throw() { return _msg; }

		private:
			std::string _msg;
	};

	
	/**
	* Class handling zip files
	*/
	class ZipReader
	{
		public:
			ZipReader(const std::string& filename) throw(ZipReaderException);
			virtual ~ZipReader();

			typedef std::list<const char*>::iterator iterator;

			inline iterator begin() { return _files.begin(); }
			inline iterator end()   { return _files.end(); }

			FileData* read_zipped_file(const char* filename) throw(ZipReaderException);

		private:
			zip*	_zip_desc;
			std::list<const char*> _files;
	};
}

#endif
