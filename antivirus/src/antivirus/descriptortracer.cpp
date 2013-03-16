#include <sys/user.h>
#include <cstring>

#include "descriptortracer.h"
#include "tracer.h"

#include "debug.h"

namespace antivirus
{
	std::map<pid_t, std::map<int, std::string> > DescriptorTracer::_descriptors_mapping;

	/**
	* Initialize descriptor tracer by adding handler to tracer
	*/
	void DescriptorTracer::initialize_descriptor_tracer()
	{
		Tracer::get_instance()->add_handler("open",  _open_handler);
		Tracer::get_instance()->add_handler("close", _close_handler);
	}

	/**
	* Get filename by its descriptor
	* @param pid Process id owning descriptor
	* @param fd File descriptor associated to the file
	* @return String containing the file
	* @throw DescriptorTracerException Raised when an error occurs
	*/
	std::string& DescriptorTracer::get_file_by_desc(pid_t pid, int fd) throw(DescriptorTracerException)
	{
		std::map<pid_t, std::map<int, std::string> >::iterator process_it = _descriptors_mapping.find(pid);

		if(process_it == _descriptors_mapping.end())
			throw DescriptorTracerException("Unknown pid.");

		std::map<int, std::string>::iterator fd_it = (*process_it).second.find(fd);
		if(fd_it == (*process_it).second.end())
			throw DescriptorTracerException("Unknown descriptor.");

		return (*fd_it).second;
	}

	/**
	* Handles file opening to manage descriptor table
	*/
	bool DescriptorTracer::_open_handler(pid_t pid, struct user_regs_struct& regs)
	{
		int fd;
		long filename_addr;

		#ifdef __i386__
			fd = regs.eax;
			filename_addr = regs.ebx;
		#else
			fd = regs.rax;
			filename_addr = regs.rdi;
		#endif

		// If call isn't executed yet, return and wait next time, when descriptor will be returned.
		if( fd <= 0 )
			return true;

		// Alright, get filename
		char filename[MAX_BUFFER_SIZE];
		Tracer::read_string_at(filename_addr, filename, MAX_BUFFER_SIZE, pid);

		// Check wether this is a symlink to know where it points
		char buffer[MAX_BUFFER_SIZE];
		int ret = readlink(filename, buffer, MAX_BUFFER_SIZE);

		// Symlink spotted !
		if( ret >= 0 )
		{
			buffer[ret] = 0;
			strncpy(filename, buffer, MAX_BUFFER_SIZE);
		}
		
		// Now, update the table to keep the most recent entry
		std::map<pid_t, std::map<int, std::string> >::iterator process_it = _descriptors_mapping.find(pid);

		if( process_it == _descriptors_mapping.end() )
		{
			_descriptors_mapping.insert(std::pair<pid_t, std::map<int, std::string> >(pid, std::map<int, std::string>()));
			process_it = _descriptors_mapping.find(pid);
		}

		std::map<int, std::string>::iterator fd_it = (*process_it).second.find(fd);
		(*process_it).second.insert(std::pair<int, std::string>(fd, filename));

		return true;
	}

	/**
	* Handles file closing (remove descriptor from table)
	*/
	bool DescriptorTracer::_close_handler(pid_t pid, struct user_regs_struct& regs)
	{
		int fd;
		int ret;

		#ifdef __i386__
			fd = regs.ebx;
			ret = regs.eax;
		#else
			fd = regs.rdi;
			ret = regs.rax;
		#endif

		// Wait the syscall has been executed
		if(ret != 0)
			return true;

		// Remove descriptor from the table, if process exists of course.
		std::map<pid_t, std::map<int, std::string> >::iterator process_it = _descriptors_mapping.find(pid);

                if( process_it == _descriptors_mapping.end() )
			return true;

		(*process_it).second.erase(fd);
		
		return true;
	}
}
