#ifndef __ANTIVIR_CORE_H__
#define __ANTIVIR_CORE_H__

#include <string>
#include "database.h"

namespace antivirus
{
	/**
	* Class controlling antivirus behaviour
	*/
	class Core
	{
		public:
			Core();
			virtual ~Core();

			enum E_RETURN_CODE
			{
				E_FAILED,
				E_VIRUS_SPOTTED,
				E_LOOKS_CLEAN
			};

			E_RETURN_CODE perform_static_check(const std::string& filename);
			E_RETURN_CODE perform_dynamic_check(const std::string& filename);

		private:
			Database _database;
	};
}

#endif
