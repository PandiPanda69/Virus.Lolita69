/*
 * @authors: Clement, Remi, Seb, Vincent
 *
 * This file has been developed for the Wargame of the course "SECURITE"
 * at INSA in Lyon.
 *
 * The purpose of this file is to override functions, which will be
 * used while the execution of the viruses. It will trace some very
 * specific actions of the already known viruses.
 *
 * In order to compile this file, use the command :
 *
 * 			gcc -fPIC -shared virusChecker.c -o virusCheckerLib.so -ldl
 *
 */

#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "fileCounter.h"

/*
 * Loads the address of the returned object into a list of "locators"
 * associated to the name of the file which is opened and then
 * executes the original fopen.
 */
FILE * fopen(const char * filename, const char * mode)
{
	// original fopen function
	FILE * (*original_fopen) (const char *, const char *) = NULL;

	// execute the previous function
    original_fopen = dlsym(RTLD_NEXT, "fopen");

    // call the original fopen with the same arugments
    FILE* f = original_fopen(filename, mode);

    // ---------------------------
	// saves the location of the file just open
    loadFile(filename, (long)f);
	// ---------------------------

	// return the result
    return f;
}

size_t fwrite ( const void * ptr,
		size_t size,
		size_t count,
		FILE * stream ) {

	// original fopen function
	size_t (*original_fwrite) ( const void * ptr,
							size_t size,
							size_t count,
							FILE * stream ) = NULL;

	// execute the previous function
	original_fwrite = dlsym(RTLD_NEXT, "fwrite");

	// call the original fopen with the same arugments
	size_t ret = original_fwrite(ptr, size, count, stream);

	// ---------------------------
	// saves the location of the file just open
	if (writeVirusSentence((long)stream, (const char*)ptr)
				== VIRUS_DETECTED) {
		printf("virus\n");
	}
	else
		printf("nonvirus\n");
	// ---------------------------
testCounter();
	// return the result
	return ret;


}
