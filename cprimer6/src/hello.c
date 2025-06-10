#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "../include/hello.h"
#include "../include/mystring.h"
#include "../include/mylog.h"
#include "../include/mydatatype.h"

void testProgressBar()
{
    for (int i = 0; i < 101; i++)
    {
        int n = i;
        n += 5;
        n /= 10;
        printf("%-12s %d%%", "**********" + 10 - n, i);
#ifdef _WIN32
        Sleep(100);
#else
        sleep(2);
#endif
        printf("\r");
    }
    printf("\n");
}

void testSomething()
{

    int a = 10;
    int b = 20;
    printf("a(10) +++ b(20) = %d\n", a++ + b);
    printf("a=%d, b=%d\n", a, b);
    printf("\"ASL\"[1] = %c\n", "ASL"[1]);
    printf("*\"ASL\" = %c\n", *"ASL");
    printf("*\"ASL\"+1 = %c\n", *"ASL" + 1);
    printf("*(\"ASL\"+1) = %c\n", *("ASL" + 1));
    printf("%c\n", "0123456789ABCDEF"[12 % 16]);

    printf("__STDC__         = %ld\n", __STDC__);
    printf("__STDC_VERSION__ = %ld\n", __STDC_VERSION__);
    printf("__STDC_HOSTED__  = %ld\n", __STDC_HOSTED__);
    printf("__CHAR_BIT__     = %ld\n", __CHAR_BIT__);
    //    printf("__STDC_NO_THREADS__     = %ld\n", __STDC_NO_THREADS__);
}
void hello()
{
    printf("Hello, World!\n");

    showDataTypeSize();
    testFuncPtr();
    testSomething();
    // testProgressBar();
    // stringDemo();
    testNodeList();
}
