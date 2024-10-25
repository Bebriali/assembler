#include <stdio.h>

ErrorKeys ProcessCmd(CodeCell_t* code, size_t *ip)
{
    switch(code[*ip])
    {
        case [
