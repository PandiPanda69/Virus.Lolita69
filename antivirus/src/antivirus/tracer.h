#ifndef __ANTIVIR_TRACER_H__
#define __ATTIVIR_TRACER_H__

#include <exception>
#include <string>
#include <map>

#define ANTIVIR_TRACER_HANDLER	bool(*)(pid_t, struct user_regs_struct&)

struct user_regs_struct;

namespace antivirus
{
	/**
	* Exception throw by Tracer class when an error occurs
	*/
	class TracerException: public std::exception
	{
		public:
			TracerException() throw()  { }
			~TracerException() throw() { }

			TracerException(std::string msg) throw() : _msg(msg)  { }

			std::string& what() throw() { return _msg; }

		private:
			std::string _msg;
	};

	/**
	* Traces execution of a program by inspecting every system calls based on <em>ptrace</em>
	*/
	class Tracer
	{
		public:
			Tracer();
			virtual ~Tracer();

			void add_handler(std::string syscall, bool (*handler)(pid_t, struct user_regs_struct&));

			void trace_me() throw(TracerException);
			void trace_it() throw(TracerException);

		private:
			std::map<std::string, ANTIVIR_TRACER_HANDLER> _handler_mapping;

			bool _trace_syscall(pid_t pid);

			long _retrieve_syscall_id(pid_t pid);

	};
}

#endif
