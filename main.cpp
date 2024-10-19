#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>

#include "assembler.h"
#include "color.h"
#include "error_keys.h"
#include "struct_file.h"


#define INIT_FILE(arg) {                                                           \
                                if(int a = InitCodeFile(arg) != NO_ERRORS) \
                                {                                                       \
                                    printf(RED("error in file \nerror_code: %d\n"), a); \
                                }                                                       \
                            }


const int CODE_SIZE = 20;

int main(int argc, char* argv[])
{
    printf("argc = %d\n", argc);

    const char* human_code = argv[1];
    printf("human_code = %s\n", human_code);
    const char* machine_code = argv[2];
    printf("machine_code = %s\n", machine_code);

    FileInf human   = {human_code,  NULL, "rb", 0};
    INIT_FILE(&human);

    FileInf machine = {machine_code, NULL, "wb", 0};
    INIT_FILE(&machine);

    char* buffer = (char*) calloc(human.size, sizeof(char));
    size_t code_size = GetBuffer(buffer, &human);
    if (code_size == 0)
    {
        printf(RED("error in getting buffer\n"));
        fclose(human.stream);
        fclose(machine.stream);
        free(buffer);
        return 0;
    }

    char** buffer_cut = (char**) calloc(code_size, sizeof(char));
    CutBuffer(buffer_cut, buffer, code_size, human.size);

    int* code = MakeCode(&human, buffer_cut, buffer, code_size);
    for (size_t i = 0; i < code_size; i++)
    {
        printf(CYAN("code[%d] = %d\n"), i, code[i]);
    }

    WriteResult(&machine, code, code_size);

    fclose(machine.stream);
    fclose(human.stream);

    return 0;
}
