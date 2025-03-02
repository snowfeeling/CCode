#include "../inc/pointerfunc.h"
#include "../inc/chap2.h"
#include <iostream>

using namespace std;

#define ARRAYSIZE 12
int testPointerFunc()
{
    SortArray testSA;

    int arr[ARRAYSIZE] = { 212, 8, 120, 98, 123, 899, 1234, 35, 901,  3, 81, 34};
    //int arr[ARRAYSIZE] = {   1, 2,   3,  4,   5,   6,    7,  8,   9, 10, 12, 11};

    printArray(arr, ARRAYSIZE);
    
    testSA.bubbleSortAsceding(arr, ARRAYSIZE);
    printArray(arr, ARRAYSIZE);
    cout << testSA.getBubbleSortTimes() << endl;

    testSA.bubbleSortAsceding(arr, ARRAYSIZE);
    printArray(arr, ARRAYSIZE);
    cout << testSA.getBubbleSortTimes() << endl;

    testSA.bubbleSortDescending(arr, ARRAYSIZE);
    printArray(arr, ARRAYSIZE);
    cout << testSA.getBubbleSortTimes() << endl;

    testSA.bubbleSortAsceding(arr, ARRAYSIZE);
    printArray(arr, ARRAYSIZE);
    cout << "The Sort Times = " << testSA.getBubbleSortTimes() << endl;

    return 0;
}

void SortArray::bubbleSortAsceding(int arr[], int n)
{
    bubbleSort(arr, n, ascending);
}

void SortArray::bubbleSortDescending(int arr[], int n)
{
    bubbleSort(arr, n, descending);
}

void SortArray::bubbleSort(int arr[], int n, compareFunc cmp) {

    bubbleSortTimes = 0;
    for (int i = 0; i < n - 1; i++) {
        bool swap = false;
        for (int j = 0; j < n - i - 1; j++) {
            if (cmp(arr[j], arr[j + 1]) > 0) {
                // 交换元素
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                
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

SortArray::SortArray()
{
    bubbleSortTimes = 0;
}

int SortArray::getBubbleSortTimes()
{
    return bubbleSortTimes;
};
