/*############# SecondRun    ######################## */
/*############# < Header File > ##################### */
int secondRun(symbolptr *, cmdptr *, listptr *, char *, int, int);
int updateState(symbolptr , char *, int);
cmdptr findAddress(cmdptr , int);
symbolptr findSymbol(symbolptr, char *);
int createOutputFile(cmdptr *, char *,int ,int );
void createBITS(FILE *, int);
int createExternFile(listptr *, symbolptr *, char *);
int addExternalToFile(FILE *, listptr, char *, char *);
symbolptr createExternFile2(FILE *, listptr , symbolptr , char *);
int createEntryFile(symbolptr *, char *);
symbolptr createEntryFile2(FILE *, symbolptr);
int checkFile(char *);
