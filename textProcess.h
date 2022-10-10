/*############# text Process ######################## */
/*############# < Header File > ##################### */
int restart(txtptr [], int);
void printTheLine(txtptr [], int);
int newTextLine(char *, FILE *, txtptr [], int);

int getType(char *);

int addArgumentToStatement(txtptr [], int , txtptr , int, char *);
char *getArrayName(char *);
char *getArrayIndex(char *);
int addToStatement(txtptr [], int , txtptr , int, char *, char *);
int nextChar(char *, int );
int nextWord(char *, int , char *);
int getArgType(char *);
int isRegister(char *);
int isLegalCommand(char *);
int isLegalLabel(char *);
int isLegalInteger(char *);
int isLegalArg(char *, int);
int checkRestricted(char *);
char *stringCopy(char *);
char *removePrefix(char *);
int setRestOfLine(txtptr [], int, txtptr,char *, char *, int);

int checkStatement(txtptr [], int , int ,char *);
int getGroup(char *);



char *stringCopy(char *);
