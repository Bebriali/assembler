#ifndef LABEL_H
#define LABEL_H

#include "error_keys.h"
#include "stack.h"

enum Arguments
{
    AX  = 0,
    BX  = 1,
    CX  = 2,
    DX  = 3,
    MLR = 4
};

typedef int LabelCell_t;
typedef int CodeCell_t;

const int label_quantity =  30;
const int rec_depth      =  11;
const int NO_LABEL       = -1;

struct LabelTable
{
    int   label_mark[label_quantity];
    const char* label_name[label_quantity];
    int label_place[label_quantity];
    size_t current_label = 0;
    size_t label_capacity = label_quantity;
};

struct RecStk
{
    char* name[rec_depth];
    int ret_place[rec_depth];
    size_t cur_call = 0;
    size_t depth = rec_depth;
    Stack lab_stk = {};
};

ErrorKeys RecCtor(RecStk* rec);
ErrorKeys GetPtr(Stack* stk, CodeCell_t* cell_addr);

ErrorKeys LabelCtor(LabelTable* table);
ErrorKeys LabelDtor(LabelTable* table);

bool CheckLabel(char* command, LabelTable* table);
int NewLabel(CodeCell_t* code, const char* command, LabelTable* table, size_t ip);
ErrorKeys MarkLabelRef(CodeCell_t* code, size_t* ip, char* command, LabelTable* table);
LabelCell_t DefLabel(const char* command, LabelTable* table);

ErrorKeys LabelDump(LabelTable* table);
ErrorKeys RecDump(RecStk* rec);

#endif
