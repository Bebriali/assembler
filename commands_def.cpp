#include <stdio.h>
#include <string.h>

#include "commands_def.h"

int GetValue(char* command)
{
    int value = 0;
    for (int i = 0; command[i] != '\0'; i++)
    {
        value = value * 10 + (int)command[i] - (int)'0';
    }

    return value;
}

Commands CommandDefine(const char* command)
{
    if (strcmp(command, "PUSH") == 0)
    {
        return PUSH;
    }
    if (strcmp(command, "POP") == 0)
    {
        return POP;
    }
    if (strcmp(command, "JMP") == 0)
    {
        return JMP;
    }
    if (strcmp(command, "JE") == 0)
    {
        return JE;
    }
    if (strcmp(command, "JB") == 0)
    {
        return JB;
    }
    if (strcmp(command, "ADD") == 0)
    {
        return ADD;
    }
    if (strcmp(command, "MUL") == 0)
    {
        return MUL;
    }
    if (strcmp(command, "DIV") == 0)
    {
        return DIV;
    }
    if (strcmp(command, "OUT") == 0)
    {
        return OUT;
    }
    if (strcmp(command, "HLT") == 0)
    {
        return HLT;
    }

    return STX_ERR;
}

size_t CommandLength(Commands cmd)
{
    if (cmd == PUSH || cmd == JMP || cmd == JE || cmd == JB)
    {
        return 2;
    }

    return 1;
}

