#ifndef MYDATATYPE_C_GUARD
#define MYDATATYPE_C_GUARD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include "../include/mylog.h"
#include "../include/mydatatype.h"

void showDataTypeSize(void)
{
    char typeName[10];
    size_t typeSize;

    char ch;
    short sh;
    int num;
    float f;
    unsigned ui;
    signed si;
    void *ptr;
    _Bool b;

    GET_TYPE_NAME(typeName, b);
    typeSize = GET_TYPE_SIZE(b);
    printf("Type: %s, Size: %zu byte(s)\n", typeName, typeSize);

    PRINT_TYPE_INFO(typeSize,"size_t");
    PRINT_TYPE_INFO(sh, "short");
    PRINT_TYPE_INFO(ch, "char");
    PRINT_TYPE_INFO(num, "int");
    PRINT_TYPE_INFO(f, "float");
    PRINT_TYPE_INFO(typeName, "char []");
    PRINT_TYPE_INFO(ui, "unsigned");
    PRINT_TYPE_INFO(si, "signed");
    PRINT_TYPE_INFO(ptr, "void *");
    PRINT_TYPE_INFO(b, "_Bool");
    PRINT_TYPE_INFO(sizeof(int), "sizeof(int)");
}

#endif