#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "label.h"
#include "color.h"
#include "commands.h"
#include "change_stack.h"
#include "debug_info.h"

//int main()
//{
//    struct LabelTable table = {};
//
//    LabelCtor(&table);
//
//    LabelDump(&table);
//
//    return 0;
//}

ErrorKeys RecCtor(RecStk* rec)
{
    if (rec == NULL)
    {
        printf(RED("nullptr to rec_stk\n"));
        return EX_ERRORS;
    }
    rec->cur_call = 0;
    memset(rec->ret_place, -1, sizeof(int) * rec->depth);

    rec->lab_stk = {};
    StackInit(&rec->lab_stk, rec->depth);

    return NO_ERRORS;
}

ErrorKeys GetPtr(Stack* stk, CodeCell_t* cell_addr)
{
    StackCell_t a = 0;
    StackPop(stk, &a);

    *cell_addr = (int) a;

    return NO_ERRORS;
}



ErrorKeys LabelCtor(LabelTable* table)
{
    table->current_label = 0;

    memset(table->label_mark, -1, sizeof(LabelCell_t) * table->label_capacity);
    memset(table->label_place, -1, sizeof(int) * table->label_capacity);
    for (size_t i = 0; i < table->label_capacity; i++)
    {
        printf("%6d ", table->label_mark[i]);
    }
    printf("\n");

    for (size_t i = 0; i < table->label_capacity; i++)
    {
        printf("%6s ", table->label_name[i]);
    }
    printf("\n");

    for (size_t i = 0; i < table->label_capacity; i++)
    {
        printf("%6d ", table->label_place[i]);
    }
    printf("\n");

    return NO_ERRORS;
}
ErrorKeys LabelDtor(LabelTable* table)
{
    memset(table->label_mark, -1, sizeof(LabelCell_t) * table->label_capacity);
    table->current_label = 0;

    return NO_ERRORS;
}

bool CheckLabel(char* command, LabelTable* table)
{
    assert(command != NULL);
    assert(table != NULL);
    for (int i = 0; command[i] != '\0'; i++)
    {
        if (command[i] == ':' && command[i + 1] == '\0')
        {
            return true;
        }
    }

    return false;
}

int NewLabel(CodeCell_t* code, const char* command, LabelTable* table, size_t ip)
{
    printf(GREEN("making new label for %s\n"), command);
    if (table->current_label >= table->label_capacity)
    {
        printf("error in label pointer\n");
        return EX_ERRORS;
    }

    printf("current_label = %d\n", table->current_label);

    int rec_place = -1;

    for (size_t i = 0; i < table->current_label; i++)
    {
        if (table->label_name[i] == NULL)
        {
            break;
        }
        if (strcmp(command, table->label_name[i]) == 0)
        {
            if (table->label_place[i] != -1)
            {
                table->label_mark[i] = ip;
                code[table->label_place[i]] = table->label_mark[i];
                printf(RED("code[%d] = %d\n"), table->label_place[i], code[table->label_place[i]]);
                rec_place = table->label_place[i];
            }
            else
            {
                printf(RED("redefinition of label\n"));
                return HLT;
            }
        }
    }

    if (rec_place != -1)
    {
        return rec_place;
    }

    table->label_name[table->current_label] = command;
    table->label_mark[table->current_label] = ip;
    printf("no match for label %s\n", command);
    table->current_label++;

    return table->label_place[table->current_label];
}

ErrorKeys MarkLabelRef(CodeCell_t* code, size_t* ip, char* command, LabelTable* table)
{
    assert(code != NULL);
    assert(ip != NULL);
    assert(table != NULL);

    printf("%s\n", command);

    bool exist_label = false;
    bool new_lab_inited = false;
    int label_mark = 0;

    for (size_t i = 0; i < table->label_capacity; i++)
    {
        if (table->label_name[i] == NULL)
            break;

        if (strcmp(command, table->label_name[i]) == 0)
        {
            if (table->label_place[i] == -1)
            {
                table->label_place[i] = (*ip) + 1;
                new_lab_inited = true;
            }
            label_mark = table->label_mark[i];

            table->current_label++;

            printf(RED("label_place[%d] = %d\n"), i, table->label_place[i]);

            ON_DEBUG(LabelDump(table);)

            code[table->label_place[i]] = table->label_mark[i];


            exist_label = true;
        }
    }

    if (exist_label && new_lab_inited)
    {
       // (*ip) += 2;
        return NO_ERRORS;
    }

    printf(YELLOW("no common command\n"));

    for (size_t i = 0; i < table->label_capacity; i++)
    {
        if (table->label_name[i] == NULL)
        {
            table->label_name[i] = command;
            table->label_place[i] = *ip + 1;
            table->label_mark[i] = (label_mark != 0) ? label_mark : -1;

            code[table->label_place[i]] = table->label_mark[i];
            table->current_label++;

            ON_DEBUG(LabelDump(table);)

            //(*ip) += 2;

            return NO_ERRORS;
        }
    }
    return NO_ERRORS;
}


LabelCell_t DefLabel(const char* command, LabelTable* table)
{
    for (size_t i = 0; i < table->label_capacity; i++)
    {
        if (strcmp(command, table->label_name[i]) == 0)
        {
            printf("label used : %d\n", table->label_mark[i]);
            return table->label_mark[i];
        }
    }

    return NO_LABEL;
}

ErrorKeys LabelDump(LabelTable* table)
{
    printf("\n");
    printf("LABEL DUMP\n");
    for (size_t i = 0; i < table->label_capacity; i++)
    {
        printf("    0%X ", i);
    }
    printf("\n");

    for (size_t i = 0; i < table->label_capacity; i++)
    {
        printf("%6d ", table->label_mark[i]);
    }
    printf("\n");

    for (size_t i = 0; i < table->label_capacity; i++)
    {
        printf("%6s ", table->label_name[i]);
    }
    printf("\n");

    for (size_t i = 0; i < table->label_capacity; i++)
    {
        printf("%6d ", table->label_place[i]);
    }
    printf("\n");

    printf("[press any key to continue] ");
    getchar();

    return NO_ERRORS;
}

ErrorKeys RecDump(RecStk* rec)
{
    assert(rec != NULL);


    printf("rec dump\n");

    printf("\t");
    for (size_t i = 0; i < rec->depth; i++)
    {
        printf("    0%X ", i);
    }
    printf("\n\t");

    for (size_t i = 0; i < rec->depth; i++)
    {
        printf("%6d ", rec->ret_place[i]);
    }
    printf("\n\t");

    for (size_t i = 0; i < rec->depth; i++)
    {
        printf("%6s ", rec->name[i]);
    }
    printf("\n\n");

    printf("[press enter to continue]   ");
    getchar();

    return NO_ERRORS;
}
