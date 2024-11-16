#include <stdio.h>
#include <sys/stat.h>

#include "color.h"
#include "struct_file.h"

#define INIT_FILE(arg1, arg2, arg3)                                                 \
                        {                                                           \
                            if(int a = InitCodeFile(arg1, arg2, arg3) != NO_ERRORS) \
                            {                                                       \
                                printf(RED("error in file \nerror_code: %d\n"), a); \
                            }                                                       \
                        }

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

ErrorKeys DtorCodeFile(struct FileInf* file)
{
    fclose(file->stream);
    file->size = 0;
    file->name = "";

    return NO_ERRORS;
}
