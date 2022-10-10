#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "utils.h"
#include "textProcess.h"

/*################################################### */
/*############# Constant List : words ############### */
const char *PROTECTED_LIST[] = {
	"define",
	"extern",
	"entry",
	"string",
	"data",
	"if",
	"else",
	"while"
};
#define PROTECTED_LIST_SIZE (sizeof (PROTECTED_LIST) / sizeof (const char *))
const char *COMMAND_LIST[] = {
	"mov",
	"cmp",
	"add",
	"sub",
	"not",
	"clr",
	"lea",
	"inc",
	"dec",
	"jmp",
	"bne",
	"red",
	"prn",
	"jsr",
	"rts",
	"stop"

};
#define COMMAND_LIST_SIZE (sizeof (COMMAND_LIST) / sizeof (const char *))
const char *REGISTER_LIST[] = {
	"r1",
	"r2",
	"r3",
	"r4",
	"r5",
	"r6",
	"r7",
	"r8"
};
#define REGISTER_LIST_SIZE (sizeof (REGISTER_LIST) / sizeof (const char *))
/*################################################### */

/* reads the file and creates a list of words from the next line */
/* also check syntax ; if iligal returns - NULL */
/* if legal - retruns text line fromated	*/
int newTextLine(char *fileName, FILE *fp, txtptr statement[], int lineCounter)		
{
	char line[MAX_LINE_LENGTH + 1];		/* the entire line			*/
	char word[MAX_LINE_LENGTH + 1];		/* single word in the line		*/

	int statementIndex = 0;
	txtptr tempText;
	int index = 0;
	int len;
	int cmdFLAG = FALSE;			/* flag to know if the next words should be arguments.	*/
	int dataFLAG = FALSE;			/* flag to know if the next word should be data argument.	*/
	int commaFLAG = FALSE;
	int stringFLAG = FALSE;			/* flag to know if the next word should be string argument.	*/
	int macroFLAG = FALSE;
	int labelFLAG = FALSE;
	int eFLAG = FALSE;			/* flag to know if the next word should be entry/extern argument.	*/	
	int type;
	char *value;
		
	fgets(line, MAX_LINE_LENGTH + 1, fp);		/* reads the next line from the file	*/
	line[MAX_LINE_LENGTH] = '\n';
	index = nextChar(line, index);			/* index gets the index of the next character	*/

	while((index <= MAX_LINE_LENGTH)&&(line[index] != '\n')&&(line[index] != ';')&&(line[index] != EOF)&&(line[index] != '\0')){		
								/* while there are still chars in the line	*/			
		len = nextWord(line, index, word);	/* len = the length of the next word				*/
		tempText = malloc(sizeof(text));	/* create new text struct (all NULL)				*/
		if((word[len-1] == ':') && (statementIndex == 0) && !labelFLAG){
			word[len-1] = '\0';		/* the word is label -> 	type = "label"			*/
			if(!isLegalLabel(word))	{	/* 				value = word - ':'		*/
				printf("Syntax Error: %s:%d: label %s is illegal\n", fileName, lineCounter, word);
				return ERROR;
			}
			labelFLAG = TRUE;
			addToStatement(statement, statementIndex, tempText, TYPE_LABEL, stringCopy(word), NULL);
		}
		else {
			if(statementIndex == 0)
				statementIndex++;
			if(dataFLAG){				/* the word is data arg ->	type = "arg_data"		*/
				if(commaFLAG){			/* 				value = word			*/
					if(word[0] != ','){
						printf("Syntax Error: %s:%d: argument ',' missing from statement \n", fileName, lineCounter);
						return ERROR;
					}			
					else{
						commaFLAG = FALSE;
					}
				}
				else{
					if(!isLegalInteger(word) && !isLegalLabel(word)) {
						printf("Syntax Error: %s:%d: illegal integer %s \n", fileName, lineCounter, word);
						return ERROR;
					}
					addToStatement(statement, statementIndex++, tempText, ARG_DATA, stringCopy(word), NULL);
					commaFLAG = TRUE;
				}		
			}
			else if(stringFLAG){				/* the word is string arg -> 	type = "arg_string"		*/
				if(statementIndex == 2){		/* 				value = word			*/
					if(word[0] != '"'){
						printf("Syntax Error: %s:%d: String argument must begin with \" \n", fileName, lineCounter);
						return ERROR;
					}
					else{
						setRestOfLine(statement , statementIndex, tempText, word, line, index);
						if(word[strlen(word) - 1] != '"'){
							printf("Syntax Error: %s:%d: String argument must end with \"\n", fileName, lineCounter);
							return ERROR;
						}
						return statementIndex+1;
					}
				}
			}
			else if(macroFLAG){				/* the word is macro arg -> 	type = "arg_macro"		*/
				switch(macroFLAG){			/* 				value = word			*/
					case 1:			/* 1st time in macro => must be label*/
						if(!isLegalLabel(word)){
							printf("Syntax Error: %s:%d: %s is illegal macro \n", fileName, lineCounter, word);
							return ERROR;
						}
						addToStatement(statement, statementIndex++, tempText, TYPE_LABEL, stringCopy(word), NULL);
						macroFLAG++;		/* after label, the flag moves to => 2	*/
						break;
					case 2:			/* 2nd time in macro => must be '='	*/
						if(word[0] != '='){
							printf("Syntax Error: %s:%d: missing '=' in statement \n", fileName, lineCounter);
							return ERROR;
						}
						macroFLAG++;
						break;
					case 3:			/* 3rd time in macro => must be integer	*/
						if(!isLegalInteger(word)){
							printf("Syntax Error: %s:%d: illegal integer %s \n", fileName, lineCounter, word);
							return ERROR;
						}
						addToStatement(statement, statementIndex++, tempText, ARG_INTEGER, stringCopy(word), NULL);
						macroFLAG++;
						break;
					default:		/* 4th time in macro => illegal-to many args	*/
						printf("Syntax Error %s:%d: to many arguments in macro statement \n", fileName, lineCounter);
						return ERROR;	
				}
			}
			else if(eFLAG){	
				if(eFLAG > 1){
					printf("Syntax Error: %s:%d too many arguments \n", fileName, lineCounter);
					return ERROR;
				}
				if(!isLegalLabel(word)){
					printf("Syntax Error: %s:%d: %s is illegal label \n", fileName, lineCounter, word);
					return ERROR;
				}
				addToStatement(statement, statementIndex++, tempText, TYPE_LABEL, stringCopy(word), NULL);
				eFLAG++;
			}
			else if(cmdFLAG){			/* the word is argument -> 	type = get_arg_type (register/integer...)*/
				if(commaFLAG){		
					if(word[0] != ','){
						printf("Syntax Error: %s:%d: argument ',' missing from statement \n", fileName, lineCounter);
						return ERROR;
					}			
					else{
						commaFLAG = FALSE;
					}
				}
				else{
					type = getArgType(word);
					if(!isLegalArg(word, type)){	/* illigal argument			*/
						printf("Syntax Error: %s:%d: %s is illegal argument \n", fileName, lineCounter, word);
						return ERROR;
					}
					addArgumentToStatement(statement, statementIndex++, tempText, type, stringCopy(word));
					commaFLAG = TRUE;
				}
			}
			else if(word[0] == '.'){			/* the word is guide -> 	type = "guide"			*/
				type = getType(removePrefix(word));	/* 				value = word - '.'		*/
				switch(type){
					case TYPE_MACRO:
						if(labelFLAG){
							printf("Syntax Error: %s:%d: label no allowed with macro \n", fileName, lineCounter);
							return ERROR;
						}
						macroFLAG = TRUE;		/* flag->next word is definitaion of macro	*/		
						addToStatement(statement, statementIndex++, tempText, TYPE_MACRO, stringCopy(word), NULL);
						break;
					case TYPE_DATA:
						dataFLAG = TRUE;	/* flag->next word is data			*/		
						addToStatement(statement, statementIndex++, tempText, TYPE_DATA, stringCopy(word), NULL);
						break;
					case TYPE_STRING:
						stringFLAG = TRUE;	/* flag->next word is string			*/		
						addToStatement(statement, statementIndex++, tempText, TYPE_STRING, stringCopy(word), NULL);
						break;
					case TYPE_EXTERN:
						eFLAG = TRUE;		/* flag->next word is label for entry/external	*/		
						addToStatement(statement, statementIndex++, tempText, TYPE_EXTERN, stringCopy(word), NULL);
						break;
					case TYPE_ENTRY:
						eFLAG = TRUE;		/* flag->next word is label for entry/external	*/		
						addToStatement(statement, statementIndex++, tempText, TYPE_ENTRY, stringCopy(word), NULL);
						break;
					default:
						printf("Syntax Error: %s:%d: %s is illegal guide statement \n", fileName, lineCounter, word);
						return ERROR;
						break;
				}
			}					
			else{					/* the word is command ->	type = "command"	*/
				cmdFLAG = TRUE;			/* flag->next words are args.	value = word		*/		
				if(!isLegalCommand(word)){	
					printf("Syntax Error: %s:%d: %s is illegal command \n", fileName, lineCounter, word);
					return ERROR;
				}	
				addToStatement(statement, statementIndex++, tempText, TYPE_COMMAND, stringCopy(word), NULL);
			}
		}
		index+=len;
		index = nextChar(line, index);
	}

	if(statementIndex <= 1){
		return 0;
	}
	type = statement[POSITION_COMMAND]->type;
	value = statement[POSITION_COMMAND]->value;
	if((type == TYPE_DATA) && (commaFLAG))
		return statementIndex;
	else if(type == TYPE_COMMAND){
		if(getGroup(value) != (statementIndex-2)){
			printf("Syntax Error: %s:%d: argument number is incorrect \n", fileName, lineCounter);
			return ERROR;
		}
		else if((getGroup(value) != GROUP_0) && !commaFLAG){
			printf("Syntax Error: %s:%d: statement cant end with ',' \n", fileName, lineCounter);
			return ERROR;
		}
	}
	return statementIndex;
}
/*restarts the array and cleans it up */
int restart(txtptr statement[], int length)
{
	int i;
	for (i = 0; i < length ; i++){
		statement[i] = NULL;
	}
	return 0;
}
/* returns int type of the command */
int getType(char *word)
{
	if(strcmp(word, "define") == 0)
		return TYPE_MACRO;
	else if(strcmp(word, "data") == 0)
		return TYPE_DATA;
	else if(strcmp(word, "string") == 0)
		return TYPE_STRING;
	else if(strcmp(word, "extern") == 0)
		return TYPE_EXTERN;
	else if(strcmp(word, "entry") == 0)
		return TYPE_ENTRY;
	else
		return UNKOWN;
}
/* returns the group of the command(how many args) */
int getGroup(char *command)
{
	if ((strcmp(command, "mov") == 0) || (strcmp(command, "sub") == 0) || (strcmp(command, "cmp") == 0) || (strcmp(command, "add") == 0) || 				(strcmp(command, "lea") == 0)){
		return GROUP_2;
	}
	if ((strcmp(command, "rts") == 0) || (strcmp(command, "stop") == 0)){
		return GROUP_0;
	}
	else{
		return GROUP_1;
	}
}
/* adds the argument string to the statement array. its the rest of the line for the string	*/
int setRestOfLine(txtptr statement[], int statementIndex, txtptr tempText,char *word, char *line, int index)
{
	int j = 0;
	while(line[index] != '\n'){
		word[j] = line[index];
		index++;
		j++;
	}
	word[j] = '\0';
	addToStatement(statement, statementIndex, tempText, ARG_STRING, stringCopy(word), NULL);
	return 0;
}
/* adds the argument data to the statement array. if the argument is ARRAY_TYPE it adds both the label=>value and the index=>value2	*/
int addArgumentToStatement(txtptr statement[], int statementIndex, txtptr tempText, int type, char *value)
{
	char *arrayName;
	char *arrayIndex;
	if(type != ARG_ARRAY){
		addToStatement(statement, statementIndex, tempText, type, value, NULL);
		return 0;
	}
	arrayName = getArrayName(stringCopy(value));
	arrayIndex = getArrayIndex(value);
	addToStatement(statement, statementIndex, tempText, type, arrayName, arrayIndex);
	return 0;
}
/* returns the "label" of the array argument label[index]		*/
char *getArrayName(char *value)
{
	int i = 0;
	while(value[i] != '['){
		value[i] = value[i];
		i++;
	}
	value[i] = '\0';
	return value;
}
/* returns the "index" of the array argument label[index]		*/
char *getArrayIndex(char *value)
{
	int i = 0, j = 0;
	while(value[i] != '['){
		i++;
	}
	i++;
	while(value[i] != ']'){
		value[j] = value[i];
		i++;
		j++;
	}
	value[j] = '\0';
	return value;
}
/* adds the word data to "tempText", and adds it to the statement array	*/
int addToStatement(txtptr statement[], int statementIndex, txtptr tempText, int type, char *value, char *value2)
{
	tempText->type = type;		
	tempText->value = value;
	tempText->value2 = value2;
	statement[statementIndex] = tempText;
	return 0;
}
/*returns the next character in "line" starting from "index" ; ignoring all spaces and tabs	*/
int nextChar(char *line, int index) 
{	
	while((line[index] == ' ') || (line[index] == '\t'))
		index++;
	return index;
}
/* places the next word in "line" starting from "index" -> into "word" ; returns the length the word	*/
int nextWord(char *line, int index, char *word)
{
	int pointer = 0;
	if((line[index] == ',') || (line[index] == '=')){
		word[pointer] = line[index];
		word[++pointer] = '\0';
		return pointer; 
	}
	while((line[index] != ' ') && (line[index] != '\t') && (line[index] != '\n') && (line[index] != EOF)
			 && (line[index] != ',') && (line[index] != '=')){
		word[pointer] = line[index];
		index++;
		pointer++;
	}
	word[pointer] = '\0';	/* the word ends with \0	*/
	return pointer;		/* the length of the word 	*/
}
/* returns the type of the argument	*/
int getArgType(char *word)
{
	int i = 0;
	char c = word[i];
	if(c == '#')
		return ARG_INTEGER;
	if(isRegister(word))
		return ARG_REGISTER;
	while(c != '['){
		if(c == '\0')
			return ARG_LABEL;
		i++;
		c = word[i];
	}
	while(c != ']'){
		if(c == '\0')
			return ARG_LABEL;
		i++;
		c = word[i];
	}	
	return ARG_ARRAY;	
}
/* returns TRUE if word is in the restricted words list	*/
int isRegister(char *word)
{
	int i;
	for(i = 0; i< REGISTER_LIST_SIZE; i++){
		if(strcmp(REGISTER_LIST[i], word) == 0)
			return TRUE;
	}
	return FALSE;
}
/* returns TRUE if word is protected	*/
int checkRestricted(char *word)
{
	int i;
	for(i = 0; i< PROTECTED_LIST_SIZE; i++){
		if(strcmp(PROTECTED_LIST[i], word) == 0)
			return TRUE;
	}
	return FALSE;
}
/* returns TRUE if command is legal	*/
int isLegalCommand(char *word)
{
	int i;
	for(i = 0; i< COMMAND_LIST_SIZE; i++){
		if(strcmp(COMMAND_LIST[i], word) == 0)
			return TRUE;
	}
	return FALSE;
}
/* returns TRUE if label is legal	*/
int isLegalLabel(char *word)
{
	int i;
	if(checkRestricted(word) || isRegister(word) || isLegalCommand(word)){
		return FALSE;
	}
	i = 0;
	if(!isalpha(word[i++])){
		return FALSE;
	}
	while(word[i] != '\0'){
		if((i > MAX_WORD_LENGTH) || (!isalpha(word[i]) && !isdigit(word[i]))){
			return FALSE;
		}
		i++;
	}
	return TRUE;
}
/* returns TRUE if integer is legal	*/
int isLegalInteger(char *word)
{
	int i = 0;
	if((word[i] == '-') || (word[i] == '+'))
		i++;
	while(word[i] != '\0'){
		if(!isdigit(word[i]))
			return FALSE;
		i++;
	}
	return TRUE;
}
/* returns TRUE if argument is legal	*/
int isLegalArg(char *word, int type)
{	
	char *arrayName;
	char *arrayIndex;
	switch(type){
		case ARG_REGISTER:
			return TRUE;
		case ARG_INTEGER:
			removePrefix(word);
			return (isLegalInteger(word) || isLegalLabel(word));
		case ARG_ARRAY:
			arrayName = getArrayName(stringCopy(word));
			arrayIndex = getArrayIndex(stringCopy(word));
			return (isLegalLabel(arrayName) && (isLegalInteger(arrayIndex) || isLegalLabel(arrayIndex)));
		default:/* label	*/
			return isLegalLabel(word);
	}
}
/* returns a copy of the string s	*/
char *stringCopy(char *s)
{
	char *p;
	p = (char*) malloc(strlen(s)+1);
	if(p != NULL)
		strcpy(p, s);
	return p;
}
/* removes the prefix(the first char) of the string s 	*/
char *removePrefix(char *s)
{
	int i = 1;
	while(s[i] != '\0'){
		s[i-1] = s[i];
		i++;
	}
	s[i-1] = '\0';
	return s;
}
