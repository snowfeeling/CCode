#include "../inc/QuickSortLogger.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>

using namespace std;

int main()
{
    string fileName = "../data/numbers.txt";      // 文件名
    string logFileName = "../log/tsaLogFile.txt"; // 日志文件名

    ifstream file(fileName);
    if (!file.is_open())
    {
        cerr << "Failed to open file: " << fileName << endl;
        return 1;
    }

    string line;
    getline(file, line);
    file.close();

    vector<int> numbers;
    stringstream ss(line);
    string number;
    while (getline(ss, number, ','))
    {
        numbers.push_back(stoi(number));
    }

    QuickSortLogger sorter(logFileName);
    // 修改: 使用 setSortType 方法进行升序排序
    sorter.setSortType(DESCENDING);
    sorter.sort(numbers);

    return 0;
}