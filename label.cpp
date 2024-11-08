#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "label.h"
#include "color.h"
#include "commands.h"

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

ErrorKeys NewLabel(CodeCell_t* code, const char* command, LabelTable* table, size_t ip)
{
    if (table->current_label >= table->label_capacity)
    {
        printf("error in label pointer\n");
        return EX_ERRORS;
    }

    for (size_t i = 0; i < table->current_label; i++)
    {
        if (strcmp(command, table->label_name[i]) == 0)
        {
            if (table->label_place[i] != -1)
            {
                table->label_mark[i] = ip;
                code[table->label_place[i]] = table->label_mark[i];
                return NO_ERRORS;
            }
            else
            {
                printf(RED("redefinition of label\n"));
                return EX_ERRORS;
            }
        }
    }

    table->label_name[table->current_label] = command;
    table->label_mark[table->current_label] = ip;
    table->current_label++;

    return NO_ERRORS;
}

ErrorKeys MarkLabelRef(CodeCell_t* code, size_t* ip, char* command, LabelTable* table)
{
    for (size_t i = 0; i < table->label_capacity; i++)
    {
        if (strcmp(command, table->label_name[i]) == 0)
        {
            table->label_place[i] = ++(*ip);

            LabelDump(table);

            code[table->label_place[i]] = table->label_mark[i];
            return NO_ERRORS;
        }
    }

    for (size_t i = 0; i < table->label_capacity; i++)
    {
        if (table->label_name[i] == NULL)
        {
            table->label_name[i] = command;
            table->label_place[i] = *ip;
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

