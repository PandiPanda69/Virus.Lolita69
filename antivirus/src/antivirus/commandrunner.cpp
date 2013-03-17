#include <cstdio>

#include <magic.h>
#include <signal.h>

#include "commandrunner.h"
#include "sandbox.h"

#include "debug.h"

namespace antivirus
{
	/**
	* Constructor
	* @param filename File to execute
	*/
	CommandRunner::CommandRunner(const std::string& filename) : _filename(filename)
	{
		_initialize_file_type();
	}

	/**
	* Destructor
	*/
	CommandRunner::~CommandRunner()
	{
	}

	/**
	* Resolve the way a file must be executed, using the right syntax (calling python if it's a pyc file, and so on)
	* @return Filled command structure with every needed information
	*/
	CommandRunner::COMMAND_INFO_STRUCT CommandRunner::resolve() throw(CommandRunnerException)
	{
		// First, get the full path of the command
		try
		{
			_filename = SandBox::get_command_location(_filename);
		}
		catch( SandBoxException ex )
		{
			throw CommandRunnerException( "Cannot determine " + _filename + " location." );
		}

		TRACE(_filename);

		// Then, determines which type the file is
		FILE_TYPE type = _determine_file_type(_filename);

		// To known the command to use to run the process
		std::string command = _determine_command_line(_filename, type);

		// Return the information needed to launch the process
		COMMAND_INFO_STRUCT result;
		result.absolute_filename = _filename;
		result.absolute_interpreter = command;
		result.type = type;

		return result;
	}

	/**
	* Fill a map to associate file type and a string that could be returned by libmagic
	*/
	void CommandRunner::_initialize_file_type()
	{
		_filetype_mapping.insert(std::pair<FILE_TYPE, std::string>(TYPE_ELF, "ELF"));
		_filetype_mapping.insert(std::pair<FILE_TYPE, std::string>(TYPE_PYTHON, "Python"));
		_filetype_mapping.insert(std::pair<FILE_TYPE, std::string>(TYPE_PERL, "perl"));
		_filetype_mapping.insert(std::pair<FILE_TYPE, std::string>(TYPE_SHELL, "shell"));
	}

	/**
	* Determine the file type to be able to run the correct command
	* @param filename File to determine the type
	* @return File type
	*/
	CommandRunner::FILE_TYPE CommandRunner::_determine_file_type(const std::string& filename) throw(CommandRunnerException)
	{
		std::string magic_raw_type;
		magic_t magic_cookie;

		magic_cookie = magic_open(MAGIC_NONE); 
		if( magic_cookie == NULL )
			throw CommandRunnerException("Cannot initialize magic.");

		if( magic_load(magic_cookie, NULL) != 0 )
		{
			magic_close(magic_cookie);
			throw CommandRunnerException("Cannot load magic.");
		}

		// What does magic tell about this file?
		magic_raw_type = magic_file(magic_cookie, filename.c_str());

		// Close magic
		magic_close(magic_cookie);


		// And now, look at returned string to see what type the file is (/!\ dirty /!\)
		size_t offset;
		std::map<FILE_TYPE, std::string>::iterator it;
		for(it = _filetype_mapping.begin(); it != _filetype_mapping.end(); it++)
		{
			offset = magic_raw_type.find((*it).second);
			if( offset != std::string::npos )
				return (*it).first;
		}

		return TYPE_UNKNOWN;
	}

	/**
	* Determine the command line to run to launch the file /!\ DIRTY /!\
	* @param filename File to run
	* @param type Type of the file
	* @return Command line
	*/
	std::string CommandRunner::_determine_command_line(const std::string& filename, FILE_TYPE type) throw(CommandRunnerException)
	{
		std::string command;

		switch(type)
		{
			case TYPE_ELF:
				break;
			case TYPE_PERL:
				command = "/usr/bin/perl";
				break;
			case TYPE_SHELL:
				command = "/bin/sh";
				break;
			case TYPE_PYTHON:
				command = "/usr/bin/python";
				break;
			default:
				throw CommandRunnerException("Unable to determine the file type.");
		}

		return command;
	}
}
