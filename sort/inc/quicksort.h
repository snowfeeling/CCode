#ifndef QUCKSORT_H_
#define QUCKSORT_H_

#include <stdio.h>
#define PMAX 100

typedef struct squeue
{
    int value[PMAX];
    int qnum;
} Squeue;

int testQuickSort();

void QuickSort( Squeue * p, int low, int high);

#endif