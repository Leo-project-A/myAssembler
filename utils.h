/*############# FILE FORMAT	################# */
#define INPUT_FILE_ENDING ".as"
#define OUTPUT_FILE_ENDING ".ob"
#define OUTPUT_FILE_EXTERNAL ".ext"
#define OUTPUT_FILE_ENTRY ".ent"
#define PREFIX_DIRECTORY "output_files/"

/*############# STRUCTS	######################## */
typedef struct cmd *cmdptr;
typedef struct cmd {			/* cmd is data/code line				*/		
	int address;			/* address - of the code/data				*/
	int value;			/* value - the code/data in decimal			*/
	cmdptr next;
} cmd;

typedef struct text *txtptr;			
typedef struct text {			/* text is the statements reformed for the assembler	*/
	char *word;			/* word - string of arguments				*/
	int type;
	char *value;
	char *value2;
} text;

typedef struct list *listptr;
typedef struct list{ 			/* list is the labels addresses for the 2nd run		*/
	char* label;			/* label - the label need replacing			*/		
	int address;			/* address - which address is the label at		*/			
	listptr next;
} list;

typedef struct symbol *symbolptr;
typedef struct symbol { 		/* the symbol table for the assembler			*/
	char *label;			/* label - the label/symbol				*/
	int type;			/* type - what type? data/sting/etc			*/
	int state;			/* state - external/enternal				*/
	int value;			/* value - the address of the label			*/
	symbolptr left;
	symbolptr right;
} symbol;

/*############# CONSTANS ######################## */

#define START_MEMORY 100
#define MAX_WORD_LENGTH 31
#define BIT_SIZE 14
#define ERROR -1

#define MAX_LINE_LENGTH 80
#define TRUE 1
#define FALSE 0

#define TYPE_DEFAULT 100
#define TYPE_LABEL 101
#define TYPE_DATA 102
#define TYPE_STRING 103
#define TYPE_ENTRY 104
#define TYPE_EXTERN 105 
#define TYPE_GUIDE 106
#define TYPE_COMMAND 107
#define TYPE_MACRO 108
#define UNKOWN -1

#define ARG_INTEGER 0
#define ARG_LABEL 1
#define ARG_ARRAY 2
#define ARG_REGISTER 3

#define ARG_DATA 4
#define ARG_STRING 5

#define STATE_EXTERNAL 10
#define STATE_ENTRY 11

#define GROUP_0 0
#define GROUP_1 1
#define GROUP_2 2

#define POSITION_LABEL 0
#define POSITION_COMMAND 1
#define POSITION_ARG_1 2
#define POSITION_ARG_2 3

#define OPCODE_OFFSET 6
#define SOURCE_TYPE_OFFSET 4
#define SOURCE_VALUE_OFFSET 5
#define DESTINATION_TYPE_OFFSET 2
#define DESTINATION_VALUE_OFFSET 2
#define VALUE_OFFSET 2

#define ARE_ABSOLUTE 0	
#define ARE_EXTERNAL 1
#define ARE_RELOCATABLE 2




