#include <stdio.h>
#include "../include/hello.h"

void hello()
{
    printf("Hello, World!\n");

    showDataTypeSize();

    printf("%ld\n",__STDC_VERSION__);
    printf("%ld\n",__STDC_HOSTED__);
    printf("%ld\n", __CHAR_BIT__);
    
    int i = 2;
    printf("\n%d  %d %d\n", i, i++ + i, i++);
    printf("%d\n", i);
}

void showDataTypeSize(void)
{
    char typeName[10];
    size_t typeSize;

    char ch;
    int num;
    float f;
    unsigned ui;
    signed si;
    void *ptr;
    _Bool b;
    
    GET_TYPE_NAME(typeName, b);
    typeSize = GET_TYPE_SIZE(b);
    printf("Type: %s, Size: %zu byte(s)\n", typeName, typeSize);

    PRINT_TYPE_INFO(typeSize);
    PRINT_TYPE_INFO(num);
    PRINT_TYPE_INFO(ch);
    PRINT_TYPE_INFO(f);
    PRINT_TYPE_INFO(typeName);
    PRINT_TYPE_INFO(ui);
    PRINT_TYPE_INFO(si);
    PRINT_TYPE_INFO(ptr);
    PRINT_TYPE_INFO(b);
    PRINT_TYPE_INFO(sizeof(int));
}