#include <signal.h>

#include "core.h"
#include "filereader.h"
#include "zipreader.h"
#include "tracer.h"
#include "sandbox.h"
#include "commandrunner.h"

#include "debug.h"

namespace antivirus
{
	/**
	* Constructor
	* @throw DatabaseException Thrown if database cannot be read
	*/
	Core::Core() throw(DatabaseException)
	{
		_database.readAppendedData();
	}

	/**
	* Constructor allowing user to specify database_file
	* @param database_file	File to be used as database
	* @throw DatabaseException Thrown if database cannot be read
	*/
	Core::Core(const std::string& database_file) throw(DatabaseException)
	{
		_database.read(database_file);
	}

	/**
	* Destructor
	*/
	Core::~Core()
	{
	}

	/**
	* Perform a static check, verifying signatures
	* @param filename Filename of the file to check
	* @return E_FAILED if something went wrong, E_VIRUS_SPOTTED if this is a virus, or E_LOOKS_CLEAN if it's a regular file
	*/
	Core::E_RETURN_CODE Core::perform_static_check(const std::string& filename)
	{
		FileData* file_data = NULL;
		bool matches = false;

		TRACE("Static check.");

		// Is the file a zip?
		try
		{
			ZipReader zip_reader(filename);
			TRACE("Zip file spotted - Checking all files");

			// Check signatures for all contained files
			ZipReader::iterator it;
			for(it = zip_reader.begin(); it != zip_reader.end() && !matches; it++)
			{
				try
				{
					// NOT SUPPORTING ZIP INTO A ZIP...
					file_data = zip_reader.read_zipped_file((*it));
					matches |= _check_signatures(file_data);

					// Release memory
					delete file_data;

					if( matches )
						TRACE((*it));
				}
				catch(ZipReaderException ex)
				{
					// File cannot be read... continue
					TRACE("ZipReaderException occured.");
					continue;
				}
			}			

			// If a signature matched, stop here and return, else, check the whole file...
			if(matches)
				return E_VIRUS_SPOTTED;
		}
		catch(ZipReaderException ex)
		{
			// File is not a zip... don't care and continue
			TRACE("Not a zip file.");
		}	


		// Open file as regular and read its content
		FileReader reader(filename);

		try {
			reader.read();
			TRACE("Read regular file");

			// Check if signatures matches
			file_data = reader.getFileData();
			matches = _check_signatures(file_data);
		}
		catch(FileReaderException ex)
		{
			cerr << ex.what() << endl;
			return E_FAILED;
		}

		return matches ? E_VIRUS_SPOTTED : E_LOOKS_CLEAN;
	}

	/**
	* Perform a dynamic check, verifying behaviour at runtime
	* @param filename Filename of the file to check
	* @return E_FAILED if something went wrong, E_VIRUS_SPOTTED if this is a virus, or E_LOOKS_CLEAN if it's a regular file
	*/
	Core::E_RETURN_CODE Core::perform_dynamic_check(const std::string& filename)
	{
		_dynamic_check_by_tracing(filename);

		return E_FAILED;
	}

	/**
	* Check if the file matches with the various signatures contained in database.
	* @param data	Data of the file to check
	* @return True if one matches, false otherwise
	*/
	bool Core::_check_signatures(FileData* data)
	{
		std::multimap<string, string> database;
		std::multimap<string, string>::const_iterator it;

		database = _database.getSignatures();

		bool matched = false;
		for( it = database.begin(); it != database.end() && !matched; it++ )
		{
			matched = data->check_signature((*it).second);

			if(matched) 
				TRACE((*it).first.c_str());
		}
	
		return matched;
	}

	/**
	* Performs a dynamic check using hijacking method.
	* @param filename File to be checked
	* @return E_FAILED if something went wrong, E_VIRUS_SPOTTED if this is a virus, or E_LOOKS_CLEAN if it's a regular file
	*/
	Core::E_RETURN_CODE Core::_dynamic_check_by_hijacking(const std::string& filename)
	{
		

		return E_FAILED;
	}

	/**
	* Performs a dynamic check using system calls tracing method.
	* @param filename File to be checked
	* @return E_FAILED if something went wrong, E_VIRUS_SPOTTED if this is a virus, or E_LOOKS_CLEAN if it's a regular file
	*/
	Core::E_RETURN_CODE Core::_dynamic_check_by_tracing(const std::string& filename)
	{
		// Sandbox the process
		SandBox sandbox(ANTIVIR_SANDBOX_DIR);
	
		sandbox.prepare(filename);

		// Fork processes, one that runs the sandboxed process, other that traces execution
		pid_t pid = fork();
		if( pid == 0 )
		{
			// Determine file type, and interpreter if needed
			CommandRunner runner(filename);
			CommandRunner::COMMAND_INFO_STRUCT command = runner.resolve();

			// Prepare sandbox & tracer
			sandbox.run();
			Tracer::get_instance()->trace_me();

			// Synchronize with parent :-)
			kill(getpid(), SIGSTOP);

			// Run the process
			if( command.type == CommandRunner::TYPE_ELF )
			{
				char* const param[] = { (char*) command.absolute_filename.c_str(), NULL };
				execve(param[0], param, environ);
			}
			else
			{
				char* const param[] = { (char*) command.absolute_interpreter.c_str(), (char*) command.absolute_filename.c_str(), NULL };
				execve(param[0], param, environ);
			}

			throw exception();
		}

		Tracer::get_instance()->trace_it(pid);

		return E_FAILED;
	}
}
