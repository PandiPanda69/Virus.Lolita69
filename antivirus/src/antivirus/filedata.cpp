#include <iostream>
#include <sstream>

#include <boost/regex.hpp>

#include "filedata.h"

namespace antivirus
{
	/**
	* Constructor
	*/
	FileData::FileData() : _content()
	{
	}

	/**
	* Constructor
	* @param str File content
	* @param length File length
	* @param is_binary Is content binary? If so it's gonna be translated in human readable characters
	*/
	FileData::FileData(const unsigned char* str, size_t length, bool is_binary )
	{
		if( is_binary )
			_convert_from_binary_to_str(str, length);
		else
			_content = std::string((const char*) str);
	}

	/**
	* Destructor
	*/
	FileData::~FileData()
	{
	}

	/**
	* Convert a binary content to a human readable hexadecimal view
	* @param str Binary content to be translated
	* @param length Length of the content
	*/
	void FileData::_convert_from_binary_to_str(const unsigned char* str, size_t length)
	{
		std::stringstream ss;

		for(size_t i = 0; i < length; i++)
		{
			ss.width(2);
			ss.fill('0');
			ss << std::hex << std::right << (int) str[i];
		}

		ss >> _content;
	}

	/**
	* Try to find the passed signature into this file applying a regex
	* @param signature Signature to be matched
	* @return True if signature matched, false otherwise
	*/
	bool FileData::check_signature(const std::string& signature)
	{
		boost::cmatch what;

		return boost::regex_match( _content.c_str(), what, boost::regex( signature ) );
	}
}
