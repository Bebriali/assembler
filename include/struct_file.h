#ifndef STRUCT_FILE_H
#define STRUCT_FILE_H

#include "error_keys.h"

struct FileInf
{
    const char* name;
    FILE* stream;
    const char* typestream;
    size_t size;
};

FileInf CreateStructFile(const char* filename, const char* typestream);
ErrorKeys InitCodeFile(struct FileInf* file, const char* filename, const char* typestream);
ErrorKeys DtorCodeFile(struct FileInf* file);


#endif
