#ifndef __ANTIVIR_COMMAND_RUNNER_H__
#define __ANTIVIR_COMMAND_RUNNER_H__

#include <string>
#include <map>
#include <exception>

#include <unistd.h>

namespace antivirus
{
	/**
	* Exception thrown by CommandRunner
	*/
	class CommandRunnerException: public std::exception
	{
		public:
			CommandRunnerException(const std::string& msg) throw() :  _msg(msg) {}
			~CommandRunnerException() throw() { }

			const char* what() const throw() { return _msg.c_str(); }

		private:
			std::string _msg;
	};

	/**
	* Class handling process execution
	*/
	class CommandRunner
	{
		public:
			enum FILE_TYPE
			{
				TYPE_ELF,
				TYPE_PYTHON,
				TYPE_PERL,
				TYPE_SHELL,
				TYPE_UNKNOWN
			};		

			struct COMMAND_INFO_STRUCT
			{
				std::string absolute_filename;
				std::string absolute_interpreter;

				FILE_TYPE  type;
			};

			CommandRunner(const std::string& filename);
			~CommandRunner();

			COMMAND_INFO_STRUCT resolve() throw(CommandRunnerException);

		private:
			std::string _filename;
			std::map<FILE_TYPE, std::string> _filetype_mapping;

			void        _initialize_file_type();
			FILE_TYPE  _determine_file_type(const std::string& filename) throw(CommandRunnerException);
			std::string _determine_command_line(const std::string& filename, FILE_TYPE type) throw(CommandRunnerException);

	};
}

#endif
