#ifndef ASSEMBLER
#define ASSEMBLER

#include "error_keys.h"
#include "struct_file.h"
#include "label.h"
#include "match_argscmd.h"

#define INIT_FILE(arg1, arg2, arg3)                                                 \
                        {                                                           \
                            if(int a = InitCodeFile(arg1, arg2, arg3) != NO_ERRORS) \
                            {                                                       \
                                printf(RED("error in file \nerror_code: %d\n"), a); \
                            }                                                       \
                        }

FileInf CreateStructFile(const char* filename, const char* typestream);
ErrorKeys InitCodeFile(struct FileInf* file, const char* filename, const char* typestream);

size_t GetBuffer(char* buffer, FileInf* file);
ErrorKeys CutBuffer(char** code, char* buffer, size_t code_size, size_t buffer_size);

int* MakeCode(FileInf* file, char** buffer_cut, char* buffer, size_t* buffer_cut_size);
ErrorKeys HandleArgs(CodeCell_t* code, size_t* ip, char** buffer_cut, size_t* bp, LabelTable* table);
ErrorKeys HandleArgs2(CodeCell_t* code, size_t* ip, char** buffer_cut, size_t* bp, LabelTable* table);
ErrorKeys CodeDump(CodeCell_t* code, size_t ip);
ErrorKeys PutArgs(ArgsCmd cmd_inf, char** buffer_cut, size_t* bp, CodeCell_t* code, size_t* ip, LabelTable* table);

bool CheckArgType(char* arg_char, TypeArgs arg_type_val);

ErrorKeys GetCmd(CodeCell_t* arg_value, char* arg_char, TypeArgs arg_type_val, LabelTable* table);

ErrorKeys WriteResult(FileInf* file, int* code, size_t* code_size);

#endif
