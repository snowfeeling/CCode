/*
 *
 *  bpt:  B+ Tree Implementation
 *  My B-PLus Tree.c
 *  Created by wangss on 2022.10.04
 *  Last modified: 
 *
 *  This implementation demonstrates the B+ tree data structure
 *  for educational purposes, includin insertion, deletion, search, and display
 *  of the search path, the leaves, or the whole tree.
 *  
 *  Must be compiled with a C99-compliant C compiler such as the latest GCC.
 *
 *  Usage:  bpt [order]
 *  where order is an optional argument
 *  (integer MIN_ORDER <= order <= MAX_ORDER)
 *  defined as the maximal number of pointers in any node.
 *
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../inc/bptree.h"

// GLOBALS.

/* The order
 * Order阶数决定最大和最小的Key/Pointer数量，在每个节点林，最大有（阶数-1）个Key，至少有(阶数-1)/2个key。
 * 叶子节点中，数据Pointer数目=Key数目
 */
int order = DEFAULT_ORDER;
/* The queue 
 * 用于打印tree，按level顺序，从root开始，每层一行，到叶子结束
 */
Node *queue = NULL;

/* 
 * 决定是否使用16进制打印
 */
bool verbose_output = false;

int testBPTree()
{
    return 0;

}


