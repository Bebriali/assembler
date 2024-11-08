#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "color.h"
#include "commands_def.h"
#include "debug_info.h"
#include "label.h"
//#include "commands.h"

int GetValue(char* command)
{
    int value = 0;
    for (int i = 0; command[i] != '\0'; i++)
    {
        value = value * 10 + (int)command[i] - (int)'0';
    }
    return value;
}

Commands CommandDefine(char* command)
{
    if (strcmp(command, "PUSH") == 0) return PUSH;
    if (strcmp(command, "POP")  == 0) return POP;
    if (strcmp(command, "JMP")  == 0) return JMP;
    if (strcmp(command, "JE")   == 0) return JE;
    if (strcmp(command, "JB")   == 0) return JB;
    if (strcmp(command, "ADD")  == 0) return ADD;
    if (strcmp(command, "MUL")  == 0) return MUL;
    if (strcmp(command, "DIV")  == 0) return DIV;
    if (strcmp(command, "IN")   == 0) return GIN;
    if (strcmp(command, "OUT")  == 0) return UOUT;
    if (strcmp(command, "HLT")  == 0) return HLT;

    return STX_ERR;
}


Commands CommandDefine2(char* command, ArgsCmd* cmd_inf)
{
    if (strcmp(command, "PUSH") == 0)
    {
        cmd_inf->cmd = PUSH;
        cmd_inf->args = (TypeArgs*) calloc(2, sizeof(TypeArgs));
        cmd_inf->args[0] = REG;
        cmd_inf->args[1] = NUM;
        cmd_inf->args_quantity = 2;

        //ON_DEBUG(for (size_t i = 0; i < cmd_inf->args_quantity; i++){ printf(MAGENTA("%d "), cmd_inf->args[i]);})

        return PUSH;
    }
    if (strcmp(command, "POP") == 0)
    {
        cmd_inf->cmd = POP;
        TypeArgs a[2] = {REG, NUM};
        cmd_inf->args = a;
        cmd_inf->args_quantity = 2;
        return POP;
    }
    if (strcmp(command, "JMP") == 0)
    {
        cmd_inf->cmd = JMP;
        TypeArgs a[2] = {NUM, LABEL};
        cmd_inf->args = a;
        cmd_inf->args_quantity = 2;
        return JMP;
    }
    if (strcmp(command, "JE") == 0)
    {
        cmd_inf->cmd = JE;
        TypeArgs a[2] = {NUM, LABEL};
        cmd_inf->args = a;
        cmd_inf->args_quantity = 2;
        return JE;
    }
    if (strcmp(command, "JB") == 0)
    {
        cmd_inf->cmd = JB;
        TypeArgs a[2] = {NUM, LABEL};
        cmd_inf->args = a;
        cmd_inf->args_quantity = 2;
        return JB;
    }
    if (strcmp(command, "ADD") == 0)
    {
        cmd_inf->cmd = ADD;
        TypeArgs a[1] = {NUM};
        cmd_inf->args = a;
        cmd_inf->args_quantity = 0;
        return ADD;
    }
    if (strcmp(command, "MUL") == 0)
    {
        cmd_inf->cmd = MUL;
        cmd_inf->args = {};
        cmd_inf->args_quantity = 0;
        return MUL;
    }
    if (strcmp(command, "DIV") == 0)
    {
        cmd_inf->cmd = DIV;
        cmd_inf->args = {};
        cmd_inf->args_quantity = 0;
        return DIV;
    }
    if (strcmp(command, "IN") == 0)
    {
        cmd_inf->cmd = GIN;
        cmd_inf->args = {};
        cmd_inf->args_quantity = 0;
        return GIN;
    }
    if (strcmp(command, "OUT") == 0)
    {
        cmd_inf->cmd = UOUT;
        cmd_inf->args = {};
        cmd_inf->args_quantity = 0;
        return UOUT;
    }
    if (strcmp(command, "HLT") == 0)
    {
        cmd_inf->cmd = HLT;
        cmd_inf->args = {};
        cmd_inf->args_quantity = 0;
        return HLT;
    }

    return STX_ERR;
}

int GetArg(char* command, LabelTable* table)
{

    if (strcmp(command, "AX") == 0)
    {
        return AX;
    }
    if (strcmp(command, "BX") == 0)
    {
        return BX;
    }
    if (strcmp(command, "CX") == 0)
    {
        return CX;
    }
    if (strcmp(command, "DX") == 0)
    {
        return DX;
    }

    for (size_t i = 0; command[i] != '\0'; i++)
    {
        if (command[i] == ':')
        {
            return (int) DefLabel(command, table);
        }
    }

    return STX_ERR;
}


int GetReg(char* command)
{
    if (strcmp(command, "AX") == 0)
    {
        return AX;
    }
    if (strcmp(command, "BX") == 0)
    {
        return BX;
    }
    if (strcmp(command, "CX") == 0)
    {
        return CX;
    }
    if (strcmp(command, "DX") == 0)
    {
        return DX;
    }

    return STX_ERR;
}

int GetLabel(char* command, LabelTable* table)
{
    for (size_t i = 0; command[i] != '\0'; i++)
    {
        if (command[i] == ':')
        {
            return (int) DefLabel(command, table);
        }
    }
    return -1;
}


size_t CommandLength(Commands cmd)
{
    if (cmd == PUSH || cmd == JMP || cmd == JE || cmd == JB)
    {
        return 2;
    }

    return 1;
}

