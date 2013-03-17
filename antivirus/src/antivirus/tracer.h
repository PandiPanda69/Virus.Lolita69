#ifndef __ANTIVIR_TRACER_H__
#define __ANTIVIR_TRACER_H__

#include <exception>
#include <string>
#include <map>
#include <set>
#include <vector>

#define MAX_BUFFER_SIZE	4096

#define ANTIVIR_TRACER_HANDLER	bool(*)(pid_t, struct user_regs_struct&)
#define ANTIVIR_CHILD_EXEC_FAILED_CODE	42

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

			const char* what() const throw() { return _msg.c_str(); }

		private:
			std::string _msg;
	};

	/**
	* Traces execution of a program by inspecting every system calls based on <em>ptrace</em>
	*/
	class Tracer
	{
		public:
			typedef std::string syscall_name;

			static Tracer* get_instance();
			virtual ~Tracer();

			void add_handler(syscall_name syscall, bool (*handler)(pid_t, struct user_regs_struct&));

			void trace_me() throw(TracerException);
			void trace_it(pid_t pid) throw(TracerException);

			static void output_regs(std::ostream& out, const struct user_regs_struct& regs);
			static size_t read_string_at(long addr, char* buffer, size_t buf_len, pid_t pid) throw(TracerException);
			static std::vector<std::string> read_string_array_at(long addr, pid_t pid) throw(TracerException);

		private:
			std::multimap<syscall_name, ANTIVIR_TRACER_HANDLER> _handler_mapping;
			std::set<pid_t> _traced_process;

			Tracer();

			bool _trace_syscall(pid_t pid);
			void _trace_subprocess(pid_t pid);

			long _retrieve_syscall_id(pid_t pid);


			static bool _clone_handler(pid_t pid, struct user_regs_struct& regs);
	};
}

#endif
