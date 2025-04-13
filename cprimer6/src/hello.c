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

    printf("%ld\n", __STDC_VERSION__);
    printf("%ld\n", __STDC_HOSTED__);
    printf("%ld\n", __CHAR_BIT__);

    stringDemo();
}


