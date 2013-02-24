#ifndef __ANTIVIR_DATABASE_H__
#define __ANTIVIR_DATABASE_H__

#include <map>
#include <exception>

using namespace std;

namespace antivirus
{
	class DatabaseException: public exception
	{
		public:
			DatabaseException() throw() {}
			~DatabaseException() throw() {}
	};

	/**
	* Class representing virus signatures database
	*/
	class Database
	{
		public:
			Database();
			virtual ~Database();

			void readAppendedData() throw(DatabaseException);
			void read(const string& filename) throw(DatabaseException);

			inline const multimap<string, string>& getSignatures() { return _signatures; }

		private:
			std::multimap<string, string>	_signatures;

			unsigned char* _key;
			size_t _key_length;

			void _decrypt_content(unsigned char* content, size_t length) const;
			void _parse_content(unsigned char* content);
	};
}

#endif
