#include "../inc/pointerfunc.h"
#include <iostream>
#include <conio.h>
#include <iomanip>

#define DEBUG

using namespace std;

#define MYARRAYSIZE 13
#define SORTSIZE MYARRAYSIZE

std::ofstream tsaLogFile;

void printArrayTitle(std::ofstream &tsaLogFile, int arr[], int num)
{
    tsaLogFile << "arr[" << setw(3) << num << "]" << endl;
    tsaLogFile << "[";
    for (int i = 0; i < num; i++)
        tsaLogFile << " " << setw(4) << i << ((i == num - 1) ? " ]" : ",");
    tsaLogFile << endl;
}
void printArray(std::ofstream &tsaLogFile, int *arr, int num)
{
    // printArrayTitle(tsaLogFile, arr, num);
    tsaLogFile << "[";
    for (int i = 0; i < num; i++)
        tsaLogFile << " " << setw(4) << arr[i] << ((i == num - 1) ? " ]" : ",");
    // tsaLogFile << endl;
}

void printTestSortResult(std::ofstream &tsaLogFile, SortArray testSA)
{
    tsaLogFile << endl
               << "[SortMode=" << ((testSA.getSortMode() == ASCENDING) ? "Ascending" : "Descending") << "] "
               << "[SortMethod = " << ((testSA.getSortMethod() == BUBBLE) ? "Bubble" : "Quick") << "] "
               << "[Sorted=" << setw(2) << testSA.getSortTimes() << "] "
               << "[Swapped=" << setw(2) << testSA.getSwapTimes() << "]" << endl
               << endl;
}
void SortArray::showSortStatus(std::ofstream &tsaLogFile, int arr[], int left, int right)
{
#ifdef DEBUG
    printArray(tsaLogFile, arr, MYARRAYSIZE);
    tsaLogFile << endl
               << "[";
    for (int k = 0; k < MYARRAYSIZE; k++)
    {
        tsaLogFile << " " << setw(4);
        if (k == left || k == right)
            tsaLogFile << k;
        else
            tsaLogFile << " ";
        tsaLogFile << ((k == MYARRAYSIZE - 1) ? " ]" : " ");
    }
    tsaLogFile << " Sorted=" << setw(2) << nSortTimes << " Swapped=" << setw(2) << nSwapTimes
               << " [ " << setw(2) << left << " <-> " << setw(2) << right << " ]" << endl;
#endif
}
/* 下面是Copliot写的commeent.
// testPointerFunc is a function to test the pointer function
*/

int testPointerFunc()
{
    try
    {
        tsaLogFile.open("../log1/tsaLogFile.txt", std::ios_base::out);
        if (!tsaLogFile.is_open())
        {
            throw runtime_error("A runtime error occurred: Could not open the file!");
        }
        SortArray testSA;
        int arr[MYARRAYSIZE] = {3, 8, 13, 50, 81, 34, 212, 98, 123, 120, 899, 901, 95};

        printArray(tsaLogFile, arr, MYARRAYSIZE);

        // quicksort the arrary with Ascending mode
        testSA.setSortMode(ASCENDING);
        testSA.setSortMethod(QUICK);
        printArrayTitle(tsaLogFile, arr, MYARRAYSIZE);
        testSA.sortArray(arr, SORTSIZE);
        printArray(tsaLogFile, arr, MYARRAYSIZE);
        printTestSortResult(tsaLogFile, testSA);

        printArrayTitle(tsaLogFile, arr, MYARRAYSIZE);
        testSA.setSortMode(DESCENDING);
        testSA.setSortMethod(BUBBLE);
        testSA.sortArray(arr, SORTSIZE);
        printArray(tsaLogFile, arr, MYARRAYSIZE);
        printTestSortResult(tsaLogFile, testSA);

        // testShowArray();

        tsaLogFile.close();
    }
    //catch (std::ofstream::failure e)
    catch (const exception& e)
    {
        std::cerr << "Exception : " << e.what() << endl;
    }
    catch (...)
    {
        std::cerr << "Exception : Unknown" << endl;
    }
    return 0;
}

/**
 * 设置排序模式
 *
 * @param sMode 排序模式，用于指定数组应按照何种方式排序
 * @return 当前的排序模式，以便调用者确认排序模式是否设置成功
 */
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
    CompareFunc cmp = (sortMode == ASCENDING) ? ascending : descending;
    bubbleSortFunc(arr, n, cmp);
}

void SortArray::bubbleSortFunc(int arr[], int n, CompareFunc cmp)
{

    nSortTimes = 0;
    nSwapTimes = 0;
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            nSortTimes++;
            if (cmp(arr[j], arr[j + 1]) > 0)
            {
                // 交换元素
                swapInt(arr, j, j + 1);
            }
        }
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
    nSwapTimes++;
}
// Swaps the elements at indices i and j in the array arr
void SortArray::swapInt(int arr[], int i, int j)
{
    int tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
    nSwapTimes++;
#ifdef DEBUG
    showSortStatus(tsaLogFile, arr, i, j);
#endif
}

int SortArray::getBubbleSortTimes()
{
    return bubbleSortTimes = nSortTimes;
};

/*
 * QuickSort分区函数实现
 */
int SortArray::quickSortPartition(int arr[], int low, int high, CompareFunc cmp)
{
    // 选择最后一个元素作为基准元素
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j <= high - 1; j++)
    {
        nSortTimes++;
        if (cmp(arr[j], pivot) < 0)
        {
            i++;
            swapInt(arr, i, j);
        }
    }
    swapInt(arr, i + 1, high);

    return i + 1;
}

void showPartitionStatus(std::ofstream &tsaLogFile, int arr[], int low, int high, int pi)
{
    if (low >= high)
        return;

    tsaLogFile << "[";
    for (int k = 0; k < MYARRAYSIZE; k++)
    {
        if (k == pi - 1 and low == pi)
            tsaLogFile << "[";
        else if (k != pi && ((k == low && k == pi - 1) || k == low || k == pi + 1))
            tsaLogFile << "[";
        else
            tsaLogFile << " ";
        tsaLogFile << setw(4);
        if (k == pi - 1 || k == pi || k == pi + 1 || k == low || k == high)
            tsaLogFile << k;
        else
            tsaLogFile << " ";

        if ((k == pi - 1) || (k == pi + 1 && pi == high) || (k == high && pi < high))
            tsaLogFile << "]";
        else
            tsaLogFile << " ";
    }
    tsaLogFile << "]";
    tsaLogFile << " [partion: low=" << low << " pivot=" << pi << " high=" << high << "]" << endl;
}

// 快速排序的递归函数实现
void SortArray::quickSortFunc(int arr[], int low, int high, CompareFunc cmp)
{
    if (low < high)
    {
        // 分区操作，获取基准元素的最终位置
        tsaLogFile << "[" << low << setw(5) << " " << high << "]" << endl;
        int pi = quickSortPartition(arr, low, high, cmp);
        tsaLogFile << "[" << low << " " << pi - 1 << "] " << pi << " [" << pi + 1 << " " << high << "]" << endl;
        showPartitionStatus(tsaLogFile, arr, low, high, pi);

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
        quickSortFunc(arr, 0, n - 1, ascending);
    else
        quickSortFunc(arr, 0, n - 1, descending);
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
    nSwapTimes = 0;
    bubbleSortTimes = 0;
    quickSortTimes = 0;
    sortMode = ASCENDING;
    sortMethod = BUBBLE;
}
SortArray::~SortArray()
{
}

int SortArray::sortArray(int arr[], int n)
{
    // using switch case to implement the sortMethod
    switch (sortMethod)
    {
    case QUICK:
        pSortFunction = quickSort;
        break;
    case BUBBLE:
    default:
        pSortFunction = bubbleSort;
        break;
    }
    (this->*pSortFunction)(arr, n);

    return 0;
}

int SortArray::getSortTimes()
{
    return nSortTimes;
}

int SortArray::getSwapTimes()
{
    return nSwapTimes;
}
void testShowArray()
{
    cout << "testShowArray";
    // 无回显输入一个字符
    getch();
    cout << "\r";
    getch();
    cout << endl;
    cout << "go to next line1" << endl;
    cout << "go to next line2" << endl;
    cout << "go to next line3" << endl;
    getch();
    cout << "\033[F";
    getch();
    cout << "\033[F" << "12345678" << "\r";
    getch();
    cout << "\033[1A" << "\r";
    getch();
}

/*
[1234,5678,9012,3456,7890,1234,5678,9012,3456,7890,1234,5678,9012,3456,7890,1234,5678,9012]
[    1,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,   12]
[ 1234, 1234, 1234, 3456] 5678[ 1234, 5678, 9012, 3456, 7890, 1234, 5678]
[    L                                                                 H]
i=0
L=0
H=11
[    L                                                                 H]
i=11




*/