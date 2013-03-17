#include <boost/filesystem.hpp>
#include <cstdlib>
#include <sstream>
#include <fstream>

#include <unistd.h>
#include <sys/stat.h>

#include "sandbox.h"
#include "tracer.h"
#include "debug.h"

using namespace boost;

namespace antivirus
{
	SandBox* SandBox::_current_instance = NULL;

	/**
	* Constructor
	* @param path Path of the sandboxed environnement
	* @throw SandBoxException Thrown if an error occurs.
	*/
	SandBox::SandBox(const std::string& path) throw(SandBoxException) : _path(path)
	{
		_initialize();

		SandBox::_current_instance = this;
	}

	/**
	* Destructor
	*/
	SandBox::~SandBox()
	{
		try
		{
			_cleanup();
		}
		catch(SandBoxException ex)
		{
			TRACE("SandBoxExceptions raised.");
		}
	}

	/**
	* Prepare sandbox to run the <em>target</em>.
	* @param target The target that is gonna be run
	* @throw SandBoxException Thrown if an error occurs.
	*/
	void SandBox::prepare(const std::string& target) throw(SandBoxException)
	{
		if(target.size() <= 0)
			throw SandBoxException();

		// Keep target
		_target = target;

		// Then resolve dependencies
		std::vector<std::string> deps;

		_find_deps(&deps);

		// Setup the dependencies
		_setup_deps(&deps);
	}

	/**
	* Run the sandbox. IT IS NOT CANCELABLE !
	* @throw SandBoxException Thrown if an error occurs.
	*/
	void SandBox::run() throw(SandBoxException)
	{
		// Copy target and add execute flag :-)
		_copy_file_and_create_subdirs(_target, true);

		// Sandboxing
		if( chroot( _path.c_str() ) != 0 )
		{
			TRACE("chroot failed.");
			throw SandBoxException();
		}

		// Change current directory
		if( chdir( _path.c_str() ) != 0 )
		{
			TRACE("chdir failed.");
		}
	}

	/**
	* Allow user to tell which dependencies must be present in sandbox
	* @param deps	Dependance names
	* @throw SandBoxException Thrown if an error occurs.
	*/
	void SandBox::set_additionnal_dependencies(std::vector<std::string> deps) throw(SandBoxException)
	{
		_setup_deps(&deps); // Easy!
	}

	/**
	* Allow user to tell which fake files must be present in sandbox
	* @param files	File names
	* @throw SandBoxException Thrown if an error occurs.
	*/
	void SandBox::set_additionnal_files(std::vector<std::string> files) throw(SandBoxException)
	{
		// Well, could be weird at the first sight but I promise, it works fine!
		_setup_deps(&files);
	}

	/**
	* Initialize tracer to cooperate with the sandbox
	*/
	void SandBox::initialize_tracer() throw(SandBoxException)
	{
		Tracer* the_tracer = Tracer::get_instance();

		the_tracer->add_handler("execve", _execve_handler);
		the_tracer->add_handler("access",  _stat_access_and_open_handler);
		the_tracer->add_handler("open",    _stat_access_and_open_handler);
		the_tracer->add_handler("stat",    _stat_access_and_open_handler);
		the_tracer->add_handler("stat64",  _stat_access_and_open_handler);
	}

	/**
	* Initialize SandBox duty
	*/
	void SandBox::_initialize() throw(SandBoxException)
	{
		// Check wether the path already exists
		if( filesystem::exists(_path) )
		{
			TRACE("Path already exists.");
			throw SandBoxException("Path already exists.");
		}

		// Then create directory
		filesystem::create_directory(_path);
	}

	/**
	* Cleanup Sandbox
	*/
	void SandBox::_cleanup() throw(SandBoxException)
	{
		// Remove directory and all its content
		filesystem::remove_all(_path);
	}

	/**
	* Find target dependencies
	* @param ptr_deps Dependencies filled list
	*/
	void SandBox::_find_deps(std::vector<std::string>* ptr_deps) const throw(SandBoxException)
	{
		// The only way to proceed I found is to use ldd in an empty bash env.
		// If ldd is directly called by the current process, since libc & co are already loaded
		// they won't appear in the dependency list.

		// Build command (ldd <target> > tmp)
		std::string command;
		command += "ldd ";
		command += _target;
		command += " > ";
		command +=__ANTIVIR_LDD_RES_TMP_FILE;

		// Execute command
		int ret = ::system(command.c_str());
		if( ret < 0 )
		{
			TRACE("System err.");
			throw SandBoxException("System returned an error");
		}

		// Read file content
		std::ifstream is(__ANTIVIR_LDD_RES_TMP_FILE);
		if(!is)
		{
			TRACE("Can't open file.");
			throw SandBoxException();
		}

		is.seekg(0, std::ios::end);
		long length = is.tellg();
		is.seekg(0, std::ios::beg);

		char* buffer = new char[length+1];
		is.read(buffer, length);
		buffer[length] = 0;

		is.close();

		// Parse result
		_parse_ldd_result(buffer, ptr_deps);

		// Clean temp file and release memory
		filesystem::remove(__ANTIVIR_LDD_RES_TMP_FILE);

		delete [] buffer;
	}

	/**
	* Parse ldd result to fetch library paths
	*/
	void SandBox::_parse_ldd_result(const std::string& raw_file, std::vector<std::string>* ptr_deps) const
	{
	        std::string current_line, path;
		size_t next_lf = 0;
		size_t last_lf = 0;
		size_t pos_sl = 0;
		bool must_continue = true;

		while( must_continue )
		{
			// Get next \n to split lines, if not found, it means we're at the end so,
			// tell to stop next time and set pos to last char.
			next_lf = raw_file.find( "\n", last_lf );
			if( next_lf == std::string::npos )
			{
				next_lf = raw_file.length();
				must_continue = false;
			}

			// Retrieves line
			current_line = raw_file.substr( last_lf, next_lf - last_lf);

			// Then, look at the first '/', it would be the path where to find the library.
			// If there are no, check the next line
			pos_sl = current_line.find( "/" );
			if( pos_sl != std::string::npos )
			{
				// Substring the path, from '/' until the next space (no space in path)
				path = current_line.substr( pos_sl, current_line.find( " ", pos_sl ) - pos_sl );

				// Check the lib exists, and if it does, add it to the list
				if( filesystem::exists(path) == true )
					ptr_deps->push_back(path);
				else
					TRACE("Path not found: " << path);
			}

			last_lf = next_lf+1;
		}
	}

	/**
	* Setup dependencies into the sandbox directory
	*/
	void SandBox::_setup_deps(std::vector<std::string>* ptr_deps) const throw(SandBoxException)
	{
		std::vector<std::string>::iterator it;
		std::string dest;

		/**
		* For each dependencies:
		*	1- Get its real path
		*	2- Create subdirectories in sandbox
		*	3- Copy file
		*/
		for(it = ptr_deps->begin(); it != ptr_deps->end(); it++)
		{
			_copy_file_and_create_subdirs((*it));
		}
	}

	/**
	* Copy a file, creating subdirs
	* @param src Source file
	*/
	void SandBox::_copy_file_and_create_subdirs(const std::string& src, bool _add_execute_flag) const
	{
		std::string dest;

		// Get real path
		filesystem::path lib_path(src);

		// Build dest
		dest  = _path + "/" ;
		dest += lib_path.branch_path().string(); // Example: /foo/bar gives /foo

		// Create directories
		filesystem::create_directories(dest.c_str());

		dest = _path + "/" + src;

		// Check the file isn't in /dev, it's still annoying to copy a device such hard drive...
		if(src.find("/dev") == 0)
		{
			// If it's the case, then just put an empty file
			std::ofstream os(dest.c_str());
			os.close();

			TRACE("Touched " << dest);
		}
		else
		{
			// Finally copy file
			if(filesystem::exists(dest) == false && filesystem::is_directory(src) == false)
			{
				TRACE("Copying " << dest);
				filesystem::copy_file(src, dest);

				if(_add_execute_flag)
					chmod(dest.c_str(), 0777);
			}
		}
	}

	/**
	* Determines the full path of the passed command <em>cmd</em>.
	* @param cmd Command to look for
	* @return Full path
	* @throw SandBoxException If the command cannot be stated.
	*/
	std::string SandBox::get_command_location(const std::string& cmd) throw(SandBoxException)
	{
		// Test the string is valid
		if(cmd.size() <= 0)
			throw SandBoxException("Empty command.");

		// Begin to check the current path
		if( filesystem::exists(cmd) )
			return cmd;

		if( filesystem::exists("./" + cmd) )
			return std::string("./" + cmd);

		// Then get the PATH variable and split it
		std::vector<std::string> path_to_check;
		std::string path_env = getenv("PATH");

		if( path_env.size() >= 0 )
		{
			std::string buf;
			int current_pos = 0;
			int last_pos = 0;
			bool must_continue = true;		

			do
			{
				current_pos = path_env.find(":", last_pos);
				if(current_pos == std::string::npos)
				{
					// The path starts from last_pos until the end
					buf = path_env.substr(last_pos, path_env.size() - last_pos);
					must_continue = false;
				}
				else
				{
					buf = path_env.substr(last_pos, current_pos - last_pos);

					// Keep last_pos and add 1 to skip the ':'
					last_pos = current_pos + 1;
				}

				path_to_check.push_back(buf + "/");

			} while(must_continue);
		}

		// Begin to check!
		std::vector<std::string>::iterator it;

		for( it = path_to_check.begin(); it != path_to_check.end(); it++)
		{
			if(filesystem::exists((*it) + cmd))
				return (*it + cmd);
		}

		// File not found !
		throw SandBoxException("File cannot be found.");
	}

	/**
	* Handler of the system call execve that allows to sandbox subprocess too.
	*/
	bool SandBox::_execve_handler(pid_t pid, struct user_regs_struct& regs)
	{
		long cmd_addr;
		long params_addr;

		// Get values depending on arch.
		#ifdef __i386__
			cmd_addr = regs.ebx;
			params_addr = regs.ecx;
		#else
			cmd_addr = regs.rdi;
			params_addr = regs.rsi;
		#endif

		/* 
		 * Algorithm is simple:
		 * 1.	Get command and check wether it's a command by looking at the PATH specified directories.
		 * 2.	Check wether the command isn't already sandboxed, if not do it.
		 * 3.	Get parameters list and split them if they contain spaces.
		 * 4.	Check wehther they are files or not
		 * 5.   Same as 2.
		 */

		// Step 1.
		char buffer[MAX_BUFFER_SIZE];
		Tracer::read_string_at(cmd_addr, buffer, MAX_BUFFER_SIZE, pid);

		try
		{
			std::string full_path = get_command_location(buffer);	
			TRACE(full_path);
			_current_instance->prepare(full_path);

			// Step 2.
			_current_instance->_copy_file_and_create_subdirs(full_path);
		}
		catch(SandBoxException ex)
		{
			TRACE("Command cannot be sandboxed... " << ex.what());
		}

		// Step 3.
		std::vector<std::string> params = Tracer::read_string_array_at(params_addr, pid);

		if( params.size() > 0 )
		{
			std::vector<std::string>::iterator it;

			std::vector<std::string> strings_to_check;
			int next_pos = 0;
			int last_pos = 0;
			for(it = params.begin(); it != params.end(); it++)
			{
				// Check if there are any space in the current string
				next_pos = (*it).find( " ", last_pos );
				if( next_pos == std::string::npos )
				{
					strings_to_check.push_back(*it);
					continue;	
				}
				else
				{
					// Splitting
					bool must_continue = true;
					while( must_continue )
					{
						strings_to_check.push_back((*it).substr( last_pos, next_pos - last_pos ));

						last_pos = next_pos + 1;

						next_pos = (*it).find( " ", last_pos );
						if( next_pos == std::string::npos )
							must_continue = false;
					}
				}
			}

			// Step 4.
			std::string full_path;
			for(it = strings_to_check.begin(); it != strings_to_check.end(); it++)
			{
				if(filesystem::exists(*it))
					full_path = (*it);
				else
				{
					try
					{
						full_path = get_command_location(*it);
					}
					catch( SandBoxException ex )
					{
						// It's not a file, check the next string
						continue;
					}
				}

				TRACE(full_path);

				// Step 5.
				// A little check that is crucial ! Is it a directory?
				// If yes, just create the directory, else, this is the easy way
				if( filesystem::is_directory(full_path) == true )
				{
					filesystem::create_directories(_current_instance->_path + "/" + full_path);
				}
				else
				{
					try
					{
						_current_instance->prepare(full_path);
						_current_instance->_copy_file_and_create_subdirs(full_path);
					}
					catch( SandBoxException ex )
					{
						TRACE( ex.what() );
					}
				}
			}
		}

		return true;
	}

	/**
	* Handler of the system calls stat, access & open that allows to place unknown files into the sandbox.
	* stat, access & open calls have the string containing filename in 1st position.
	*/
	bool SandBox::_stat_access_and_open_handler(pid_t pid, struct user_regs_struct& regs)
	{
                long file_addr;

                // Get values depending on arch.
                #ifdef __i386__
                        file_addr = regs.ebx;
                #else
                        file_addr = regs.rdi;
                #endif

		char filename[MAX_BUFFER_SIZE];
		Tracer::read_string_at(file_addr, filename, MAX_BUFFER_SIZE, pid);

		// Check wether this file exists
		if(filesystem::exists(filename) == true)
		{
			// And copy it into the filesystem
			_current_instance->_copy_file_and_create_subdirs(filename);
		}

		return true;
	}
}
