#ifndef CMD_DEF
#define CMD_DEF

#include "commands.h"

Commands CommandDefine(const char* command);
size_t CommandLength(Commands cmd);
int GetValue(char* command);

#endif
