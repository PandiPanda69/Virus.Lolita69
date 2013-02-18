#ifndef __ANTIVIRUS_FILEDATA_H__
#define __ANTIVIRUS_FILEDATA_H__

#include <string>

namespace antivirus
{
	/**
	* Class representing file content as hexadecimal readable view (ex: ff0123456789abcdef)
	*/
	class FileData
	{
		public:
			FileData();
			FileData(const unsigned char* str, size_t length, bool is_binary = true);
			virtual ~FileData();

			bool check_signature(const std::string& signature);

		protected:
			std::string _content;

			void _convert_from_binary_to_str(const unsigned char* str, size_t length);
			
	};
}

#endif
