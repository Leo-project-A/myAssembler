#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h" 
#include "SecondRun.h"
#include "textProcess.h"

/* secondRun() - goes through the list of labels needed replaced, and updates addresses		*/
/* symbolTable	: the symbol table for the assembler						*/
/* cmdSection	: the section for the code info							*/
/* labels	: a list of the labels and thier addresses for later updating			*/
int secondRun(symbolptr *symbolTable, cmdptr *codeSection , listptr *labels, char* fileName, int ic, int dc)
{
	cmdptr cmdPointer;
	listptr labelPointer;
	symbolptr symbolPointer;
	char *label;
	int address;
	int AREvalue;

	char *outputFile;
	char *externFile;
	char *entryFile;

	if(codeSection == NULL)
		return 0;
	if(labels == NULL){
		/*no need for updating - Go to file making */
	}
	else{
		cmdPointer = *codeSection;
		labelPointer = *labels;
		while(labelPointer != NULL){
			label = labelPointer->label;
			address = labelPointer->address;
			if(address == -1)
				updateState(*symbolTable, label, STATE_ENTRY);
			else{
				cmdPointer = findAddress(cmdPointer, address); 	/*moves the pointer to the address in the cmdSection */
				symbolPointer = findSymbol(*symbolTable, label);
				AREvalue = (symbolPointer->state == STATE_EXTERNAL)? ARE_EXTERNAL : ARE_RELOCATABLE;
				cmdPointer->value = (symbolPointer->value << VALUE_OFFSET) + AREvalue;
			}
			labelPointer = labelPointer->next;
		}
	}
	outputFile = strcat(strcat(stringCopy(PREFIX_DIRECTORY), stringCopy(fileName)), OUTPUT_FILE_ENDING); 
	externFile = strcat(strcat(stringCopy(PREFIX_DIRECTORY), stringCopy(fileName)), OUTPUT_FILE_EXTERNAL); 
	entryFile = strcat(strcat(stringCopy(PREFIX_DIRECTORY), stringCopy(fileName)), OUTPUT_FILE_ENTRY); 
	createOutputFile(codeSection, outputFile, ic, dc);
	createExternFile(labels, symbolTable, externFile);
	createEntryFile(symbolTable, entryFile);

	if(checkFile(outputFile))
		printf("%s : Output file created successfully. \n", fileName);
	if(checkFile(externFile))
		printf("%s : External file created successfully. \n", fileName);
	if(checkFile(entryFile))
		printf("%s : Entries file created successfully. \n", fileName);

	return 0;
}
/* checks if the file created is empty. if so - delete it. no need for it	*/
int checkFile(char *fileName)
{
	FILE *fp;
	char c;
	fp = fopen(fileName, "r");
	if((c = getc(fp)) == EOF){
		remove(fileName);
		return FALSE;
	}
	ungetc(c, fp);
	fclose(fp);
	return TRUE;
}
/* creates the out file with the code in special binary			*/
int createOutputFile(cmdptr *codeSection, char *fileName, int ic ,int dc)
{ 
	cmdptr node;
	FILE *fp;
	if(codeSection == NULL) /*code is empty */
		return 0;
	node = *codeSection; 		/* first entry in the code file */
	if ((fp = fopen(fileName, "w")) == NULL){
		printf("cant create file %s\n", fileName);
	}
	else{
		fprintf(fp, "\t %d %d", (ic - START_MEMORY), dc);
		fprintf(fp, "\n");
		while(node != NULL){
			fprintf(fp, "\t");
			fprintf(fp, "%d" ,node->address);
			fprintf(fp, "\t");
			createBITS(fp, node->value);
			fprintf(fp, "\n");
			node = node->next;
		}
	}
	fclose(fp);
	return 0;
}
/* prints in the file bit by bit				*/
void createBITS(FILE *fp, int num)
{
	int bit;
	int temp = 1 << (BIT_SIZE - 1);
	while(temp > 0){
		bit = ((temp & num) > 0)? 1 : 0 ;
		fprintf(fp, "%d", bit);
		temp = temp >> 1;
	}
}
/* creates the externals file with the symbol table			*/
int createExternFile(listptr *labels, symbolptr *symbolTable, char *fileName)
{
	FILE *fp;
	if ((fp = fopen(fileName, "w")) == NULL)
		printf("cant create file %s\n", fileName);

	if((symbolTable == NULL) || (labels == NULL))
		return 0;	
	createExternFile2(fp, *labels, *symbolTable, fileName);
	fclose(fp);
	return 0;
}
symbolptr createExternFile2(FILE *fp, listptr labels, symbolptr node, char *fileName)
{
	if(node == NULL){
		return NULL;		
	}
	if(node->state == STATE_EXTERNAL)
		addExternalToFile(fp, labels, node->label, fileName);
	createExternFile2(fp, labels, node->left, fileName);
	createExternFile2(fp, labels, node->right, fileName);
	return NULL;
}
int addExternalToFile(FILE *fp, listptr node, char *label, char *fileName)
{	
	while(node != NULL){
		if(strcmp(node->label, label) == 0)
			fprintf(fp, "\t%s\t%d\n", label, node->address);		
		node = node->next;
	}
	return 0; 
}
/* creates the entries file with the symbol table			*/
int createEntryFile(symbolptr *symbolTable, char *fileName)
{
	FILE *fp;
	if ((fp = fopen(fileName, "w")) == NULL){
		printf("cant create file %s\n", fileName);
		return 1;
	}
	if(symbolTable == NULL)
		return 0;	
	createEntryFile2(fp, *symbolTable);
	fclose(fp);
	return 0;
}
symbolptr createEntryFile2(FILE *fp, symbolptr node)
{
	if(node == NULL){
		return NULL;		
	}
	if(node->state == STATE_ENTRY)
		fprintf(fp, "\t%s\t%d\n", node->label, node->value);
	createEntryFile2(fp, node->left);
	createEntryFile2(fp, node->right);
	return NULL;
}
symbolptr findSymbol(symbolptr node, char *label)
{
	int cond;
	while(node != NULL){
		if((cond = strcmp(node->label, label)) == 0)
			return node;
		else if(cond < 0)
			node = node->left;
		else 
			node = node->right;
	}
	return NULL;
}
/* updates the state of the symbol : extern/entry			*/
int updateState(symbolptr node, char *label, int state)
{
	if(node == NULL)
		return 0;
	else if(strcmp(node->label, label) == 0)
		node->state = state;
	else{
		updateState(node->left, label, state);
		updateState(node->right, label, state);
	}
	return 0;
}
/* finds the code line with the address			*/
cmdptr findAddress(cmdptr pointer, int address)
{
	while(pointer->address != address)
		pointer = pointer->next;
	return pointer;
}
