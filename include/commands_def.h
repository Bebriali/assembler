#ifndef CMD_DEF
#define CMD_DEF

#include "commands.h"
#include "label.h"
#include "match_argscmd.h"

Commands CommandDefine(char* command);
size_t CommandLength(Commands cmd);
int GetValue(char* command);
int GetArg(char* command, LabelTable* table);
int GetReg(char* command);
int GetLabel(char* command, LabelTable* table);

Commands CommandDefine2(char* command, ArgsCmd* cmd_inf);


#endif
