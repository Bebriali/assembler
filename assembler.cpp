#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>

#include "assembler.h"
#include "color.h"
#include "commands_def.h"
#include "error_keys.h"

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


int* MakeCode(FileInf* file, char** buffer_cut, char* buffer, size_t buffer_cut_size)
{
    assert(file != NULL);
    assert(buffer_cut != NULL);
    assert(buffer != NULL);

    printf("buffer_cut_size = %d\n", buffer_cut_size);
    for (size_t i = 0; i < buffer_cut_size; i++)
    {
        printf(YELLOW("buffer_cut[%d] = '%s'\n"), i, buffer_cut[i]);
    }
    printf("\n");

    int* code = (int*) calloc(buffer_cut_size, sizeof(int)); // check

    size_t ip = 0;
    while (ip < buffer_cut_size)
    {
        code[ip] = CommandDefine(buffer_cut[ip]);
        printf("buffer_cut[%d] = %s\n", ip, buffer_cut[ip]);
        if (code[ip] == STX_ERR)
        {
            printf("syntax error:\n");
            printf("command: %s doesn't exist\n", buffer_cut[ip]);
            assert(0);
        }

        printf(CYAN("code[%d] = %d\n"), ip, code[ip]);

        if (CommandLength((Commands)code[ip]) == 2)
        {
            ip++;
            code[ip] = GetValue(buffer_cut[ip]);
        }

        ip++;
    }

    printf("command line:\n");
    for (int i = 0; i < 6; i++)
    {
        printf("%d ", code[i]);
    }
    printf("\n");

    return code;
}

ErrorKeys WriteResult(FileInf* file, int* code, size_t code_size)
{
    for (size_t i = 0; i < code_size; i++)
    {
        printf("%d ", code[i]);
    }
    printf("\n\n");
    fwrite(&code_size, sizeof(int), 1, file->stream);
    fwrite(code, sizeof(int), code_size, file->stream);

    return NO_ERRORS;
}
