/*
 * fileCounter.h
 *
 *  Created on: 9 mars 2013
 *      Author: remi
 */

#define _GNU_SOURCE

#include <dlfcn.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef FILECOUNTER_H_
#define FILECOUNTER_H_

	/* The number of files to open */
	extern int nbFiles;

	/* The name of the file containing the database */
	#define DATABASE_FILE "database"

	/* When a virus has been found */
	#define VIRUS_DETECTED 1

	/* When the file is virus-free */
	#define VIRUS_FREE 0

	/* TODO - comment */
	typedef struct
	{
		/* Name of the opened file */
		char* _file;

		/* The sentence which is meant to be writen in if
		 * the tested app is a virus */
		char* _sentence;

		/* The address it has been laoded in */
		long _address;

	} fileLocator;

	/* one file allocator for each file */
	extern fileLocator* locators;

	/* defines the addsress given in parameter as the address that the
	 * given in parameter */
	void loadFile(const char* file, long address);

	/* removes the address from the locator */
	void closeFile(long address);

	/* returns VIRUS_DETECTED if the [sentence] parameter is the one
	 * representing the nasty action of the virus, writing in the file
	 * opened at the address [address]. returns VIRUS_FREE otherwise */
	int writeVirusSentence(long address, const char* sentence);

#endif /* FILECOUNTER_H_ */
