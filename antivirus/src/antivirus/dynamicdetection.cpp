#include <sys/user.h>

#include "dynamicdetection.h"
#include "descriptortracer.h"

#include "debug.h"

namespace antivirus
{
	/**
	* Constructor
	*/
	DynamicDetection::DynamicDetection() : _is_malicious(false)
	{
	}

	/**
	* Destructor
	*/
	DynamicDetection::~DynamicDetection()
	{
	}

	/**
	* Returns the only instance of the class
	*/
	DynamicDetection* DynamicDetection::get_instance()
	{
		static DynamicDetection the_instance;
		return &the_instance;
	}

	/**
	* Initialize handlers and signatures /!\ DIRTY /!\
	*/
	void DynamicDetection::initialize()
	{
		// Add handlers
		Tracer::get_instance()->add_handler("write",   _write_handler);
		Tracer::get_instance()->add_handler("lseek",   _lseek_handler); // x86_64
		Tracer::get_instance()->add_handler("_llseek", _lseek_handler); // x86

		// Then add signatures (dirty way... but not enough time to do something cleaner)

		// Initialize syscalls used
		_malicious_actions.insert(std::pair<syscall_name, std::map<filename, std::string> >("write", std::map<filename, std::string>()));

		// For each, add malicious behaviour to spot
		std::map<syscall_name, std::map<filename, std::string> >::iterator it;

		// TODO - Insert signatures here
		it = _malicious_actions.find("write");
		(*it).second.insert(std::pair<filename, std::string>("../../../raspberry-portal/logs/application.log", "HELLO"));
	}

	/**
	* Try to see if output is malicious or not
	* @return Return true if action is malicious, false otherwise
	*/
	bool DynamicDetection::_check_if_action_is_malicious(syscall_name syscall, filename file)
	{
		// Retrieves malicious actions for this syscall.
		std::map<syscall_name, std::map<filename, std::string> >::iterator syscall_it = _malicious_actions.find(syscall);

		// Check the file is sensible or not
		std::map<filename, std::string>::iterator action_it = (*syscall_it).second.find(file);
		if(action_it == (*syscall_it).second.end())
			return false;

		// Get stashed content
		std::map<filename, std::string>::iterator content_it = _written_content.find(file);

		// If content is equal, no doubt, it's malicious !
		if( (*content_it).second.find((*action_it).second) != std::string::npos )
		{
			TRACE("[" << (*content_it).second << "] is malicious." );
			_is_malicious = true;
			return true;
		}

		return false;
	}

	/**
	* Stash new content at the right place (using content offset)
	*/
	void DynamicDetection::_stash_new_output(filename file, std::string output)
	{	
		// Handling multipart writes...
		std::map<filename, std::string>::iterator content_it = _written_content.find(file);
		if(content_it == _written_content.end())
		{
			TRACE("NO : " << output );

			// Creates the entry
			_written_content.insert(std::pair<filename, std::string>(file, output));
		}
		else
		{
			// Append new output to previous ones
			(*content_it).second.append(output);

			TRACE("Final : " << (*content_it).second );
		}
	}

	/**
	* Change file pointer offset
	*/
	void DynamicDetection::_change_pointer_position(filename file, int new_offset)
	{
		std::map<filename, int>::iterator it = _content_offset.find(file);
		if( it != _content_offset.end() )
		{
			// Remove and re-add entry (modification)
			_content_offset.erase(it);
		}

		// Insertion
		_content_offset.insert(std::pair<filename, int>(file, new_offset));
	}

	/**
	* Detect malicious write actions
	*/
	bool DynamicDetection::_write_handler(pid_t pid, struct user_regs_struct& regs)
	{
		static bool precall_stop = true; // Determines wether the stop occured before syscall exec, or after.
		bool result = true;
		
		if( precall_stop )
		{
			int fd;
			long string_addr;

			#ifdef __i386__
				fd = regs.ebx;
				string_addr = regs.ecx;
			#else
				fd = regs.rdi;
				string_addr = regs.rsi;
			#endif
		
			// Read string process attempt to write.
			filename filename_str;
			char buffer[MAX_BUFFER_SIZE];

			Tracer::read_string_at(string_addr, buffer, MAX_BUFFER_SIZE, pid);

			// Get file the process wanna write in
			try
			{
				filename_str = DescriptorTracer::get_file_by_desc(pid, fd);

				// Try to match with database
				DynamicDetection::get_instance()->_stash_new_output(filename_str, buffer);
				result = !DynamicDetection::get_instance()->_check_if_action_is_malicious("write", filename_str);
			}
			catch(DescriptorTracerException ex)
			{
				TRACE(ex.what());
			}
		}

		precall_stop = !precall_stop;
		return result;
	}

	/**
	* Hooks file position changes
	*/
	bool DynamicDetection::_lseek_handler(pid_t pid, struct user_regs_struct& regs)
	{
		int fd;
		int offset;
		int whence;

		#ifdef __i386__
			fd = regs.ebx;
			offset = regs.ecx;
			whence = SEEK_SET; // TODO Improve that... it's really so dirty
		#else
			fd = regs.rdi;
			offset = regs.rsi;
			whence = regs.rdx;
		#endif	

		TRACE("lseek(" << fd << ", " << offset << ", " << whence << ")");		

		filename filename_str = DescriptorTracer::get_file_by_desc(pid, fd);
		DynamicDetection::get_instance()->_change_pointer_position(filename_str, offset);

		return true;
	}
}
