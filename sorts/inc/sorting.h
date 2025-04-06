#ifndef SORTING_H
#define SORTING_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <list>

// 定义排序类型
enum SortType { ASCENDING, DESCENDING };
// 定义排序方法
enum SortMethod { BUBBLE_SORT, SHELL_SORT, QUICK_SORT, BPLUS_TREE_SORT };

class Sorting {
private:
    std::vector<int> data;
    SortType sortType;
    SortMethod sortMethod;
    std::ofstream logFile;
    int compareCount;
    int swapCount;

    // 记录日志
    void log(const std::string& message);
    // 交换两个元素
    void swap(int& a, int& b);
    // 比较两个元素
    bool compare(int a, int b);

    // 冒泡排序
    void bubbleSort();
    // 希尔排序
    void shellSort();
    // 快速排序
    void quickSort(int left, int right);
    // B+树排序
    void bPlusTreeSort();

public:
    // 构造函数
    Sorting();
    // 析构函数
    ~Sorting();

    // 从文件加载数据
    void loadDataFromFile(const std::string& filename);
    // 设置排序类型
    void setSortType(SortType type);
    // 设置排序方法
    void setSortMethod(SortMethod method);
    // 执行排序
    void testSort();

    void performSort();
    // 打印排序后的数据
    void printData();

    // 新增函数：获取格式化的时间戳
    std::string getCurrentTimestamp();
};

#endif // SORTING_H