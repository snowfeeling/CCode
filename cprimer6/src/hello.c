#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/hello.h"
#include "../include/mystring.h"
#include "../include/mylog.h"
#include "../include/mydatatype.h"

void hello()
{
    printf("Hello, World!\n");

    showDataTypeSize();
    testFuncPtr();

    {
        int a = 10;
        int b = 20;
        printf("%d\n", a +++ b);
    }
    printf("%ld\n", __STDC__);
    printf("%ld\n", __STDC_VERSION__);
    printf("%ld\n", __STDC_HOSTED__);
    printf("%ld\n", __CHAR_BIT__);

    // stringDemo(); 
    testNodeList();
}


