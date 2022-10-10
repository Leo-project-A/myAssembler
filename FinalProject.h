/*############# FinalProject ######################## */
/*############# < Header File > ##################### */
int assemble(FILE *, char *);
void restartStructs();

symbolptr restartTree(symbolptr);
listptr restartList(listptr);
cmdptr restartSection(cmdptr);

symbolptr symbolTable;	/* the symbol table		*/
cmdptr cmdSection;	/* the instruction code		*/
cmdptr dataSection;	/* the data code		*/
listptr labels;		/* list of labels to replace	*/

int IC;			/* code address counter		*/
int DC;			/* data address counter		*/

int errorFLAG;		/* flag for errors in the file	*/
int lineCounter;	/* file line counter		*/
