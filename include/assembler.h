#ifndef ASSEMBLER
#define ASSEMBLER

#include "error_keys.h"
#include "struct_file.h"

ErrorKeys InitCodeFile(struct FileInf* file);
size_t GetBuffer(char* buffer, FileInf* file);
ErrorKeys CutBuffer(char** code, char* buffer, size_t code_size, size_t buffer_size);
int* MakeCode(FileInf* file, char** buffer_cut, char* buffer, size_t buffer_cut_size);
ErrorKeys WriteResult(FileInf* file, int* code, size_t code_size);

#endif
