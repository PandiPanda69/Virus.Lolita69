#ifndef __ANTIVIR_SANDBOX_H__
#define __ANTIVIR_SANDBOX_H__

#include <string>
#include <exception>

namespace antivirus
{
	class SandBoxException: public std::exception
	{
		public:
			SandBoxException() throw()  { }
			~SandBoxException() throw() { }
	};

	/**
	* A class handling chroot sandboxing by:
	*	- Determining dependencies and resolve them (copy .so files where needed)
	*	- Initializing configurations files such /etc/passwd, /etc/shadow, /root/.bashrc, ...
	*	- Cleaning all the mess up
	*/
	class SandBox
	{
		public:
			SandBox(const std::string& path);
			virtual ~SandBox();

		private:
			std::string _path;

			void _initialize();
	};
}

#endif
