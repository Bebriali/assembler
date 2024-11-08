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
    GIN      =  8,
    UOUT     =  9,
    SUB     =  10,
    EMPTY   =  170,
    STX_ERR =  666,
    HLT     = -1
};

enum TypeArgs
{
    REG   = 1,
    NUM   = 2,
    LABEL = 3
};

#endif
