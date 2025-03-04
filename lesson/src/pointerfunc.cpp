#include "../inc/pointerfunc.h"
#include "../inc/chap2.h"
#include <iostream>

using namespace std;

#define ARRAYSIZE  12
#define SORTSIZE   ARRAYSIZE

void printTestSortResult(const char *str, SortArray testSA)
{
    cout << "[SortMode=" << ((testSA.getSortMode() == ASCENDING) ?  "Ascending" : "Descending") << "] ";
    cout << "[SortMethod = " << ((testSA.getSortMethod() == BUBBLE) ? "Bubble" : "Quick") << "] ";
    cout << "[SortTimes = " << testSA.getSortTimes() << "] " << endl << endl;
} 

int testPointerFunc()
{
    SortArray testSA;

    int arr[ARRAYSIZE] = { 212, 98, 120, 8, 123, 899, 13, 35, 901,  3, 81, 34};
    //int arr[ARRAYSIZE] = {   1, 2,   3,  4,   5,   7,    6,  8,   9, 10, 12, 11};

    printArray(arr, ARRAYSIZE);

    testSA.setSortMode(ASCENDING);
    testSA.setSortMethod(BUBBLE);
    testSA.sortArray(arr, SORTSIZE);
    printArray(arr, ARRAYSIZE);
    printTestSortResult("Bubble", testSA);

    // sort the array with quick sort
    testSA.setSortMode(ASCENDING);
    testSA.setSortMethod(QUICK);
    testSA.sortArray(arr, SORTSIZE);
    printArray(arr, ARRAYSIZE);
    printTestSortResult("Quick", testSA);

    // sort the array with descending mode
    testSA.setSortMode(DESCENDING);
    testSA.setSortMethod(BUBBLE);
    testSA.sortArray(arr, SORTSIZE);
    printArray(arr, ARRAYSIZE);
    printTestSortResult("Bubble", testSA);

    return 0;
}

SORTMODE SortArray::setSortMode(SORTMODE sMode)
{
    sortMode = sMode;
    return sortMode;
}

SORTMODE SortArray::getSortMode()
{
    return sortMode;
}
void SortArray::bubbleSort(int arr[], int n)
{   

    if (sortMode == ASCENDING)
        bubbleSortFunc(arr, n, ascending);
    else 
        bubbleSortFunc(arr, n, descending);

}

void SortArray::bubbleSortFunc(int arr[], int n, compareFunc cmp) {

    nSortTimes = 0;
    for (int i = 0; i < n - 1; i++) {
        bool swap = false;
        for (int j = 0; j < n - i - 1; j++) {
            if (cmp(arr[j], arr[j + 1]) > 0) {
                // 交换元素
                swapInt(arr[j], arr[j+1]);

                nSortTimes ++;
                swap = true;
            }
        }
        if (!swap)  
            nSortTimes ++;
    }
} 

// 升序比较函数
 int SortArray::ascending(int a, int b)
{
    return (a - b);
};
// 降序比较函数
 int SortArray::descending(int a, int b)
{
    return (b - a);
}

void SortArray::swapInt(int &a, int &b)
{
    int tmp = a;
    a = b;
    b = tmp;
}


int SortArray::getBubbleSortTimes()
{
    return bubbleSortTimes = nSortTimes;
};

/*
 * QuickSort
*/
// 分区函数实现
int SortArray::quickSortPartition(int arr[], int low, int high, compareFunc cmp) {
    // 选择最后一个元素作为基准元素
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j <= high - 1; j++) {
        if (cmp(arr[j] , pivot) < 0) {
            i++;
            swapInt(arr[i], arr[j]);

            nSortTimes ++;
        }
    }
    swapInt(arr[i + 1], arr[high]);

    return i + 1;
}

// 快速排序的递归函数实现
void SortArray::quickSortFunc(int arr[], int low, int high, compareFunc cmp) {
    if (low < high) {
        // 分区操作，获取基准元素的最终位置
        int pi = quickSortPartition(arr, low, high, cmp);

        // 递归地对基准元素左边的部分进行排序
        quickSortFunc(arr, low, pi - 1, cmp);
        // 递归地对基准元素右边的部分进行排序
        quickSortFunc(arr, pi + 1, high, cmp);
    }
}

void SortArray::quickSort(int arr[], int n)
{
    nSortTimes = 0;
    if (sortMode == ASCENDING)
        quickSortFunc(arr, 0, n-1, ascending);
    else
        quickSortFunc(arr, 0, n-1, descending);
}

int SortArray::getQuickSortTimes()
{
    return quickSortTimes = nSortTimes;
};

SORTMOTHOD SortArray::setSortMethod(SORTMOTHOD sMethod)
{
    sortMethod = sMethod;
    return sortMethod;
}
SORTMOTHOD SortArray::getSortMethod()
{
    return sortMethod;
}

SortArray::SortArray()
{
    nSortTimes = 0;
    bubbleSortTimes = 0;
    quickSortTimes = 0;
    sortMode = ASCENDING;
    sortMethod = BUBBLE;
}

int SortArray::sortArray(int arr[], int n)
{
    // using switch case to implement the sortMethod
    switch (sortMethod) {
        case QUICK:
            quickSort(arr, n);
            break;
        case BUBBLE:

        default:
            bubbleSort(arr, n);
            break;
    }

    return 0;
}

int SortArray::getSortTimes()
{
    return nSortTimes;
}