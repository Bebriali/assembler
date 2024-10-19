#ifndef STRUCT_FILE
#define STRUCT_FILE

struct FileInf
{
    const char* name;
    FILE* stream;
    const char* typestream;
    size_t size;
};

#endif
