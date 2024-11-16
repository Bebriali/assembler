#ifndef CMD_VALUES
#define CMD_VALUES

enum Commands
{
    PUSH    =  0,
    POP     =  1,
    JMP     =  2,
    JB      =  3,
    JE      =  4,
    ADD     =  5,
    MUL     =  6,
    DIV     =  7,
    GIN     =  8,
    UOUT    =  9,
    SUB     =  10,
    CALL    =  11,
    RET     =  12,
    SQR     =  13,
    JA      =  14,
    EMPTY   =  170,
    STX_ERR =  666,
    HLT     = -1
};

enum TypeArgs
{
    NUM   = 1,
    LABEL = 2,
    REG   = 3
};

#endif
