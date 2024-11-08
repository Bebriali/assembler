#ifndef LABEL_H
#define LABEL_H

#include "error_keys.h"

enum Arguments
{
    AX  = 0,
    BX  = 2,
    CX  = 3,
    DX  = 4,
    MLR = 5
};

typedef int LabelCell_t;
typedef int CodeCell_t;

const int label_quantity = 11;
const int NO_LABEL = -1;

struct LabelTable
{
    int   label_mark[label_quantity];
    const char* label_name[label_quantity];
    int label_place[label_quantity];
    size_t current_label = 0;
    size_t label_capacity = label_quantity;
};

ErrorKeys LabelCtor(LabelTable* table);
ErrorKeys LabelDtor(LabelTable* table);

bool CheckLabel(char* command, LabelTable* table);
ErrorKeys NewLabel(CodeCell_t* code, const char* command, LabelTable* table, size_t ip);
ErrorKeys MarkLabelRef(CodeCell_t* code, size_t* ip, char* command, LabelTable* table);
LabelCell_t DefLabel(const char* command, LabelTable* table);

ErrorKeys LabelDump(LabelTable* table);

#endif
