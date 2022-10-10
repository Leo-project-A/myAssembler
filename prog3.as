		;comment

.entry  LIST
.extern W

.define sz = 2
MAIN: mov r445, LIST[sz]

LOOP:		jmp W
prn	#-544+66
	mov STR[5],STR[2]
sub r1   	,r4
MAI434N:	cmp K, #sz
	bne W
	L1: inc L3
.entry LOOP

	bne LOOP
END:
