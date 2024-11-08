#include "TxLib.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <string.h>

#include "assembler.h"
#include "color.h"
#include "error_keys.h"
#include "struct_file.h"

const int CODE_SIZE = 20;

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        printf(RED("enter human code file and machine code file as arguments of the command line, please <3"));
        return 0;
    }

    FileInf human   = CreateStructFile(argv[1], "rb");
    FileInf machine = CreateStructFile(argv[2], "wb");

    char* buffer = (char*) calloc(human.size, sizeof(char));
    if (buffer == NULL)
    {
        printf(RED("error in buffer calloc\n"));
        return 0;
    }

    size_t code_size = GetBuffer(buffer, &human);
    if (code_size == 0)
    {
        printf(RED("error in getting buffer\n"));
        fclose(human.stream);
        fclose(machine.stream);
        free(buffer);
        return 0;
    }

    char** buffer_cut = (char**) calloc(code_size, sizeof(char*));
    if (buffer_cut == NULL)
    {
        printf(RED("error in buffer_cut calloc\n"));
        return 0;
    }

    CutBuffer(buffer_cut, buffer, code_size, human.size);

    int* code = MakeCode(&human, buffer_cut, buffer, &code_size);
    if (code == NULL)
    {
        printf("escaping...\n");
        free(buffer);
        for (size_t i = 0; i < code_size; i++)
        {
            free(buffer_cut[i]);
        }
        //free(buffer_cut);
        fclose(machine.stream);
        fclose(human.stream);
        return 0;
    }


    for (size_t i = 0; i < code_size; i++)
    {
        printf(CYAN("code[%d] = %d\n"), i, code[i]);
    }

    WriteResult(&machine, code, &code_size);

    free(code);
    free(buffer);
    free(buffer_cut);

    fclose(machine.stream);
    fclose(human.stream);

    return 0;
}
