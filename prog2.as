		;comment

.entry  LIST
.extern W

.define sz = 2
MAIN: mov r4, LIST[sz]

LOOP		jmp W 
prn	#-5
	mov STR[5],STR[2]
sub r1   	,r4
	cmp K, #sz ,
	bne W
	L1: inc L3
.entry LOOP

	bne LOOP
END: stop

.define len =4
STR: .string "abcddfde  f" fdsgg
LIST: .data 6, -9 , lte
K: .data +22

.extern L3 dd
