#ifndef CMD_DEF
#define CMD_DEF

#include "commands.h"
#include "label.h"
#include "match_argscmd.h"

Commands CommandDefine(char* command);
ErrorKeys HandleCommands(int* code, size_t* ip, char** buffer_cut, size_t* bp, RecStk* rec, LabelTable* table);

ErrorKeys JumpCallDefine(RecStk* rec, LabelTable* table, char** buffer_cut, size_t ip, size_t bp);

size_t CommandLength(Commands cmd); //unused

int GetValue(char* command);

int GetArg(char* command, LabelTable* table);

int RetPtr(RecStk* rec, char* command); // move to "label.h" + "label.cpp"
int GetReg(char* command);
int GetLabel(char* command, LabelTable* table);

char* NameLabel(int place, RecStk* rec); // move to "label.h" + "label.cpp"

Commands CommandDefine2(char* command, ArgsCmd* cmd_inf); // unused

bool JumpType(Commands command); // for backward compatibility

#endif
