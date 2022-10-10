myprog : FinalProject.o textProcess.o FirstRun.o SecondRun.o
	gcc -g -Wall -ansi -pedantic FinalProject.o textProcess.o FirstRun.o SecondRun.o -o myprog
FinalProject.o : FinalProject.c utils.h
	gcc -c -Wall -ansi -pedantic FinalProject.c -o FinalProject.o
textProcess.o : textProcess.c utils.h
	gcc -c -Wall -ansi -pedantic textProcess.c -o textProcess.o
FirstRun.o : FirstRun.c utils.h FirstRun.h
	gcc -c -Wall -ansi -pedantic FirstRun.c -o FirstRun.o
SecondRun.o : SecondRun.c utils.h SecondRun.h
	gcc -c -Wall -ansi -pedantic SecondRun.c -o SecondRun.o
