#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h" 
#include "FirstRun.h"
#include "textProcess.h"

extern int errorFLAG;
/* firstRun() - goes through the text file and translates it to special binay			*/
/* symbolTable	: the symbol table for the assembler						*/
/* cmdSection	: the section for the code info							*/
/* dataSection	: the section for the data info (data/string)					*/
/* labels	: a list of the labels and thier addresses for later updating			*/
int firstRun(char *fileName, FILE *fp, symbolptr *symbolTable ,cmdptr *cmdSection, cmdptr *dataSection, listptr *labels)
{
	int lineCounter, length, counter;
	char c;
	int type;
	txtptr statement[MAX_LINE_LENGTH + 1];	/* the statement in the line		*/
	lineCounter = 1;
	while((c = getc(fp)) != EOF){
		ungetc(c, fp);
		restart(statement, MAX_LINE_LENGTH);
		length = newTextLine(fileName, fp, statement, lineCounter);
		if(length == ERROR){		/* flag an error */			
			errorFLAG = TRUE;
		}
		else if(length >= 1){
			length -= 2;
			type = statement[POSITION_COMMAND]->type;
			counter = ((type == TYPE_DATA) || (type == TYPE_STRING))? DC : IC;

			if(statement[POSITION_LABEL] != NULL){
				updateTable(symbolTable, newSymbol(statement[POSITION_LABEL]->value, type, UNKOWN, counter));
			}
			switch(type){
				case TYPE_MACRO:
					updateTable(symbolTable, newSymbol(statement[POSITION_ARG_1]->value, type, UNKOWN, convertInteger(statement[POSITION_ARG_2]->value)));
					break;
					case TYPE_DATA:
					if(updateData(dataSection, statement) == ERROR){	/* if dada - update integers 	*/
						printf("Error: %s:%d: macro not defined\n", fileName, lineCounter);
						errorFLAG = TRUE;
					}
					break;
				case TYPE_STRING:
					updateString(dataSection, statement[POSITION_ARG_1]->value);	/* if string - update chars	*/
					break;
				case TYPE_EXTERN:							/* if extern - add to symbol table*/
					updateTable(symbolTable, newSymbol(statement[POSITION_ARG_1]->value, type, STATE_EXTERNAL, (int)NULL));
					break;
				case TYPE_ENTRY:
					updateList(labels, newList(-1, statement[POSITION_ARG_1]->value));/* if entry - add to list update*/
					break;
				case TYPE_COMMAND:
					updateCode(cmdSection, statement, labels);			/*if instruction - update code	*/
					break;
			}
		}
		lineCounter++;				
	}
	updateDataAddress(dataSection);
	updateTableAddress(*symbolTable);
	updateSection(cmdSection, *dataSection);
	return 0;
}

/* updates the addresses of the data code, with IC			*/
int updateDataAddress(cmdptr *head)
{
	cmdptr node;
	if(head == NULL)
		return 0;
	node = *head;
	while(node != NULL){
		node->address = node->address + IC;
		node = node->next;
	}
	return 0;
}
/* updates all the addersses in the symbol table with +IC			*/
int updateTableAddress(symbolptr node)
{
	if(node == NULL)
		return 0;
	else if((node->type == TYPE_DATA) || (node->type == TYPE_STRING))
		node->value = node->value + IC;
	else{
		updateTableAddress(node->left);
		updateTableAddress(node->right);
	}
	return 0;
}
/* updates the data section with all the new integers			*/
int updateData(cmdptr *dataSection, txtptr statement[]) 
{
	int value;
	int i = POSITION_ARG_1;
	char c;
	while(statement[i] != NULL){
		c = (statement[i]->value)[0];
		if((c >= 'A') && (c <= 'z')){
			if((value = getValue(&symbolTable, statement[i]->value)) == (int)NULL){
				return ERROR;}
		}
		else {
			value = convertInteger(statement[i]->value);}
		updateSection(dataSection, newMemoryLine(DC, value));
		i++;
		DC++;
	}
	return 0;
}		
/* converting char* number to int			*/
int convertInteger(char * s)
{
	int sign = 1;
	int i = 0;
	int sum = 0;
	char c = s[i];
	if(c == '-'){
		sign = -1;
		i++;
	}
	if(c == '+')
		i++;
	while(i < strlen(s)){
		sum = (sum*10)+(s[i] - '0');
		i++;
	}
	return sum*sign;
}
/* updates the data section with the chars of String s			*/	
int updateString(cmdptr *dataSection, char *s) 
{
	int i = 1;
	char c;
	int len = strlen(s) - 1;
	while(i < len){
		c = s[i];
		updateSection(dataSection, newMemoryLine(DC, c));
		i++;
		DC++;
	}
	updateSection(dataSection, newMemoryLine(DC, '\0'));
	DC++;
	return 0;
}		
/* updates the code section by the rules of the assembler			*/
int updateCode(cmdptr *cmdSection, txtptr statement[], listptr *labels) 
{
	int value = 0;
	int sourceType = 0;	
	char *sourceValue;
	int destinationType = 0; 
	char *destinationValue; 
	int argGroup = 0;
	char *command = statement[POSITION_COMMAND]->value;
	argGroup = getGroup(command);
	switch(argGroup){
		case 0:
			/* no arguments for the command */
			break;
		case 1:
			destinationType = statement[POSITION_ARG_1]->type;
			destinationValue = statement[POSITION_ARG_1]->value;
			break;
		case 2:	
			sourceType = statement[POSITION_ARG_1]->type;
			sourceValue = statement[POSITION_ARG_1]->value;
			destinationType = statement[POSITION_ARG_2]->type;
			destinationValue = statement[POSITION_ARG_2]->value;
			break;
	}
	/* the command line	*/
	value = (getOpcodeValue(command) << OPCODE_OFFSET) + (sourceType << SOURCE_TYPE_OFFSET) + (destinationType << DESTINATION_TYPE_OFFSET);
	updateSection(cmdSection, newMemoryLine(IC, value));
	IC++;
	
	switch(argGroup){
		case 0:
			/* no arguments for the command */
			break;
		case 1:
			switch(destinationType){
				case ARG_ARRAY:
					updateSection(cmdSection, newMemoryLine(IC, (int)NULL)); 	/* to be updated in the second run	*/
					updateList(labels, newList(IC, destinationValue));
					IC++;
					value = getArgValue(statement[POSITION_ARG_1]->value2, ARG_ARRAY) << VALUE_OFFSET;
					updateSection(cmdSection, newMemoryLine(IC, value));
					IC++;
					break;
				case ARG_REGISTER:
					value = getArgValue(destinationValue, ARG_REGISTER) << SOURCE_VALUE_OFFSET;
					updateSection(cmdSection, newMemoryLine(IC, value));
					IC++;
					break;
				case ARG_LABEL:
					updateSection(cmdSection, newMemoryLine(IC, (int)NULL));
					updateList(labels, newList(IC, destinationValue));
					IC++;	
					break;
				case ARG_INTEGER:
					value = getArgValue(destinationValue, ARG_INTEGER) << VALUE_OFFSET;
					updateSection(cmdSection, newMemoryLine(IC, value));
					IC++;
					break;
			}
			break;
		case 2:
			if((sourceType == ARG_REGISTER) && (destinationType == ARG_REGISTER)){
				value = (getArgValue(sourceValue, ARG_REGISTER) << SOURCE_VALUE_OFFSET) + (getArgValue(destinationValue, ARG_REGISTER) << DESTINATION_VALUE_OFFSET);
				updateSection(cmdSection, newMemoryLine(IC, value));
				IC++;
			}
			else{
				switch(sourceType){
					case ARG_ARRAY:
						updateSection(cmdSection, newMemoryLine(IC, (int)NULL));
						updateList(labels, newList(IC, sourceValue));
						IC++;
						value = getArgValue(statement[POSITION_ARG_1]->value2, ARG_ARRAY) << VALUE_OFFSET;
						updateSection(cmdSection, newMemoryLine(IC, value));
						IC++;	
						break;
					case ARG_REGISTER:
						value = getArgValue(sourceValue, ARG_REGISTER) << SOURCE_VALUE_OFFSET;
						updateSection(cmdSection, newMemoryLine(IC, value));
						IC++;
						break;
					case ARG_LABEL:
						updateSection(cmdSection, newMemoryLine(IC, (int)NULL));
						updateList(labels, newList(IC, sourceValue));
						IC++;	
						break;
					case ARG_INTEGER:
						value = getArgValue(sourceValue, ARG_INTEGER) << VALUE_OFFSET;
						updateSection(cmdSection, newMemoryLine(IC, value));
						IC++;	
						break;
				}
				switch(destinationType){
					case ARG_ARRAY:
						updateSection(cmdSection, newMemoryLine(IC, (int)NULL));
						updateList(labels, newList(IC, destinationValue));
						IC++;
						value = (getArgValue(statement[POSITION_ARG_2]->value2, ARG_ARRAY)) << VALUE_OFFSET;
						updateSection(cmdSection, newMemoryLine(IC, value));
						IC++;
						break;
					case ARG_REGISTER:
						value = getArgValue(destinationValue, ARG_REGISTER) << SOURCE_VALUE_OFFSET;
						updateSection(cmdSection, newMemoryLine(IC, value));
						IC++;
						break;
					case ARG_LABEL:
						updateSection(cmdSection, newMemoryLine(IC, (int)NULL));
						updateList(labels, newList(IC, destinationValue));
						IC++;	
						break;
					case ARG_INTEGER:
						value = getArgValue(destinationValue, ARG_INTEGER) << VALUE_OFFSET;
						updateSection(cmdSection, newMemoryLine(IC, value));
						IC++;	
						break;
				}
			}
			break;
	}	
	return 0;
}
/* updates the list of lables for the second run address update			*/
int updateList(listptr *labels, listptr newEntry)
{
	if (*labels == NULL)
		*labels = newEntry;
	else
		addToList(*labels, newEntry);
	return 0;
}
int addToList(listptr labels, listptr newEntry)
{
	while(labels->next != NULL)
		labels = labels->next;
	labels->next = newEntry;
	return 0;
}
/* creates new listing struct Entry			*/
listptr newList(int address, char *name)
{
	listptr temp = malloc(sizeof(list));
	temp->address = address;
	temp->label = name;
	temp->next = NULL;
	return temp;
}
/* returns the int value of the arguent given "name"			*/
int getArgValue(char *name, int type)
{
	int tempValue;
	switch(type){
		case ARG_INTEGER:
			if((tempValue = getValue(&symbolTable, name)) == 0)
				return convertInteger(name);
			return tempValue;		
		case ARG_LABEL:
			return getValue(&symbolTable, name);
			break;

		case ARG_ARRAY:
			if((tempValue = getValue(&symbolTable, name)) == 0)
				return convertInteger(name);
			return tempValue;		
			break;

		case ARG_REGISTER:
			return (name[1] - '0');
			break;
		default:
			return 0;
	}
}
/* returns the int value of the command "name"			*/
int getOpcodeValue(char *name)
{
	if (strcmp(name, "mov") == 0)
		return 0;
	if (strcmp(name, "cmp") == 0)
		return 1;
	if (strcmp(name, "add") == 0)
		return 2;
	if (strcmp(name, "sub") == 0)
		return 3;
	if (strcmp(name, "not") == 0)
		return 4;
	if (strcmp(name, "clr") == 0)
		return 5;
	if (strcmp(name, "lea") == 0)
		return 6;
	if (strcmp(name, "inc") == 0)
		return 7;
	if (strcmp(name, "dec") == 0)
		return 8;
	if (strcmp(name, "jmp") == 0)
		return 9;
	if (strcmp(name, "bne") == 0)
		return 10;
	if (strcmp(name, "red") == 0)
		return 11;
	if (strcmp(name, "prn") == 0)
		return 12;
	if (strcmp(name, "jsr") == 0)
		return 13;
	if (strcmp(name, "rts") == 0)
		return 14;
	if (strcmp(name, "stop") == 0)
		return 15;
	else
		return ERROR;
}
/* updates the wanted section(data or code) with the new Entry of data			*/
int updateSection(cmdptr *section, cmdptr newEntry)
{
	if (*section == NULL)
		*section = newEntry;
	else
		addToSection(*section, newEntry);
	return 0;
}
int addToSection(cmdptr section, cmdptr newEntry)
{
	while(section->next != NULL)
		section = section->next;
	section->next = newEntry;
	return 0;
}
/* creates new struct of memory for the data/code section			*/
cmdptr newMemoryLine(int counter,int value)
{
	cmdptr temp = malloc(sizeof (cmd));
	temp->address = counter;
	temp->value = value;
	temp->next = NULL;
	return temp;
}
/* creates new struct of symbol						*/
symbolptr newSymbol(char* label, int type, int state, int value)
{
	symbolptr newEntry;
	newEntry = malloc(sizeof (symbol));
	newEntry->label = label;
	newEntry->type = type;
	newEntry->state = state;
	newEntry->value = value;
	newEntry->left = newEntry->right = NULL;
	return newEntry;
}
/* updates the symbol table with the new Entry			*/
int updateTable(symbolptr *head, symbolptr newEntry)
{
	if(*head == NULL)
		*head = newEntry;	
	else
		addToTable(*head, newEntry);
	return 0;
}
symbolptr addToTable(symbolptr node, symbolptr newEntry)
{	
	int cond;
	if(node == NULL) 
		node = newEntry;
	else if((cond = strcmp(node->label, newEntry->label)) == 0){
		/* label already exists */
		errorFLAG = TRUE;
		printf("The label '%s' already exists in symbol Table \n", newEntry->label);
	}
	else if (cond < 0)
		node->left = addToTable(node->left, newEntry);
	else 
		node->right = addToTable(node->right, newEntry);
	return node;
}
/* returns the value of the label from the symbol table			*/
int getValue(symbolptr *head, char *label)
{
	int cond;
	symbolptr node;
	if(head == NULL)
		return (int)NULL;
	node = *head;
	while(node != NULL){
		if((cond = strcmp(node->label, label)) == 0)
			return node->value;
		else if(cond < 0)
			node = node->left;
		else 
			node = node->right;
	}
	return (int)NULL;
}
/* updates the memort with the new Entry			*/
int updateMemory(cmdptr *start, cmdptr newEntry)
{
	if(*start == NULL)
		*start = newEntry;
	else
		addToMemory(*start, newEntry);
	return 0;
}
int addToMemory(cmdptr node, cmdptr newEntry)
{
	while(node->next != NULL)
		node = node->next;
	node->next =  newEntry;
	return 0;
}
