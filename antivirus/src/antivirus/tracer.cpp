#include <sys/ptrace.h>
#include <asm/ptrace-abi.h>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <sys/user.h>

#include <cstring>
#include <cerrno>
#include <cstdio>

#include <vector>

#include "tracer.h"

#include "debug.h"

// Defined in syscall.cpp
extern std::vector<std::string> syscall_names;
void fill_syscall();


namespace antivirus
{
	/**
	* Constructor
	*/
	Tracer::Tracer()
	{
		fill_syscall();
	}

	/**
	* Destructor
	*/
	Tracer::~Tracer()
	{
	}

	/**
	* Associates an handler to a system call.
	* @param syscall System call name
	* @handler Handler to associate
	*/
	void Tracer::add_handler(std::string syscall, bool (*handler)(pid_t, struct user_regs_struct&))
	{
		_handler_mapping.insert(std::pair<std::string, ANTIVIR_TRACER_HANDLER>( syscall, handler ));
	}

	/**
	* Traces the current process
	*/
	void Tracer::trace_me() throw(TracerException)
	{
		int ret;

		ret = ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		if( ret != 0 && errno != 0 )
			throw TracerException("ptrace initialization failed.");

		// Even children must be traced
                /* ret = ptrace(PTRACE_SETOPTIONS, getpid(), NULL, PTRACE_O_TRACEFORK | PTRACE_O_TRACEVFORK | PTRACE_O_TRACECLONE | PTRACE_O_TRACEEXIT);
		if( ret != 0 )
		{
			perror("ptrace option failed");
			throw TracerException(strerror(errno));
		} */
	}

	/**
	* Run the step-by-step analysis.
	* This method isn't asynchronous, it returns when observed process ends.
	*/
	void Tracer::trace_it() throw(TracerException)
	{
		pid_t child;
		bool must_continue;
		int status;

		must_continue = true;
		while(must_continue)
		{
			// Waits until a child sends a signal
			child = waitpid(-1, &status, __WALL);

			// Check if the signal tells us child exited or whatever
			if(WIFEXITED(status))
			{
				TRACE("Child exited.");
				break;
				must_continue = false;
                        }
                        if(WIFSIGNALED(status))
			{
				TRACE("Child killed.");
				break;
				must_continue = false;
                        }
                        if(!WIFSTOPPED(status))
			{
				TRACE("Unknown state.");
				break;
                                must_continue = false;
                        }

			// Check if a syscall is gonna be called
			if(WSTOPSIG(status) == SIGTRAP)
			{
				must_continue = _trace_syscall(child);
			}
			else
			{
				TRACE("Child stopped.");
			}

			// Continue the execution
			ptrace(PTRACE_SYSCALL, child, NULL, NULL );
		}
	}

	/**
	* Trace a system call
	* @param pid Processus id of the process to observe
	* @return True if the tracing must continue, false otherwise
	*/
	bool Tracer::_trace_syscall(pid_t pid)
	{
		long syscall_number = _retrieve_syscall_id(pid);

		// Check system call number is valid
		if(syscall_number >= syscall_names.size())
		{
			TRACE("Syscall number is too great.");
			return true;
		}

		// Get associated handler
		std::map<std::string, ANTIVIR_TRACER_HANDLER>::iterator it = _handler_mapping.find( syscall_names[syscall_number] );
		if(it != _handler_mapping.end())
		{
			struct user_regs_struct regs;

			ptrace((__ptrace_request) PTRACE_GETREGS, pid, NULL, &regs);

			TRACE(syscall_number << " - " << regs.orig_rax);

			return ((*it).second)(pid, regs);
		}

		return true;
	}

	/**
	* Retrieves the syscall id, taking care of architecture type (x86 / x86_64)
	* @param child Child process id
	* @result System call identification number
	*/
	long Tracer::_retrieve_syscall_id(pid_t child)
	{
		long syscall_number;

		#ifdef __i386__
			syscall_number = ptrace(PTRACE_PEEKUSER, child, ORIG_EAX * sizeof(long), NULL);
		#else
			syscall_number = ptrace(PTRACE_PEEKUSER, child, ORIG_RAX * sizeof(long), NULL);
		#endif

		return syscall_number;
	}
}
