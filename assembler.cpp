#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>

#include "assembler.h"
#include "change_stack.h"
#include "color.h"
#include "commands_def.h"
#include "debug_info.h"
#include "error_keys.h"
#include "label.h"
#include "match_argscmd.h"
#include "struct_file.h"


ErrorKeys Assemble(struct FileInf* human, struct FileInf* machine)
{
    assert(human);
    assert(machine);

    char* buffer = (char*) calloc(human->size, sizeof(char));
    if (buffer == NULL)
    {
        printf(RED("error in buffer calloc\n"));
        return EX_ERRORS;
    }

    size_t code_size = GetBuffer(buffer, human);
    if (code_size == 0)
    {
        printf(RED("error in getting buffer\n"));
        fclose(human->stream);
        fclose(machine->stream);
        free(buffer);
        return EX_ERRORS;
    }

    char** buffer_cut = (char**) calloc(code_size, sizeof(char*));
    if (buffer_cut == NULL)
    {
        printf(RED("error in buffer_cut calloc\n"));
        return EX_ERRORS;
    }

    CutBuffer(buffer_cut, buffer, &code_size, human->size);

    int* code = MakeCode(human, buffer_cut, buffer, &code_size);
    if (code == NULL)
    {
        printf("escaping...\n");
        free(buffer);
        free(buffer_cut);
        fclose(machine->stream);
        fclose(human->stream);
        return EX_ERRORS;
    }


    for (size_t i = 0; i < code_size; i++)
    {
        printf(CYAN("code[%d] = %d\n"), i, code[i]);
    }

    WriteResult(machine, code, &code_size);

    free(code);
    free(buffer);
    free(buffer_cut);

    fclose(machine->stream);
    fclose(human->stream);

    return NO_ERRORS;
}

size_t GetBuffer(char* buffer, FileInf* file)
{
    size_t file_size = fread(buffer, sizeof(char), file->size, file->stream);
    if (file_size != file->size)
    {
        return 0;
    }

    size_t code_size = 0;
    for (size_t i = 0; i < file_size; i++)
    {
        if (isspace(buffer[i]))
        {
            buffer[i] = '\0';
            if (i + 1 < file->size && !isspace(buffer[i + 1]))
                code_size++;
        }
    }

    code_size++;

    return code_size;
}

ErrorKeys CutBuffer(char** code, char* buffer, size_t* code_size, size_t buffer_size)
{
    if (code == NULL)
    {
        printf(RED("code addr got NULL value\n"));
        return EX_ERRORS;
    }
    if(buffer == NULL)
    {
        printf(RED("buffer addr got NULL value\n"));
        return EX_ERRORS;
    }

    size_t code_it = 0;
    code[code_it] = &buffer[0];
    printf(MAGENTA("%s\n"), code[code_it]);

    for (size_t buf_it = 0; buf_it < buffer_size; buf_it++)
    {
        if (code_it > *code_size)
        {
            printf(RED("out of code range\n"));
            return EX_ERRORS;
        }

        if (buffer[buf_it] == '\0' && buf_it + 1 < buffer_size && buffer[buf_it + 1] != '\0')
        {
            code[++code_it] = &buffer[buf_it + 1];

            if (code_it > 0 && code[code_it - 1][0] == '\0') // if prev text command is empty
            {
                code_it -= 1;
                code[code_it] = code[code_it + 1];
            }
        }
    }

    *code_size = code_it + 1;

    return NO_ERRORS;
}


int* MakeCode(FileInf* file, char** buffer_cut, char* buffer, size_t* buffer_cut_size)
{
    assert(file != NULL);
    assert(buffer_cut != NULL);
    assert(buffer != NULL);

    printf("buffer_cut_size = %d\n", *buffer_cut_size);
    for (size_t i = 0; i < *buffer_cut_size; i++)
    {
        printf(YELLOW("buffer_cut[%d] = '%s'\n"), i, buffer_cut[i]);
    }
    printf("\n");

    size_t code_size = 3 * (*buffer_cut_size);
    ON_DEBUG(printf("buffer_cut_size = %d\n", *buffer_cut_size);)
    ON_DEBUG(printf("code_size = %d\n", code_size);)

    int* code = (int*) calloc(code_size + 1, sizeof(int));
    if (code == NULL)
    {
        printf(RED("%s: In function '%s':\nline %d\nerror callocation\n"),
                                      __FILE__, __func__, __LINE__);
        return NULL;
    }

    struct LabelTable table = {};
    LabelCtor(&table);

    struct RecStk rec = {};
    ErrorKeys rec_ctor_status = RecCtor(&rec);
    if (rec_ctor_status != NO_ERRORS)
    {
        return NULL;
    }

    ON_DEBUG(RecDump(&rec);)

    size_t ip = 0;
    size_t bp = 0;

    while (bp < *buffer_cut_size)
    {
        printf(CYAN("buffer_cut[%d] = %s\n"), bp, buffer_cut[bp]);

        if (HandleArgs(code, &ip, buffer_cut, &bp, &table, &rec, buffer_cut_size)!= NO_ERRORS)
        {
            return NULL;
        }

        ON_DEBUG(CodeDump(code, ip);)
    }
    free(buffer_cut);
    printf("command line:\n");
    for (size_t i = 0; i < ip + 1; i++)
    {
        printf("%d ", code[i]);
    }
    printf("\n");

    *buffer_cut_size = ip + 1;
    return code;
}

ErrorKeys HandleArgs(CodeCell_t* code, size_t* ip, char** buffer_cut, size_t* bp, \
                            LabelTable* table, RecStk* rec, size_t* buffer_cut_size)
{

    size_t args_quantity = 0;
    size_t bp_fixed = *bp;

    while (*bp < *buffer_cut_size - 1 && CommandDefine(buffer_cut[++(*bp)]) == STX_ERR && !CheckLabel(buffer_cut[*bp], table))
        args_quantity++;

    *bp = bp_fixed;


    if (CheckLabel(buffer_cut[*bp], table))
    {
        StackPush(&rec->lab_stk, NewLabel(code, buffer_cut[*bp], table, *ip) + 1);
        *bp += 1;
        ON_DEBUG(LabelDump(table);)
        return NO_ERRORS;
    }

    if (HandleCommands(code, ip, buffer_cut, bp, rec, table) != NO_ERRORS)
    {
        return EX_ERRORS;
    }

    printf("args_quantity = %d\n", args_quantity);
    if (args_quantity == 0)
    {
        *ip += 1;
        return NO_ERRORS;
    }

    if (PutArgs(buffer_cut, bp, code, ip, &args_quantity, table) != NO_ERRORS)
    {
        return EX_ERRORS;
    }

    return NO_ERRORS;
}

ErrorKeys PutArgs(char** buffer_cut, size_t* bp, CodeCell_t* code, size_t* ip, size_t* args_quantity, LabelTable* table)
{
    for (size_t ap = 0; ap < *args_quantity; ap++)
    {
        printf("buffer_cut[%d] = %s\n", *bp + ap, buffer_cut[*bp + ap]);
        if (isdigit(buffer_cut[*bp + ap][0]) || (buffer_cut[*bp + ap][0] == '-' && isdigit(buffer_cut[*bp + ap][1])))
        {
            code[*ip] = code[*ip] | NUM;
            printf(GREEN("code[%d] = %d\n"), *ip + 1, code[*ip + 1]);
            code[*ip + ap + 1] = GetValue(buffer_cut[*bp + ap]);
        }
        CodeDump(code, *ip);
    }

    for (size_t ap = 0; ap < *args_quantity; ap++)
    {
        printf("buffer_cut[%d] = %s\n", *bp + ap, buffer_cut[*bp + ap]);
        if (isalpha(buffer_cut[*bp + ap][0]) && !CheckLabel(buffer_cut[*bp + ap], table))
        {
            code[*ip] = code[*ip] | LABEL;
            printf(GREEN("code[%d] = %d\n"), *ip + 1, code[*ip + 1]);
            code[*ip + ap + 1] = GetArg(buffer_cut[*bp + ap], table);
        }
        else if (CheckLabel(buffer_cut[*bp + ap], table))
        {
            code[*ip] = code[*ip] | 4;
            printf(GREEN("code[%d] = %d\n"),  *ip + 1, code[*ip + 1]);
            code[*ip + ap + 1] = GetArg(buffer_cut[*bp + ap], table);
        }
        CodeDump(code, *ip);
    }

    *bp += *args_quantity;
    *ip += *args_quantity + 1;

    return NO_ERRORS;
}

bool CheckArgType(char* arg_char, TypeArgs arg_type_val)
{
    switch(arg_type_val)
    {
        case REG:
            if (isalpha(arg_char[0]))
            {
                return true;
            }
            break;
        case NUM:
            if (isdigit(arg_char[0]) || (isdigit(arg_char[1]) && arg_char[0] == '-'))
            {
                return true;
            }
            break;
        case LABEL:
            for (size_t i = 0; arg_char[i] != '\0'; i++)
            {
                if (arg_char[i] == ':' && arg_char[i + 1] == '\0')
                {
                    return true;
                }
            }
            break;
        default:
            return false;
    }

    return false;
}

ErrorKeys GetCmd(CodeCell_t* arg_value, char* arg_char, TypeArgs arg_type_val, LabelTable* table)
{
    ErrorKeys status = NO_ERRORS;
    switch(arg_type_val)
    {
        case REG:
            printf("getting register\n");
            *arg_value = GetReg(arg_char);
            break;
        case NUM:
            printf("getting number\n");
            *arg_value = GetValue(arg_char);
            break;
        case LABEL:
            printf("getting label\n");
            *arg_value = GetLabel(arg_char, table);
            break;
        default:
            printf(RED("something is wrong with arg_type_val\n") "%s: In function '%s':\n\t""line %d\n", __FILE__, __func__, __LINE__);
            printf("\targ_type_val = %d\n", arg_type_val);
            status = EX_ERRORS;
            *arg_value = (CodeCell_t) HLT;
            break;
    }

    return status;
}

ErrorKeys CodeDump(CodeCell_t* code, size_t ip)
{
    FILE* code_file = fopen("code.txt", "a");

    fprintf(code_file, "ip = %d\n", ip);
    for (int i = 0; i < (int) ip + 1; i++)
    {
        fprintf(code_file, "%3d ", i);
    }
    fprintf(code_file, "\n");
    for (int i = 0; i < (int) ip + 1; i++)
    {
        fprintf(code_file, "%3d ", code[i]);
    }
    fprintf(code_file, "\n\n");

    fclose(code_file);

    //fprintf("[press enter to continue]  ");
    //getchar();

    return NO_ERRORS;
}

ErrorKeys WriteResult(FileInf* file, int* code, size_t* code_size)
{
    for (size_t i = 0; i < *code_size; i++)
    {
        printf("%d ", code[i]);
    }
    printf("\n\n");

    fwrite(code_size, sizeof(int), 1, file->stream);
    fwrite(code, sizeof(int), *code_size, file->stream);

    return NO_ERRORS;
}
