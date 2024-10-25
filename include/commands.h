#ifndef CMD_VALUES
#define CMD_VALUES

enum Commands
{
    PUSH = 0,
    POP = 1,
    JMP = 2,
    JB = 3,
    JE = 4,
    ADD = 5,
    MUL = 6,
    DIV = 7,
    IN = 8,
    OUT = 9,
    SUB = 10,
    HLT = -1,
    STX_ERR = 666
};

#endif
