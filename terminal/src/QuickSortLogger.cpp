#include "../inc/QuickSortLogger.h"
#include <iostream>
#include <iomanip> // 添加头文件以支持时间格式化
#include <sstream> // 添加头文件以支持字符串流操作

QuickSortLogger::QuickSortLogger(const std::string &logFileName) : logFile(logFileName), comparisons(0), swaps(0), sortType(ASCENDING) {}

void QuickSortLogger::sort(std::vector<int> &arr)
{
    CompareFunc compareFunc = [this](int a, int b)
    {
        return this->sortType == ASCENDING ? a < b : a > b;
    };
    quickSort(arr, 0, arr.size() - 1, compareFunc);
    logFile << "Total Comparisons: " << comparisons << ", Total Swaps: " << swaps << std::endl;
}

void QuickSortLogger::sortAscending(std::vector<int> &arr)
{
    setSortType(ASCENDING);
    sort(arr);
}

void QuickSortLogger::sortDescending(std::vector<int> &arr)
{
    setSortType(DESCENDING);
    sort(arr);
}

void QuickSortLogger::setSortType(SortType type)
{
    sortType = type;
}

SortType QuickSortLogger::getSortType()
{
    return sortType;
}

void QuickSortLogger::quickSort(std::vector<int> &arr, int low, int high, CompareFunc compareFunc)
{
    if (low < high)
    {
        int pi = partition(arr, low, high, compareFunc);
        quickSort(arr, low, pi - 1, compareFunc);
        quickSort(arr, pi + 1, high, compareFunc);
    }
}

int QuickSortLogger::partition(std::vector<int> &arr, int low, int high, CompareFunc compareFunc)
{
    int pivot = arr[high];
    int i = low - 1;
    for (int j = low; j <= high - 1; j++)
    {
        comparisons++;
        if (compareFunc(arr[j], pivot))
        {
            i++;
            std::swap(arr[i], arr[j]);
            swaps++;
            logSwap(arr, high, i, j);
            logArray(arr, 0, arr.size() - 1, i, j, "Swap");
        }
    }
    std::swap(arr[i + 1], arr[high]);
    swaps++;
    logSwap(arr, high, i + 1, high);
    return i + 1;
}

void QuickSortLogger::logArray(const std::vector<int> &arr, int low, int high, int i, int j, const std::string &action)
{
    logFile << getCurrentTime() << " [" << action << "] ";
    for (int k = low; k <= high; k++)
    {
        if (k == i || k == j)
        {
            logFile << "(" << arr[k] << "), ";
        }
        else
        {
            logFile << arr[k] << ", ";
        }
    }
    logFile << "\n";
}

void QuickSortLogger::log(const std::string &message)
{
    logFile << getCurrentTime() << " " << message << "\n";
}

void QuickSortLogger::logSwap(std::vector<int> &arr, int pivotIndex, int i, int j)
{
    logFile << getCurrentTime() << " [";
    for (size_t k = 0; k < arr.size(); k++)
    {
        if (k == pivotIndex)
        {
            logFile << "[" << arr[k] << "), ";
        }
        else if (k == i || k == j)
        {
            logFile << "(" << arr[k] << "), ";
        }
        else
        {
            logFile << arr[k] << ", ";
        }
    }
    logFile << "] Swapped " << arr[i] << " and " << arr[j] << "\n";
}

// 添加 getCurrentTime 函数
std::string QuickSortLogger::getCurrentTime()
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = *std::localtime(&now_time);

    std::ostringstream oss;
    oss << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}