
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "../inc/mylog.h"
#include "../inc/quicksort.h"

FILE *flog;
#define SQFNAME "../log/quicklog.txt"

bool InitializeSource(Squeue *p);
int ShowData(const Squeue *p);
int LogMeg(char *str);
int LogQueueData(Squeue *p);

int testQuickSort()
{
    Squeue p;
    if (!InitLogfile(&flog, SQFNAME))
        return EXIT_FAILURE;

    if (!InitializeSource(&p))
        return EXIT_FAILURE;

    printf("The original data is :\n");
    ShowData(&p);

    QuickSort(&p, 0, p.qnum - 1);

    printf("The sorted data is :\n");
    ShowData(&p);

    CloseLogfile(flog);
    return 0;
}

bool InitializeSource(Squeue *p)
{
    FILE *fp;
    char *fname = "../data/testqs.txt";
    int tmp, i = 0;
    if (!(fp = fopen(fname, "r")))
    {
        printf("Can't open file %s. Bye.\n", fname);
        return false;
    }
    while (i < PMAX /* condition */)
    {
        int k = fscanf(fp, "%d", &tmp);
        if (k == 1)
        {
            p->value[i] = tmp;
            i++;
        }
        else if (k == EOF)
            break;
        else if (k != 1) // 如果读的有错（非数字），则 K==0
        {
            printf("Error data found.\n");
            fclose(fp);
            return false;
        }
    }
    fclose(fp);
    p->qnum = i; // 计数器

    return true;
}
int ShowData(const Squeue *p)
{
    for (int i = 0; i < p->qnum; i++)
    {
        printf("[%2d]%d\n", i, p->value[i]);
    }
    return 0;
}

/*
*  Quick Sort
*/
void QuickSort(Squeue *p, int low, int high)
{
    if (low >= high)
        return;
    int tmp, i, j;
    char str[30];
    i = low;
    j = high;
    tmp = p->value[i]; // 取第一个值，为比较值
    sprintf(str, "[Begin:P=%6d %3d -%3d]", tmp, i, j);
    LogMeg(str);
    LogQueueData(p);
    do
    {
        while (p->value[j] > tmp && i < j) 
            j--;  // 如果从右的数字大于比较值，就向左走，直到等于或者小于比较值。
        if (i < j)
        {
            p->value[i] = p->value[j]; // 把右边的较小值，放到左边
            sprintf(str, "[R->L :P=%6d %3d -%3d]", tmp, j, i);
            LogMeg(str);
            LogQueueData(p);
            i++;  //往右走一个
        }
        while (p->value[i] < tmp && i < j) 
            i++;   // 如果从左的数字小于比较值，就向右走，直到等于或者大于比较值。
        if (i < j)
        {
            p->value[j] = p->value[i]; //把左边较大的值，放到右边
            // 记录
            sprintf(str, "[L->R :P=%6d %3d -%3d]", tmp, i, j);
            LogMeg(str);
            LogQueueData(p);
            j--; // 往左走一步
        }
    } while (i != j);
    p->value[i] = tmp; // 把比较值，放到中间的的位置。
    sprintf(str, "[TMP  :P=%6d %3d -%3d]", tmp, i, j);
    LogMeg(str);
    LogQueueData(p);

    QuickSort(p, low, i - 1);  // 比较左边的
    QuickSort(p, i + 1, high); // 比较右边的
}

int LogQueueData(Squeue *p)
{
    int j = 0;
    char str[1000];
    memset(str, 0, sizeof(str));
    for (int i = 0; i < p->qnum; i++)
    {
        j += sprintf(str + j, "%6d", p->value[i]);
    }
    sprintf(str + j, "\n");
    LogMeg(str);
    return 0;
}
int LogMeg(char *str)
{
    SendToLog(flog, str);
    return 0;
}
