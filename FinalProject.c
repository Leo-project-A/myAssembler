/*######################################################*/
/*		Final Project:	Assembler		*/
/*	@author: Leonid Sobol				*/
/*	@August_2019					*/
/*######################################################*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "FirstRun.h"
#include "SecondRun.h"
#include "textProcess.h"
#include "FinalProject.h"

/*	main() - recives number of files to translate		*/
int main(int argc, char *argv[])
{
	FILE *fp;
	char *prog; 			/* for the name of the prog			*/
	char *fileName;			/* the fileName of the output files		*/
	char *inputFile;		/* the name of the input file			*/
	int i;
	prog = argv[0]; 

	restartStructs();		/* restart all global veriables			*/

	if(argc == 1)
		printf("%s : No file passed as arguments \n", prog);
	else{
		for(i = 1; i < argc ; i++){
			fileName = argv[i];
			inputFile = strcat(stringCopy(fileName), INPUT_FILE_ENDING);
			if ((fp = fopen(inputFile, "r")) == NULL){
				printf("%s : cant open file %s\n", prog, fileName);
			}
			else{
				assemble(fp, fileName); /* creates the output files */
				fclose(fp);
				restartStructs();
			}
		}
	}	
	return 0;
}

/*	the main engine: calls 1st run, if no errors-> go to 2nd run		*/
int assemble(FILE *fp, char *fileName)	
{
	errorFLAG = FALSE;
	firstRun(fileName, fp, &symbolTable, &cmdSection, &dataSection, &labels);
	if(errorFLAG){				/* if any errors - dont cuntinue the assembly	*/
		printf("%s: error in file : no output created \n", fileName);
		return 1;
	}
	secondRun(&symbolTable, &cmdSection, &labels, fileName, IC, DC);
	return 0;
}

/* restarting all the global veriables			*/
void restartStructs()
{
	symbolTable = NULL;
	cmdSection =  NULL;
	dataSection =  NULL;
	labels =  NULL;

	errorFLAG = FALSE;
	lineCounter = 1;

	IC = START_MEMORY;
	DC = 0;
}







