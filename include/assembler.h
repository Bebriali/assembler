#ifndef ASSEMBLER
#define ASSEMBLER

#include "error_keys.h"
#include "struct_file.h"
#include "label.h"
#include "match_argscmd.h"

ErrorKeys Assemble(struct FileInf* human, struct FileInf* machine);


size_t GetBuffer(char* buffer, FileInf* file);
ErrorKeys CutBuffer(char** code, char* buffer, size_t* code_size, size_t buffer_size);

int* MakeCode(FileInf* file, char** buffer_cut, char* buffer, size_t* buffer_cut_size);
ErrorKeys HandleArgs(CodeCell_t* code, size_t* ip, char** buffer_cut, size_t* bp, \
                            LabelTable* table, RecStk* rec, size_t* buffer_cut_size);
ErrorKeys HandleArgs2(CodeCell_t* code, size_t* ip, char** buffer_cut, size_t* bp, LabelTable* table);
ErrorKeys CodeDump(CodeCell_t* code, size_t ip);
ErrorKeys PutArgs(char** buffer_cut, size_t* bp, CodeCell_t* code, size_t* ip, size_t* args_quantity, LabelTable* table);

bool CheckArgType(char* arg_char, TypeArgs arg_type_val);

ErrorKeys GetCmd(CodeCell_t* arg_value, char* arg_char, TypeArgs arg_type_val, LabelTable* table);

ErrorKeys WriteResult(FileInf* file, int* code, size_t* code_size);

#endif
