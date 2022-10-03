#include <stdio.h>
#include "../inc/sayhello.h"
#include "../inc/quicksort.h"


int testPerror();

int SayHello()
{
    printf("Hello world!\n");
    //testPerror();
    return 0;
}


int testPerror()
{
    FILE *fp;
    fp = fopen("../log/err.log","r");
    if (fp == NULL)
    perror("err.log");
    return EXIT_FAILURE;
}