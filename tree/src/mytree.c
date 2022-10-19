/*
*   Mytree.c
*   Create by Wangss on 2022-10-19 14:29
*   1) BP Tree
*   2) ...
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../inc/mytree.h"

/* Global Variable */
int tree_order = DEFAULT_ORDER;


/*  Base Function declarion.
*   For general use.
*/

void usage();
FILE * get_file();
int get_data_from_file(FILE *fp);


/* the input data
19 |
7 13 | 25 31 |
3 5 | 9 11 | 15 17 | 21 23 | 27 29 | 33 35 |
1 2 | 3 4 | 5 6 | 7 8 | 9 10 | 11 12 | 13 14 | 15 16 | 17 18 | 19 20 | 21 22 | 23 24 | 25 26 | 27 28 | 29 30 | 31 32 | 33 34 | 35 36 37 |
*/

/* Funcation Definition.
*/

void usage()
{
    printf("This program is testing the B+ Tree of Order %d.\n", tree_order);
    printf("The program will test creation, insertion, deletion and destory of BP tree.\n"
            "To start with inputing from one file of [Primary, Name] \n"
            "and will insert the creation date & time.\n" );
}

FILE * get_file()
{
    char * input_file_name = "../data/mytree.txt";
    FILE *fp;

    fp = fopen(input_file_name, "r");
    if ( fp== NULL)
    {
        perror("Failure to open input file.");
        exit(EXIT_FAILURE);
    }
    return fp;
}

/* 从文件里读取数据。数据格式
*   [25 20211425 LiTongHe]
*   int char[9]  char[20]   
*/
int get_data_from_file(FILE *fp)
{
    int input_key;
    char input_number[9], input_name[20];
    int i;

    fp = get_file();

    while (!feof(fp))
    {
        i = fscanf(fp, "%d %s %s\n", &input_key, input_number, input_name);
        if (i == 3)
            printf("[Key%d] [No:%s] [Name:%s] \n", input_key, input_number, input_name);
        else
        {
            fclose(fp);
            perror("Input data error.");
            exit(EXIT_FAILURE);
        }
    }
    
    fclose(fp);

    return 0;
}

int mybptree()
{
    FILE *fp;
    usage();

    get_data_from_file(fp);

    return EXIT_SUCCESS;
}


