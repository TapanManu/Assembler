FIRST  START   1000
        LDX     ZERO
        LDA     ZERO
        STA     INDEX
LOOP    LDA     ALPHA,X
        ADD     BETA,X
        STA     GAMMA,X
        LDA     INDEX
        ADD     THREE
        STA     INDEX
        COMP    D300
        JLT     LOOP
        RSUB
ALPHA   RESW    100
BETA    RESW    100
GAMMA   RESW    100
ZERO    WORD    0
INDEX   RESW    1
THREE   WORD    3
D300    WORD    300
        END