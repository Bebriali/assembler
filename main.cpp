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

    ErrorKeys asm_status = Assemble(&human, &machine);

    DtorCodeFile(&human);
    DtorCodeFile(&machine);

    return asm_status;
}
