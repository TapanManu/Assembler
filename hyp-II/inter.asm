0	HYP	START	0
0	A	DB	0
1	C	EQU	100
1		LDI	R0,C
3		PUSH	R0
4		POP	R1
5		ADD	R2,R0,R1
7		MUL	R3,R2,R1
9		SUB	R15,R15,R15
b		CALL	FN
d		JZ	R4,L1
f		JGTZ	R4,L1
11		JLTZ	R4,L1
13	HALT	HLT
14	FN	LD	R10,A
16		DIV	R11,R10,R0
18		RET
19	L1	ST	R0,A
1b		MUL	R14,R13,R12
1d		JNZ	R0,HALT
1f		END
