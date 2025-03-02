#include "../inc/pointerfunc.h"
#include "../inc/chap2.h"
#include <iostream>

using namespace std;

#define ARRAYSIZE  12
#define ASCENDING   1
#define DESCENDING  2
#define SORTSIZE   ARRAYSIZE
int testPointerFunc()
{
    SortArray testSA;

    int arr[ARRAYSIZE] = { 212, 98, 120, 8, 123, 899, 13, 35, 901,  3, 81, 34};
    //int arr[ARRAYSIZE] = {   1, 2,   3,  4,   5,   6,    7,  8,   9, 10, 12, 11};

    printArray(arr, ARRAYSIZE);

    testSA.quickSort(arr, SORTSIZE, ASCENDING);
    printArray(arr, ARRAYSIZE);
    cout << "Quick Sort Times = " << testSA.getQuickSortTimes() << endl << endl;

    testSA.quickSort(arr, SORTSIZE, DESCENDING);
    printArray(arr, ARRAYSIZE);
    cout << "Quick Sort Times = " << testSA.getQuickSortTimes() << endl << endl;

    testSA.bubbleSort(arr, SORTSIZE, ASCENDING);
    printArray(arr, ARRAYSIZE);
    cout << "Bubble Sort Times = " << testSA.getBubbleSortTimes() << endl << endl;

    testSA.bubbleSort(arr, SORTSIZE, DESCENDING);
    printArray(arr, ARRAYSIZE);
    cout << "Bubble Sort Times = " << testSA.getBubbleSortTimes() << endl << endl;

    testSA.quickSort(arr, SORTSIZE, DESCENDING);
    printArray(arr, ARRAYSIZE);
    cout << "Quick Sort Times = " << testSA.getQuickSortTimes() << endl << endl;

    return 0;
}

void SortArray::bubbleSort(int arr[], int n, int mode)
{   
    if (mode == ASCENDING)
        bubbleSortFunc(arr, n, ascending);
    else 
        bubbleSortFunc(arr, n, descending);

}

void SortArray::bubbleSortFunc(int arr[], int n, compareFunc cmp) {

    bubbleSortTimes = 0;
    for (int i = 0; i < n - 1; i++) {
        bool swap = false;
        for (int j = 0; j < n - i - 1; j++) {
            if (cmp(arr[j], arr[j + 1]) > 0) {
                // 交换元素
                swapInt(arr[j], arr[j+1]);

                bubbleSortTimes ++;
                swap = true;
            }
        }
        if (!swap)  
            bubbleSortTimes ++;
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

SortArray::SortArray()
{
    bubbleSortTimes = 0;
    quickSortTimes = 0;
}

int SortArray::getBubbleSortTimes()
{
    return bubbleSortTimes;
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

            quickSortTimes ++;
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

void SortArray::quickSort(int arr[], int n, int mode)
{
    quickSortTimes = 0;
    if (mode == ASCENDING)
        quickSortFunc(arr, 0, n-1, ascending);
    else
        quickSortFunc(arr, 0, n-1, descending);

}

int SortArray::getQuickSortTimes()
{
    return quickSortTimes;
};