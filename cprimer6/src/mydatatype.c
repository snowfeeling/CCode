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
    typeof(ch) nch;

    GET_TYPE_NAME(typeName, b);
    typeSize = GET_TYPE_SIZE(b);
    printf("Type: %s, Size: %zu byte(s)\n", typeName, typeSize);

    PRINT_TYPE_INFO(typeSize, "size_t");
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
    PRINT_TYPE_INFO(nch, "typeof() declare char");
}

int add(int a, int b)
{
    return a + b;
}
int subtract(int a, int b)
{
    return a - b;
}
int multiply(int a, int b)
{
    return a * b;
}
int divide(int a, int b)
{
    if (b == 0)
    {
        printf("Error: devision by ZERO!");
        return 0;
    }
    return a / b;
}
typedef int (*MathOperation)(int, int);

int testFuncPtr()
{

    MathOperation *mathOperation;
    mathOperation = (MathOperation *) malloc(4 * sizeof(MathOperation));

    mathOperation[0] = &add;
    mathOperation[1] = &subtract;
    mathOperation[2] = &multiply;
    mathOperation[3] = &divide;
    int a = 10;
    int b = 2;
    for (int i = 0; i < 4; i++)
    {
        int result = mathOperation[i](a, b);
        switch (i)
        {
        case 0:
        case 1:
        case 2:
        case 3:
            printf("%d %c %d = %d\n", a, "+-*/"[i], b, result);
            break;
        default:
            break;
        }
    }
    free(mathOperation);
}
#endif