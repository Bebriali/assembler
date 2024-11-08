#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>

#include "assembler.h"
#include "color.h"
#include "commands_def.h"
#include "debug_info.h"
#include "error_keys.h"
#include "label.h"
#include "match_argscmd.h"

FileInf CreateStructFile(const char* filename, const char* type)
{
    struct FileInf file = {.name = filename, .stream = NULL, .typestream = type, .size = 0};
    INIT_FILE(&file, file.name, file.typestream);

    return file;
}

ErrorKeys InitCodeFile(struct FileInf* file, const char* filename, const char* typestream)
{
    *file = {.name = filename,  .stream = NULL, .typestream = typestream, .size = 0};
    file->stream = fopen(file->name, file->typestream);
    struct stat st = {};
    stat(file->name, &st);
    file->size = st.st_size;
    if (file->stream == NULL)
    {
        return FILE_ERROR;
    }

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

    for (size_t i = 0; i < file->size; i++)
    {
        printf(GREEN("buffer[%d] = %d\n"), i, buffer[i]);
    }

    return code_size;
}

ErrorKeys CutBuffer(char** code, char* buffer, size_t code_size, size_t buffer_size)
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
        if (code_it > code_size)
        {
            printf(RED("out of code range\n"));
            return EX_ERRORS;
        }
        if (buffer[buf_it] == '\0' && buf_it + 1 < buffer_size && buffer[buf_it + 1] != '\0')
        {
            code[++code_it] = &buffer[buf_it + 1];
        }
    }

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
    printf("buffer_cut_size = %d\n", *buffer_cut_size);
    printf("code_size = %d\n", code_size);

    int* code = (int*) calloc(code_size + 1, sizeof(int));
    if (code == NULL)
    {
        printf("code addr =  %p\n", code);
        printf(RED("%s: In function '%s':\nline %d\nerror callocation\n"),
                                      __FILE__, __func__, __LINE__);
        return NULL;
    }

    struct LabelTable table = {};
    LabelCtor(&table);

    size_t ip = 0;
    size_t bp = 0;

    while (bp < *buffer_cut_size)
    {
        printf(CYAN("buffer_cut[%d] = %s\n"), bp, buffer_cut[bp]);

        HandleArgs(code, &ip, buffer_cut, &bp, &table);

        CodeDump(code, ip);
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

ErrorKeys HandleArgs(CodeCell_t* code, size_t* ip, char** buffer_cut, size_t* bp, LabelTable* table)
{
    size_t args_quantity = 0;
    size_t bp_fixed = *bp;
    size_t ip_fixed = *ip;

    if (CheckLabel(buffer_cut[*bp], table))
    {
        NewLabel(code, buffer_cut[*bp], table, *ip);
        *bp += 1;
        LabelDump(table);
        return NO_ERRORS;
    }

    if (CommandDefine(buffer_cut[*bp]) == HLT)
    {
        code[*ip] = HLT;
        *bp += 1;
        //*ip += 1;
        return NO_ERRORS;
    }

    while (CommandDefine(buffer_cut[++(*bp)]) == STX_ERR && !CheckLabel(buffer_cut[*bp], table))
        args_quantity++;

    *bp = bp_fixed;
    if (CommandDefine(buffer_cut[*bp]) == JMP && CheckLabel(buffer_cut[*bp + 1], table))
    {
        printf("call MarkLabelRef\n");
        getchar();
        code[(*ip)] = CommandDefine(buffer_cut[(*bp)++]);
        code[++(*ip)] = 1;
        MarkLabelRef(code, ip, buffer_cut[(*bp)], table);
        *bp += 1;
        *ip += 1;
        return NO_ERRORS;
        args_quantity = 1;
    }
    printf(RED("buffer_cut[%d] = %s\n"), *bp, buffer_cut[*bp]);
    code[*ip] = CommandDefine(buffer_cut[(*bp)++]);
    CodeDump(code, *ip);

    code[++(*ip)] = 0;
    for (size_t i = 0; i < *ip + 1; i++)
    {
        printf(YELLOW("%d "), code[i]);
    }
    printf("\n");

    printf("args_quantity = %d\n", args_quantity);
    if (args_quantity == 0)
    {
        *ip += 1;
        printf("no args\n");
        return NO_ERRORS;
    }

    for (size_t ap = 0; ap < args_quantity; ap++)
    {
        printf("buffer_cut[%d] = %s\n", *bp + ap, buffer_cut[*bp + ap]);
        if (isdigit(buffer_cut[*bp + ap][0]))
        {
            code[ip_fixed + 1] = code[ip_fixed + 1] | 1;
            printf(GREEN("code[%d] = %d\n"), ip_fixed + 1, code[ip_fixed + 1]);
            code[*ip + ap + 1] = GetValue(buffer_cut[*bp + ap]);
        }
        CodeDump(code, *ip);
    }

    for (size_t ap = 0; ap < args_quantity; ap++)
    {
        printf("buffer_cut[%d] = %s\n", *bp + ap, buffer_cut[*bp + ap]);
        if (isalpha(buffer_cut[*bp + ap][0]) && !CheckLabel(buffer_cut[*bp + ap], table))
        {
            code[ip_fixed + 1] = code[ip_fixed + 1] | 2;
            printf(GREEN("code[%d] = %d\n"), ip_fixed + 1, code[ip_fixed + 1]);
            code[*ip + ap + 1] = GetArg(buffer_cut[*bp + ap], table);
        }
        else if (CheckLabel(buffer_cut[*bp + ap], table))
        {
            code[ip_fixed + 1] = code[ip_fixed + 1] | 4;
            printf(GREEN("code[%d] = %d\n"),  ip_fixed + 1, code[ip_fixed + 1]);
            code[*ip + ap + 1] = GetArg(buffer_cut[*bp + ap], table);
        }
        CodeDump(code, *ip);
    }

    *bp += args_quantity;
    *ip += args_quantity + 1;

    return NO_ERRORS;
}

ErrorKeys HandleArgs2(CodeCell_t* code, size_t* ip, char** buffer_cut, size_t* bp, LabelTable* table)
{
    if (CheckLabel(buffer_cut[*bp], table))
    {
        NewLabel(code, buffer_cut[*bp], table, *ip);
        *bp += 1;
        LabelDump(table);
        return NO_ERRORS;
    }

    ArgsCmd cmd_inf = {};
    if (CommandDefine2(buffer_cut[*bp], &cmd_inf) == STX_ERR)
    {
        printf(RED("error in defining command\n\t") "%s: In function '%s':\n\t" "line %d\n\t" "command: %s\n", __FILE__, __func__, __LINE__, buffer_cut[*bp]);
    }

    code[(*ip)++] = cmd_inf.cmd;
    if (cmd_inf.args_quantity == 0)
    {
        *bp += 1;
        return NO_ERRORS;
    }

    PutArgs(cmd_inf, buffer_cut, bp, code, ip, table);

    return NO_ERRORS;
}

ErrorKeys PutArgs(ArgsCmd cmd_inf, char** buffer_cut, size_t* bp, CodeCell_t* code, size_t* ip, LabelTable* table)
{
    ErrorKeys getcmd_status = NO_ERRORS;
    while (CommandDefine2(buffer_cut[++(*bp)], &cmd_inf) == STX_ERR)
    {
        ON_DEBUG(for (size_t i = 0; i < cmd_inf.args_quantity; i++){ printf(MAGENTA("%d "), cmd_inf.args[i]);})
        printf("\n");

        for (size_t i = 0; i < cmd_inf.args_quantity; i++)
        {
            if (!CheckArgType(buffer_cut[(*bp)], cmd_inf.args[i]) && !CheckLabel(buffer_cut[(*bp)], table))
            {
                printf("buffer_cut[%d] = %s", *bp, buffer_cut[(*bp)]);
                printf(RED(" IS NOT "));
                printf( "cmd_inf.args[%d] = %d\n", i, cmd_inf.args[i]);
                continue;
            }

            printf("buffer_cut[%d] = %s", *bp, buffer_cut[(*bp)]);
            printf( GREEN(" IS "));
            printf( "cmd_inf.args[%d] = %d\n", i, cmd_inf.args[i]);

            getcmd_status = GetCmd(&code[(*ip)++], buffer_cut[(*bp)++], cmd_inf.args[i], table);
            break;
        }
        printf(GREEN("current bp = %d\n"), *bp);
        if (CommandDefine2(buffer_cut[*bp], &cmd_inf) != STX_ERR || CheckLabel(buffer_cut[*bp], table))
            break;
    }

    return getcmd_status;
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
            if (isdigit(arg_char[0]))
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
    printf("ip = %d\n", ip);
    for (int i = 0; i < (int) ip + 1; i++)
    {
        printf(MAGENTA("%2d "), i);
    }
    printf("\n");
    for (int i = 0; i < (int) ip + 1; i++)
    {
        printf(MAGENTA("%2d "), code[i]);
    }
    printf("\n\n");

    printf("[press enter to continue]  ");
    getchar();

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
