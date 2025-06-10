#pragma once

#ifndef MYDATATYPE_H
#define MYDATATYPE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "../include/tinycthread.h"
//#include <threads.h>


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

int testFuncPtr();

// Define the structure for the data node
typedef struct DataNode
{
    int index;
    char name[256]; // Adjust size as necessary
    double value;   // Assuming the value is numeric
    struct DataNode *prev;
    struct DataNode *next;
} DataNode;
// 数据链表定义
typedef struct
{
    DataNode *head;
    DataNode *tail;
    int size;
} DataList;
// 查询返回结果的节点集合定义
typedef struct
{
    DataNode **nodes;
    int count;
} DataNodeMatchResult;
// 函数指针类型定义
typedef bool (*CompareFunc)(DataNode *node, void *target);
// 条件节点定义
typedef struct CriteriaNode
{
    DataNode *head;
    void *target;
    CompareFunc compareFunc;
} CriteriaNode;

// 动态缓冲区结构
typedef struct {

    int count;
    mtx_t mutex;
    cnd_t not_empty;
    cnd_t not_full;
} ThreadNode;

DataNodeMatchResult findNodesByValue(DataNode *head, double targetValue);
void freeNodeMatchResult(DataNodeMatchResult *result);

// 比较函数原型声明
bool compareIndex(DataNode *node, void *target);
bool compareValue(DataNode *node, void *target);
bool compareName(DataNode *node, void *target);

// 统一查询接口
DataNodeMatchResult findNodesByCriteria(CriteriaNode *criteriaNode);
DataNode *findFirstNodeByCriteria(CriteriaNode *criteriaNode);

DataNode *createNode(int index, const char *name, double value);
void appendNode(DataList *list, int index, const char *name, double value);
DataList createNodeListFromFile();
void freeList(DataList *list);

// 测试函数
int testNodeList();
#endif