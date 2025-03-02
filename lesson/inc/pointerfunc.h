#ifndef PONITER_FUNC_H
#define PONITER_FUNC_H
#include <stdio.h>


// 回调函数类型
typedef int (*compareFunc)(int, int);

class SortArray {

    public:
        // 冒泡升序
        void bubbleSortAsceding(int atr[], int n);
        // 冒泡降序
        void bubbleSortDescending(int atr[], int n);
        // 构造函数
        SortArray();
        // 获得冒泡排序测试
        int getBubbleSortTimes();
        int bubbleSortTimes;
        
    private:
        // 冒泡排序函数，接受一个比较函数作为参数
        void bubbleSort(int arr[], int n, compareFunc cmp) ;
        // 升序比较函数
        static int ascending(int a, int b);
        // 降序比较函数
        static int descending(int a, int b);
 };
 int testPointerFunc();


#endif