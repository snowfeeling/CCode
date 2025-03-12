#ifndef PONITER_FUNC_H
#define PONITER_FUNC_H
#include <stdio.h>
#include <fstream>
#include <string>
using namespace std;
// 回调函数类型
typedef int (*CompareFunc)(int, int);

enum SORTMODE {ASCENDING, DESCENDING};
enum SORTMOTHOD {BUBBLE, QUICK};

/// @brief 
class SortArray {
public:
    // 构造函数
    SortArray();
    ~SortArray();

    SORTMODE getSortMode();
    SORTMODE setSortMode(SORTMODE sMode);
    SORTMOTHOD setSortMethod(SORTMOTHOD sMethod);
    SORTMOTHOD getSortMethod();

    int sortArray(int arr[], int n);
    int getSortTimes();
    int getSwapTimes();

    typedef void (SortArray::*SortFunction)(int[], int);
    SortFunction pSortFunction; 

private:
    SORTMODE sortMode;
    SORTMOTHOD sortMethod;

    int nSortTimes;
    int nSwapTimes;
    int bubbleSortTimes;
    int quickSortTimes;

    void bubbleSort(int arr[], int n);
    int getBubbleSortTimes();
    int getQuickSortTimes();
    void quickSort(int arr[], int n) ;

    // 冒泡排序函数，接受一个比较函数作为参数
    void bubbleSortFunc(int arr[], int n, CompareFunc cmp) ;
    // 升序比较函数
    static int ascending(int a, int b);
    // 降序比较函数
    static int descending(int a, int b);
    // 交换两个数字Int
    void swapInt(int &a, int &b);
    void swapInt(int arr[], int i, int j);

    // 快速排序的分区
    int quickSortPartition(int arr[], int low, int high, CompareFunc cmp);
    void quickSortFunc(int arr[], int low, int high, CompareFunc cmp) ;
    void showSortStatus(std::ofstream &tsaLogFile, int arr[], int left, int right);
 };

 
int testPointerFunc();
void testShowArray();
#endif