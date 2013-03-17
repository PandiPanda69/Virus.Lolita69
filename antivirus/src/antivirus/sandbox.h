#ifndef __ANTIVIR_SANDBOX_H__
#define __ANTIVIR_SANDBOX_H__

#include <string>
#include <exception>
#include <vector>

#include <sys/user.h>

#define __ANTIVIR_LDD_RES_TMP_FILE	"._ldd_res"

namespace antivirus
{
	class SandBoxException: public std::exception
	{
		public:
			SandBoxException() throw()  { }
			~SandBoxException() throw() { }

			SandBoxException(const std::string& msg) throw() : _msg(msg) { }

			std::string& what() throw() { return _msg; }

		private:
			std::string _msg;
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
			SandBox(const std::string& path) throw(SandBoxException);
			virtual ~SandBox();

			void prepare(const std::string& target) throw(SandBoxException);
			void run() throw(SandBoxException);

			void set_additionnal_dependencies(std::vector<std::string> deps) throw(SandBoxException);
			void set_additionnal_files(std::vector<std::string> files) throw(SandBoxException);

			void initialize_tracer() throw(SandBoxException);

			static std::string get_command_location(const std::string& cmd) throw(SandBoxException);

		private:
			static SandBox* _current_instance;

			std::string _path;
			std::string _target;

			void _initialize() throw(SandBoxException);
			void _cleanup()    throw(SandBoxException);

			void _find_deps(std::vector<std::string>* ptr_deps) const throw(SandBoxException);
			void _setup_deps(std::vector<std::string>* ptr_deps) const throw(SandBoxException);

			void _parse_ldd_result(const std::string& raw_file, std::vector<std::string>* ptr_deps) const;

			void _copy_file_and_create_subdirs(const std::string& src, bool _add_execute_flag = false) const;

			static bool _execve_handler(pid_t pid, struct user_regs_struct& regs);
			static bool _stat_access_and_open_handler(pid_t pid, struct user_regs_struct& regs);

	};
}

#endif
