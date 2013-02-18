#ifndef __ANTIVIRUS__FILEREADER_H__
#define __ANTIVIRUS__FILEREADER_H__

#include <string>
#include <exception>

#include "filedata.h"

namespace antivirus
{
	/**
	* Exception that might be thrown by FileReader class
	*/
	class FileReaderException : public std::exception
	{
		public:
			FileReaderException(const std::string& m) throw() : msg(m) {}
			virtual ~FileReaderException() throw() {}

			const char* what() const throw() { return msg.c_str(); }

		private:
			std::string msg;
	};

	/**
	* Class reading a file and put its content into a FileData object
	*/
	class FileReader
	{
		public:
			FileReader(const std::string& filename);
			virtual ~FileReader();

			void read() throw(FileReaderException);

			inline FileData* getFileData() { return _data; }

		private:
			FileData* _data;
			std::string _filename;
	};
}

#endif
