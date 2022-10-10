/*############# FirstRun  ########################### */
/*############# < Header File > ##################### */
int firstRun(char *, FILE *, symbolptr *, cmdptr *, cmdptr *, listptr *);
char *idenifyType(txtptr);
char *idenifyLabel(char *);
char *trim(char *, int);
txtptr trimLabel(txtptr);
int updateTable(symbolptr *, symbolptr );
symbolptr addToTable(symbolptr , symbolptr );
symbolptr newSymbol(char*, int, int, int );
cmdptr newMemoryCell(listptr , int , int );
int updateMemory(cmdptr *, cmdptr);
int addToMemory(cmdptr , cmdptr);
int isLegalLabel(char *);
int updateData(cmdptr *, txtptr []);
int nextValue(char *);	
int updateString(cmdptr *, char *);	
int updateCode(cmdptr *,  txtptr [], listptr *);
int updateSection(cmdptr *, cmdptr);
int addToSection(cmdptr , cmdptr);
cmdptr newMemoryLine(int, int);
int getValue(symbolptr *, char *);
listptr newList(int, char *);
int updateList(listptr *, listptr);
int addToList(listptr , listptr);
int updateDataAddress(cmdptr *);
int updateTableAddress(symbolptr);
int getOpcodeValue(char *);
int getArgValue(char *, int);
int convertInteger(char *);

/* external variables	*/
extern int IC;
extern int DC;
extern listptr labels;
extern int errorFLAG;
extern int lineCounter;
extern symbolptr symbolTable;
extern cmdptr cmdSection;
extern cmdptr dataSection;



