#include "fileCounter.h"

int main(int argc, char ** argv) {

	//résultats attendus seulement sur printf!

	// ouvre un fichier virus
	FILE * myFile = fopen("test1.txt","w");
	fwrite("sentenceTest1", 1, strlen("sentenceTest1"), myFile);
	fclose(myFile);
	// -> resultat attendu : virus

	// ouvre un fichier virus, un non virus, et ne referme les 2
	//qu'après avoir écri dans chacun
	FILE * myFile2 = fopen("test1.txt","w");
	fwrite("sentenceTest1", 1, strlen("sentenceTest1"), myFile2);
	FILE * myFile3 = fopen("test2.txt","w");
	fwrite("sentenceTest1", 1, strlen("sentenceTest1"), myFile3);
	fclose(myFile3);
	fclose(myFile2);
	// -> resultat attendu : virus \n nonvirus

	return 0;
}
