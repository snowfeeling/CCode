#ifndef QUICKSORTLOGGER_H
#define QUICKSORTLOGGER_H

#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <ctime>
#include <functional>

// 修改: 使用 typedef 定义 CompareFunc
typedef std::function<bool(int, int)> CompareFunc;

// 提升 enum SortType 到类定义外部
enum SortType
{
    ASCENDING,
    DESCENDING
}; // 定义排序类型枚举

class QuickSortLogger
{
public:
    QuickSortLogger(const std::string &logFileName);
    void sort(std::vector<int> &arr); // 修改为不接受比较函数参数
    void sortAscending(std::vector<int> &arr);
    void sortDescending(std::vector<int> &arr);
    void setSortType(SortType type); // 添加 setSortType 声明
    SortType getSortType();          // 添加 getSortType 声明

private:
    std::ofstream logFile;
    int comparisons;
    int swaps;
    SortType sortType; // 使用枚举类型表示排序类型

    void quickSort(std::vector<int> &arr, int low, int high, CompareFunc compareFunc);
    int partition(std::vector<int> &arr, int low, int high, CompareFunc compareFunc);
    void logArray(const std::vector<int> &arr, int low, int high, int i, int j, const std::string &action);
    void log(const std::string &message);
    void logSwap(std::vector<int> &arr, int pivotIndex, int i, int j); // 新增 logSwap 成员函数
    std::string getCurrentTime();                                      // 新增 getCurrentTime 成员函数声明
};

#endif // QUICKSORTLOGGER_H