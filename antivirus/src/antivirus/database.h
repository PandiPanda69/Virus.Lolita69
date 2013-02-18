#ifndef __ANTIVIR_DATABASE_H__
#define __ANTIVIR_DATABASE_H__

#include <map>

using namespace std;

namespace antivirus
{
	/**
	* Class representing virus signatures database
	*/
	class Database
	{
		public:
			Database();
			virtual ~Database();

			bool read(const string& filename);

			inline const multimap<string, string>& getSignatures() { return _signatures; }

		private:
			std::multimap<string, string>	_signatures;

			unsigned char* _key;
			size_t _key_length;

			void _decrypt_content(unsigned char* content, size_t length);

			
			
	};
}

#endif
