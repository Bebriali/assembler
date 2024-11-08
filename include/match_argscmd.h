#ifndef MATCH_ARGSCMD_H
#define MATCH_ARGSCMD_H

#include "commands.h"

struct ArgsCmd
{
    Commands cmd;
    TypeArgs* args;
    size_t args_quantity;
};

#endif
