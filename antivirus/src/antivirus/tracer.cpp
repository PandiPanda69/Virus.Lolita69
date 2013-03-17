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
	* Returns the only instance of the class
	*/
	Tracer* Tracer::get_instance()
	{
		static Tracer inst;

		return &inst;
	}

	/**
	* Constructor
	*/
	Tracer::Tracer()
	{
		fill_syscall();

		// Default behaviour
		add_handler("clone", _clone_handler);
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
	}

	/**
	* Run the step-by-step analysis.
	* This method isn't asynchronous, it returns when observed process ends.
	* @param pid Process ID of the process to trace
	*/
	void Tracer::trace_it(pid_t pid) throw(TracerException)
	{
		pid_t child;
		bool must_continue;
		int status;

		// Keep in mind which process we trace
		_traced_process.insert(pid);

		must_continue = true;
		while(must_continue)
		{
			// Waits until a child sends a signal
			child = waitpid(-1, &status, __WALL);

			// Check if the signal tells us children exited or whatever
			if(WIFEXITED(status) || WIFSIGNALED(status) || !WIFSTOPPED(status) )
			{
				// Remove current child from traced process and, if there are no left, exit.
				_traced_process.erase(child);

				if(_traced_process.size() == 0 )
				{
					TRACE("No children anymore.");
					break;
				}
				else
					TRACE("A child exited (" << _traced_process.size() << " left).");

				continue;
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

		// Get associated handlers
		std::pair<std::multimap<std::string, ANTIVIR_TRACER_HANDLER>::iterator, std::multimap<std::string, ANTIVIR_TRACER_HANDLER>::iterator> handlers_it;

		handlers_it = _handler_mapping.equal_range( syscall_names[syscall_number] );

		// Then, execute them all
		bool result = true;
		std::multimap<std::string, ANTIVIR_TRACER_HANDLER>::iterator it;
		for(it = handlers_it.first; it != handlers_it.second && result == true; it++)
		{
			struct user_regs_struct regs;

			ptrace((__ptrace_request) PTRACE_GETREGS, pid, NULL, &regs);
			result = ((*it).second)(pid, regs);
		}

		return result;
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

	/**
	* Traces offsprings
	* @param pid The process ID of the new process to trace
	*/
	void Tracer::_trace_subprocess(pid_t pid)
	{
		int ret = ptrace(PTRACE_ATTACH, pid, NULL, NULL);

		if( ret != 0 )
		{
			TRACE("ERROR - Cannot attach to process #" << pid);
		}
		else
		{
			_traced_process.insert(pid);	
			TRACE(_traced_process.size() << " process are currently traced. (" << pid << ")");
		}
	}

	/**
	* Prints formatted register values out (in debug mode only)
	* @param out Stream to print in
	* @param regs Values to be printed out
	*/
	void Tracer::output_regs(std::ostream& out, const struct user_regs_struct& regs)
	{
	#ifdef DEBUG
		out << "Registers values:" << std::endl << "-------------------------------" << std::endl;

		#define OUTPUT_REG(__name, __var)	out << __name << " : 0x" << std::hex << regs.__var << std::endl;

		#ifdef __i386__
			OUTPUT_REG("ORIG_EAX", orig_eax)
			OUTPUT_REG("EAX", eax)
			OUTPUT_REG("EBX", ebx)
			OUTPUT_REG("ECX", ecx)
			OUTPUT_REG("EDX", edx)
			OUTPUT_REG("ESI", esi)
			OUTPUT_REG("EDI", edi)
			OUTPUT_REG("EIP", eip)
			OUTPUT_REG("EBP", ebp)
			OUTPUT_REG("ESP", esp)
		#else
			OUTPUT_REG("ORIG_RAX", orig_rax)
			OUTPUT_REG("RAX", rax)
			OUTPUT_REG("RBX", rbx)
			OUTPUT_REG("RCX", rcx)
			OUTPUT_REG("RDX", rdx)
			OUTPUT_REG("RSI", rsi)
			OUTPUT_REG("RDI", rdi)
			OUTPUT_REG("RIP", rip)
			OUTPUT_REG("RBP", rbp)
			OUTPUT_REG("RSP", rsp)
			OUTPUT_REG("R8", r8)
			OUTPUT_REG("R9", r9)
			OUTPUT_REG("R10", r10)
			OUTPUT_REG("R11", r11)
			OUTPUT_REG("R12", r12)
			OUTPUT_REG("R13", r13)
			OUTPUT_REG("R14", r14)
			OUTPUT_REG("R15", r15)
		#endif

		out << std::dec;
	#endif
	}

	/**
	* Reads a string at the address <em>addr</em> of the process having id <em>pid</em> into the specified <em>buffer</em>.
	* @param addr Address of the string to read
	* @param buffer Buffer to put read content in
	* @param buf_len Max size of the buffer
	* @param pid Process ID of the process to read in
	* @return How many characters have been read
	*/
	size_t Tracer::read_string_at(long addr, char* buffer, size_t buf_len, pid_t pid) throw(TracerException)
	{
		if( buffer == NULL )
			throw TracerException("Buffer is NULL.");

		register char c;
		register int offset;

		offset = 0;

		// Until ptrace returns an error or a '\0' is fetched, get character by character.
		do
		{
			c = ptrace(PTRACE_PEEKDATA, pid, addr + offset, NULL);

			// If no error, append character to the string
			if( c != 0 && c != -1 )
			{
				buffer[offset] = c;
				offset++;

				// Avoid buffer overflow (+1 to take in consideration the '\0')
				if( offset+1 > buf_len )
					break;
			}
		} while( c != 0 && c != -1 );

		buffer[offset] = 0;

		return offset;
	}

	/**
	* Reads an array of string at the address <em>addr</em> of the process having id <em>pid</em>.
	* @param addr Address to read
	* @param pid Process id of the process to read in
	* @return Vector of strings
	*/
	std::vector<std::string> Tracer::read_string_array_at(long addr, pid_t pid) throw(TracerException)
	{
		std::vector<std::string> result;

		char buffer[MAX_BUFFER_SIZE];
		register long next_addr;
		register long offset;

		offset = 0;

		do
		{
			next_addr = ptrace(PTRACE_PEEKDATA, pid, addr + offset, NULL);

			if(next_addr > 0)
			{
				Tracer::read_string_at(next_addr, buffer, MAX_BUFFER_SIZE, pid);

				result.push_back(buffer);

				// Offset calculation is quite easy : add address size (arch makes it different)
				offset += sizeof(long);
			}
		}
		while( next_addr != 0 && next_addr != -1 );

		return result;
	}

	/**
	* Default handler when clone syscall is invoked
	* @return True.
	*/
	bool Tracer::_clone_handler(pid_t pid, struct user_regs_struct& regs)
	{
		// Check wether syscall has been executed and return value set (return values are put in rax/eax).
		pid_t new_pid = -1;

		#ifdef __i386__
			new_pid = regs.eax;
		#else
			new_pid = regs.rax;
		#endif

		if( new_pid <= 0 )
			return true;
		
		Tracer::get_instance()->_trace_subprocess(new_pid);

		return true;
	}
}
