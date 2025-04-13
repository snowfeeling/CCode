#pragma once

#ifndef MYDATATYPE_H
#define MYDATATYPE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 定义类型名称映射的宏
#define TYPE_NAME(X) _Generic((X),            \
    int: "int",                               \
    char: "char",                             \
    float: "float",                           \
    double: "double",                         \
    short: "short",                           \
    long: "long",                             \
    long long: "long long",                   \
    unsigned int: "unsigned int",             \
    unsigned char: "unsigned char",           \
    unsigned short: "unsigned short",         \
    unsigned long: "unsigned long",           \
    unsigned long long: "unsigned long long", \
    void *: "void*",                          \
    char *: "char*",                          \
    int *: "int*",                            \
    float *: "float*",                        \
    double *: "double*",                      \
    int **: "int**",                          \
    _Bool: "_Bool",                           \
    default: "unknown")

// 定义打印类型信息的宏
#define PRINT_TYPE_INFO(X, SNAME) printf("Delared: %-20s =  %-20s, Size: %zu byte(s)\n", SNAME, TYPE_NAME(X), sizeof(X))
// X 是一个变量，TYPE_NAME(X) 是一个宏，用于获取变量 X 的类型名称。
#define GET_TYPE_NAME(str, X) sprintf(str, "%s", TYPE_NAME(X))
// X 是一个变量，sizeof(X) 是一个运算符，用于获取变量 X 的大小（以字节为单位）。
#define GET_TYPE_SIZE(X) sizeof(X)

#endif