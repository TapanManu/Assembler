FIBO    START  0
FIRST   LDT    #1
        LDS    #10
        +JSUB  RDREC
        +JSUB  WRREC
LENGTH  RESW   1
.
. SUBROUTINE RDREC
.
RDREC   CLEAR  A
RLOOP   TD     INPUT
        JEQ    RLOOP
        RD     INPUT
        COMPR  A,T
        JLT    RLOOP
        COMPR  A,S
        JGT    RLOOP
        STA    LENGTH
        RSUB
INPUT   BYTE   X'F1'
.
. SUBROUTINE WDREC
.
WRREC   LDX    #0
        LDS    #1
        LDA    #0
        LDT    LENGTH
WLOOP   TD     OUTPUT
        JEQ    WLOOP
        WD     OUTPUT
        ADDR   S,A
        STA    VALUE1
        STS    VALUE2
        LDA    VALUE2
        LDS    VALUE1
        TIXR   T 
        JLT    WLOOP
OUTPUT  BYTE   X'05'
VALUE1  RESW   1
VALUE2  RESW   1
        END    FIRST