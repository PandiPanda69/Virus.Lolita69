#include "fileCounter.h"

#define MAX_STR 512

// ----------------------------------------------------- global variables
int nbFiles = 0;
fileLocator* locators = NULL;

// ------------------------------------------------------------ functions
/*
 * This function opens the file database, containing the names
 * of the files which can be modified by the known viruses and
 * the sentences added into these files by the viruses, and
 * loads these informations into the "locators" variable.
 *
 * Initiates the "locators" variable thanks to a "database" file
 */
void initFileLocators() {

	// ----------------------------------- variables
	// buffer for the line readings
	char line[MAX_STR];

	// will count the nulber of useful lines
	// (name of files / sentences)
	int lineCounter = 0;

	// index representing the file in the locators table
	int indexLocator = 0;


	// ----------------------- loading database file

	FILE* (*myFopen) (const char *, const char *) = NULL;

	// execute the previous function
	myFopen = dlsym(RTLD_NEXT, "fopen");

	// call the original fopen with the same arugments
	FILE* myDatabase = myFopen(DATABASE_FILE, "r");


	// ------------------------------ main algorithm

	// bail out if file not found
	if(!myDatabase)
		return;

	// Gets the first line (number of files)
	if(fgets(line, sizeof(line), myDatabase) != NULL) {
		// updates the global variables
		nbFiles = atoi(line);
		locators = malloc(nbFiles*sizeof(fileLocator));
	}

	// Gets the files details:
	// 		first line = fileName
	//		second line = sentence to insert inside if the tested
	// app is a virus
	while(fgets(line,sizeof(line),myDatabase) != NULL) {

		// strip trailing '\n' if it exists
		int len = strlen(line)-1;
		if(line[len] == '\n')
			line[len] = '\0';

		// update locator's index
			// one locator needs two lines for being set up
		indexLocator = lineCounter / 2;

		// add the line in the appropriate fileLocator
		if (lineCounter % 2 == 0) {
			// pair number = name file
			locators[indexLocator]._file = malloc(MAX_STR*sizeof(char));
			strcpy(locators[indexLocator]._file, line);
		}
		else {
			// odd number = sentence for file
			locators[indexLocator]._sentence = malloc(MAX_STR*sizeof(char));
			strcpy(locators[indexLocator]._sentence, line);
		}
		++lineCounter;
	}
	fclose(myDatabase);
}

/*
 * If [file] is a file known as attacked by a virus,
 * sets [address] to its locator's address.
 */
void loadFile(const char* file, long address) {

	int i;

	// Initialize variables in case it hasn't been done yet
	if (locators == NULL) {
		initFileLocators();
	}

	// looks for the locator which file name is [file]
	for (i = 0; i < nbFiles; i++) {
		// If there is one, affects the address to the locator
		if (strcmp(locators[i]._file, file) == 0) {
			locators[i]._address = address;
			return;
		}
	}
}

/*
 * If [file] is a file known as attacked by a virus,
 * reinitializes its locator's address.
 */
void closeFile(long address) {
	int i;
	// looks for the locator which address is [address]
	for (i = 0; i < nbFiles; i++) {
		// If there is one, affects the address 0 to the locator
		if (locators[i]._address == address) {
			locators[i]._address = 0;
			return;
		}
	}
}

/*
 * Returns VIRUS_DETECTED if the virus' sentence is [sentence] for the
 * file which address is [address]. Returns VIRUS_FREE otherwise.
 */
int writeVirusSentence(long address, const char* sentence) {
	int i;
	// looks for the locator which address is [address]
	for (i = 0; i < nbFiles; i++) {
		// If there is one, affects the address 0 to the locator
		if (locators[i]._address == address
				&& strcmp(locators[i]._sentence, sentence) == 0) {
			return VIRUS_DETECTED;
		}
	}
	return VIRUS_FREE;
}

// ----------------------------------------------- only for tests
// to be deleted
void testCounter() {

	int i;
	//initFileLocators();

	for (i = 0; i<3; i++)
		printf("%s # %s # %d\n", locators[i]._file, locators[i]._sentence, (int)locators[i]._address);

	return;
}
