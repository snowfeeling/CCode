#ifndef PONITER_FUNC_H
#define PONITER_FUNC_H
#include <stdio.h>


// 回调函数类型
typedef int (*compareFunc)(int, int);

enum SORTMODE {ASCENDING, DESCENDING};
enum SORTMOTHOD {BUBBLE, QUICK};

class SortArray {

public:
    // 构造函数
    SortArray();
    
    // 
    SORTMODE setSortMode(SORTMODE sMode);
    SORTMODE getSortMode();
    
    SORTMOTHOD setSortMethod(SORTMOTHOD sMethod);
    SORTMOTHOD getSortMethod();

    int sortArray(int arr[], int n);
    int getSortTimes();

private:
    SORTMODE sortMode;
    SORTMOTHOD sortMethod;

    int nSortTimes;
    int bubbleSortTimes;
    int quickSortTimes;
    // 冒泡排序，可以用升序和降序两个模式
    void bubbleSort(int atr[], int n);
    // 获得冒泡排序次数
    int getBubbleSortTimes();

    // 获得快速排序次数
    int getQuickSortTimes();
    void quickSort(int arr[], int n) ;

    // 冒泡排序函数，接受一个比较函数作为参数
    void bubbleSortFunc(int arr[], int n, compareFunc cmp) ;
    // 升序比较函数
    static int ascending(int a, int b);
    // 降序比较函数
    static int descending(int a, int b);
    // 交换两个数字Int
    void swapInt(int &a, int &b);
    // 快速排序的分区
    int quickSortPartition(int arr[], int low, int high, compareFunc cmp);
    void quickSortFunc(int arr[], int low, int high, compareFunc cmp) ;

 };
 int testPointerFunc();

#endif