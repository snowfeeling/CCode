#pragma once

#ifndef MYSTRING_H
#define MYSTRING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 1024

typedef struct
{
    char *str;
    int len;
} str_t;
typedef void (*func_ptr)(char *);

void strHandleString(func_ptr fPtr, const char *str);
void strUpper(char *str);
void strLower(char *str);
void strReverse(char *str);
void strCapitalized(char *str);

void cleanInputBuffer();

void showManual(void);
void stringDemo(void);
void strHandle(int ch);
void strHandleOneLine(int ch, const char *str);

int strSplitTest(const char *str);
char **splitWords(const char *str, int *word_count);
void cleanWords(char **words, int word_count);

#endif