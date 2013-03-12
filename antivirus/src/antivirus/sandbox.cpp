#include <boost/filesystem.hpp>
#include <cstdlib>
#include <sstream>
#include <fstream>

#include <unistd.h>

#include "sandbox.h"
#include "debug.h"

using namespace boost;

namespace antivirus
{
	/**
	* Constructor
	* @param path Path of the sandboxed environnement
	* @throw SandBoxException Thrown if an error occurs.
	*/
	SandBox::SandBox(const std::string& path) throw(SandBoxException) : _path(path)
	{
		_initialize();
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
		// Copy target 
		_copy_file_and_create_subdirs(_target);

		// Sandboxing
		if( chroot( _path.c_str() ) != 0 )
		{
			TRACE("chroot failed.");
			throw SandBoxException();
		}

		// Change current directory
		chdir( _path.c_str() );
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
	* Initialize SandBox duty
	*/
	void SandBox::_initialize() throw(SandBoxException)
	{
		// Check wether the path already exists
		if( filesystem::exists(_path) )
		{
			TRACE("Path already exists.");
			throw SandBoxException();
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
		if( ret != 0 )
		{
			TRACE("System err.");
			throw SandBoxException();
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

		char* buffer = new char[length];
		is.read(buffer, length);

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
	void SandBox::_copy_file_and_create_subdirs(const std::string& src) const
	{
		std::string dest;

		// Get real path
		filesystem::path lib_path(src);

		// Build dest
		dest  = _path + "/" ;
		dest += lib_path.branch_path().string(); // Example: /foo/bar gives /foo

		// Create directories
		filesystem::create_directories(dest.c_str());

		// Finally copy file
		dest = _path + "/" + src;
		TRACE("Copying " << dest);
		filesystem::copy_file(src, dest);
	}
}
