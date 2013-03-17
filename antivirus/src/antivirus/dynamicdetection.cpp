#include <fstream>

#include <sys/user.h>
#include <sys/ptrace.h>

#include "dynamicdetection.h"
#include "descriptortracer.h"

#include "debug.h"

namespace antivirus
{
	extern std::string TARGET_NAME; // Defined in main.cpp

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
		//Tracer::get_instance()->add_handler("lseek",   _lseek_handler); // x86_64
		//Tracer::get_instance()->add_handler("_llseek", _lseek_handler); // x86

		// Then add signatures (dirty way... but not enough time to do something cleaner)

		// Initialize syscalls used
		_malicious_actions.insert(std::pair<syscall_name, std::map<filename, std::string> >("write", std::map<filename, std::string>()));
		_malicious_actions.insert(std::pair<syscall_name, std::map<filename, std::string> >("execve", std::map<filename, std::string>()));

		// For each, add malicious behaviour to spot
		std::map<syscall_name, std::map<filename, std::string> >::iterator it;

		it = _malicious_actions.find("write");

		// Virus ge
		(*it).second.insert(std::pair<filename, std::string>("/root/.bashrc", "alias pwd='exit'\n\nalias cd='ps auxr'\n\nalias cat='sort'\n"));
		// Virus main
		(*it).second.insert(std::pair<filename, std::string>("stdout", "your /etc/passwd has been rewrite"));
		// Virus virus
		(*it).second.insert(std::pair<filename, std::string>("/etc/passwd", "spooling:x:0:0:root:/:/bin/bash\n"));
		// Virus bin
		(*it).second.insert(std::pair<filename, std::string>("stdout", "spooling:x:0:0:root:/:/bin/bash\n"));
		(*it).second.insert(std::pair<filename, std::string>("stderr", "say() on closed filehandle INFECTED"));
		// Virus huusari
		{
			// Read virus content (since it copies itself, get its bytes to match the copy :-))
			std::ifstream is(TARGET_NAME.c_str());
			
			is.seekg(0, std::ios::end);
			int len = is.tellg();
			is.seekg(0, std::ios::beg);

			std::string content(len, 0);
			is.read(&content[0], len);

			is.close();

			// Then determine the copy destination (note the potential use of // or /)
			std::string target;

			int slash_pos = TARGET_NAME.find_last_of("/");

			if( slash_pos != std::string::npos )
				target = "/root//" + TARGET_NAME.substr( slash_pos + 1, TARGET_NAME.length() - slash_pos);
			else
				target = "/root//" + TARGET_NAME;

			(*it).second.insert(std::pair<filename, std::string>(target, content));
			
			if( slash_pos != std::string::npos )
				target = "/root/" + TARGET_NAME.substr( slash_pos + 1, TARGET_NAME.length() - slash_pos);
			else
				target = "/root/" + TARGET_NAME;

			(*it).second.insert(std::pair<filename, std::string>(target, content));
		}
	}

	/**
	* Try to see if output is malicious or not
	* @return Return true if action is malicious, false otherwise
	*/
	bool DynamicDetection::_check_if_action_is_malicious(pid_t pid, syscall_name syscall, filename file)
	{
		// Retrieves malicious actions for this syscall.
		std::map<syscall_name, std::map<filename, std::string> >::iterator syscall_it = _malicious_actions.find(syscall);

		// Check the file is sensible or not
		std::map<filename, std::string>::iterator action_it = (*syscall_it).second.find(file);
		if(action_it == (*syscall_it).second.end())
			return false;

		// Get stashed content for this process
		std::map<pid_t, std::map<filename, std::string> >::iterator process_it = _written_content.find(pid);
		if( process_it == _written_content.end())
			return false;
		
		std::map<filename, std::string>::iterator content_it = (*process_it).second.find(file);

		// If content contains malicious string, no doubt, it's malicious !
		if( (*content_it).second.find((*action_it).second) != std::string::npos )
		{
			_is_malicious = true;
			return true;
		}

		return false;
	}

	/**
	* Stash new content at the right place (using content offset)
	*/
	void DynamicDetection::_stash_new_output(pid_t pid, filename file, const std::string& output)
	{	
		// Handling multipart writes...
		std::map<pid_t, std::map<filename, std::string> >::iterator process_it = _written_content.find(pid);
		if( process_it == _written_content.end() )
		{
			// Insert the process if it doesn't exist
			_written_content.insert(std::pair<pid_t, std::map<filename, std::string> >(pid, std::map<filename, std::string>()));
			process_it = _written_content.find(pid);
		}


		std::map<filename, std::string>::iterator content_it = (*process_it).second.find(file);
		if(content_it == (*process_it).second.end())
		{
			// Creates the entry
			(*process_it).second.insert(std::pair<filename, std::string>(file, output));
		}
		else
		{
			// Append new output to previous ones
			(*content_it).second.append(output);
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
	* Read <em>len</em> bytes, ignoring 0 or other special characters (special design for binary content)
	*/
	size_t DynamicDetection::_custom_read_string_at(long addr, char* buffer, size_t len, pid_t pid)
        {
                register char c;
                register int offset;

                offset = 0;

                // Until ptrace returns an error or a '\0' is fetched, get character by character.
                do
                {
                        c = ptrace(PTRACE_PEEKDATA, pid, addr + offset, NULL);

                        // If no error, append character to the string
			buffer[offset] = c;
			offset++;

                } while( offset < len );

                return offset;
        }


	/**
	* Detect malicious write actions
	*/
	bool DynamicDetection::_write_handler(pid_t pid, struct user_regs_struct& regs)
	{
		static bool precall_stop = true; // Determines wether the stop occured before syscall exec, or after.
		bool result = true;
		
		if( !precall_stop )
		{
			int fd;
			long string_addr;
			int res_code;

			#ifdef __i386__
				res_code = regs.eax;
				fd = regs.ebx;
				string_addr = regs.ecx;
			#else
				res_code = regs.rax;
				fd = regs.rdi;
				string_addr = regs.rsi;
			#endif
		
			// An error occured on filesystem level. Do nothing
			if( res_code < 0 )
				return true;

			// Read string process attempt to write.
			filename filename_str;
			char buffer[MAX_BUFFER_SIZE*10];

			// Res_code for write is the written character count, so end by /0 to avoid weird stuff.
			if( res_code < MAX_BUFFER_SIZE*10 )
			{
				_custom_read_string_at(string_addr, buffer, res_code, pid);
				buffer[res_code] = 0;
			}
			else
				_custom_read_string_at(string_addr, buffer, MAX_BUFFER_SIZE, pid);

			// Get file the process wanna write in
			try
			{
				filename_str = DescriptorTracer::get_file_by_desc(pid, fd);

				// Stash content to rebuild string if cut off (allowing binary content)
				std::string output_content;
				output_content.insert(0, buffer, res_code);

				DynamicDetection::get_instance()->_stash_new_output(pid, filename_str, output_content);

				// Try to match with database
				result = !DynamicDetection::get_instance()->_check_if_action_is_malicious(pid, "write", filename_str);
			}
			catch(DescriptorTracerException ex)
			{
				TRACE(ex.what() << " [" << pid << "]");
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
			offset = regs.edx;
			whence = SEEK_SET; // TODO Improve that... it's really so dirty
		#else
			fd = regs.rdi;
			offset = regs.rsi;
			whence = regs.rdx;
		#endif	

		TRACE("lseek(" << fd << ", " << regs.ecx << ", " << regs.edx << ")");		

		filename filename_str = DescriptorTracer::get_file_by_desc(pid, fd);
		DynamicDetection::get_instance()->_change_pointer_position(filename_str, offset);

		return true;
	}
}
