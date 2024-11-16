#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"
#include "color.h"
#include "commands_def.h"
#include "debug_info.h"
#include "label.h"
//#include "commands.h"

int GetValue(char* command)
{
    int value = 0;
    int beg   = 0;
    int sign  = 1;
    if (command[0] == '-')
    {
        sign = -1;
        beg++;
    }

    for (int i = beg; command[i] != '\0'; i++)
    {
        value = value * 10 + (int)command[i] - (int)'0';
    }
    return sign * value;
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
    if (strcmp(command, "SUB")  == 0) return SUB;
    if (strcmp(command, "CALL") == 0) return CALL;
    if (strcmp(command, "ret")  == 0) return RET;
    if (strcmp(command, "SQR")  == 0) return SQR;
    if (strcmp(command, "JA")   == 0) return JA;
    if (strcmp(command, "HLT")  == 0) return HLT;

    return STX_ERR;
}

ErrorKeys HandleCommands(int* code, size_t* ip, char** buffer_cut, size_t* bp, RecStk* rec, LabelTable* table)
{
    Commands command = CommandDefine(buffer_cut[*bp]);

    switch (command)
    {
        case HLT:
            code[*ip] = HLT;
            break;

        case RET:
            code[(*ip)++] = JMP;
            code[(*ip)++] = 1;
            GetPtr(&rec->lab_stk, &code[(*ip)]);
            break;

        case CALL:
            JumpCallDefine(rec, table, buffer_cut, *ip, *bp);

            code[(*ip)++] = JMP;
            code[(*ip)] = NUM;

            MarkLabelRef(code, ip, buffer_cut[++(*bp)], table);
            break;

        case JMP:
        case JE:
        case JA:
        case JB:
            code[(*ip)++] = command;
            if (CheckLabel(buffer_cut[*bp + 1], table))
            {
                code[(*ip)] = NUM;
                MarkLabelRef(code, ip, buffer_cut[++(*bp)], table);
            }
            else
            {
                code[(*ip)] = NUM;
                *ip += 1;
                code[(*ip)] = GetValue(buffer_cut[++(*bp)]);
            }
            break;

        case PUSH:
            code[*ip] = CommandDefine(buffer_cut[(*bp)]);
            ON_DEBUG(CodeDump(code, *ip);)

            code[*ip + 1] = 0;
            break;

        case POP:
            code[*ip] = CommandDefine(buffer_cut[(*bp)]);
            ON_DEBUG(CodeDump(code, *ip);)

            code[*ip + 1] = 0;
            break;

        case ADD:
            code[*ip] = CommandDefine(buffer_cut[(*bp)]);
            ON_DEBUG(CodeDump(code, *ip);)

            code[*ip + 1] = 0;
            break;

        case MUL:
            code[*ip] = CommandDefine(buffer_cut[(*bp)]);
            ON_DEBUG(CodeDump(code, *ip);)

            code[*ip + 1] = 0;
            break;

        case DIV:
            code[*ip] = CommandDefine(buffer_cut[(*bp)]);
            ON_DEBUG(CodeDump(code, *ip);)

            code[*ip + 1] = 0;
            break;

        case GIN:
            code[*ip] = CommandDefine(buffer_cut[(*bp)]);
            ON_DEBUG(CodeDump(code, *ip);)

            code[*ip + 1] = 0;
            break;

        case UOUT:
            code[*ip] = CommandDefine(buffer_cut[(*bp)]);
            ON_DEBUG(CodeDump(code, *ip);)

            code[*ip + 1] = 0;
            break;

        case SUB:
            code[*ip] = CommandDefine(buffer_cut[(*bp)]);
            ON_DEBUG(CodeDump(code, *ip);)

            code[*ip + 1] = 0;
            break;

        case SQR:
            code[*ip] = CommandDefine(buffer_cut[(*bp)]);
            ON_DEBUG(CodeDump(code, *ip);)

            code[*ip + 1] = 0;
            break;

        case EMPTY:
            printf(RED("error: an empty command\n"));
            return EX_ERRORS;
            break;

        case STX_ERR:
            printf(RED("error: non-defined command type\n"));
            return EX_ERRORS;
            break;

        default:
            printf(RED("error: non-defined command type\n"));
            return EX_ERRORS;
            break;
    }

    *ip += 1;
    *bp += 1;

    return NO_ERRORS;
}

ErrorKeys JumpCallDefine(RecStk* rec, LabelTable* table, char** buffer_cut, size_t ip, size_t bp)
{
    assert(rec);
    assert(table);

    rec->name[rec->cur_call] = buffer_cut[bp + 1];
    rec->ret_place[rec->cur_call] = ip + 1;
    rec->cur_call++;

    ON_DEBUG(printf("cur_call = %d\n", rec->cur_call);)
    ON_DEBUG(getchar();)

    return NO_ERRORS;
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
    if (strcmp(command, "MLR") == 0)
    {
        return MLR;
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

int RetPtr(RecStk* rec, char* command)
{
    assert(rec != NULL);

    printf("command = %s\n", command);

    printf("cur_call = %d\n", rec->cur_call);

    for (size_t i = 0; i < rec->cur_call; i++)
    {
        printf("name[%d] = %s\n", i, rec->name[i]);
        if (strcmp(command, rec->name[i]) == 0)
        {
            printf("ret_place = %d\n", *rec->ret_place);
            return *rec->ret_place;
        }
    }

    return HLT;
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

char* NameLabel(int place, RecStk* rec)
{
    printf(RED("cur_call = %d\n"), rec->cur_call);
    printf(RED("name = %s\n"), rec->name[rec->cur_call - 1]);
    for (size_t i = 0; i < rec->cur_call; i++)
    {
        if (rec->ret_place[i] == place)
        {
            return rec->name[i];
        }
    }

    return "no such label";
}

size_t CommandLength(Commands cmd)
{
    if (cmd == PUSH || cmd == JMP || cmd == JE || cmd == JB)
    {
        return 2;
    }

    return 1;
}

bool JumpType(Commands command)
{
    switch (command)
    {
        case JMP:
            return true;
        case JE:
            return true;
        case JA:
            return true;
        case JB:
            return true;
        case CALL:
            return true;

        case PUSH:
        case POP:
        case ADD:
        case MUL:
        case DIV:
        case GIN:
        case UOUT:
        case SUB:
        case RET:
        case SQR:
        case EMPTY:
        case STX_ERR:
        case HLT:
        default:
            return false;
    }

    return false;
}
