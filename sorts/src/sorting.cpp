#include "../inc/sorting.h"
#include <iomanip>
#include <sstream>

#define LOG_FILE_PATH "../log/sorting_log.txt"
#define MAX_DATA_SIZE 100
#define NUM_WIDTH 4
using namespace std;

Sorting::Sorting() : sortType(ASCENDING), sortMethod(BUBBLE_SORT), compareCount(0), swapCount(0)
{
    logFile.open(LOG_FILE_PATH, std::ios::out);
    if (!logFile.is_open())
    {
        throw std::runtime_error("Failed to open log file: " + (string)LOG_FILE_PATH);
    }
}

Sorting::~Sorting()
{
    if (logFile.is_open())
    {
        try
        {
            logFile.flush();
            logFile.close();
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error closing log file: " << e.what() << std::endl;
        }
    }
}

void Sorting::log(const std::string &message)
{
    logFile << "[" << getCurrentTimestamp() << "] " << message << std::endl;
}

void Sorting::swap(int &a, int &b)
{
    int temp = a;
    a = b;
    b = temp;

    ++swapCount;
    std::ostringstream oss;
    oss << std::setw(10) << "Swapping " << setw(NUM_WIDTH) << a << " <-> " << setw(NUM_WIDTH) << b << setw(20) << " Swap count: " << swapCount;
    log(oss.str());
}

bool Sorting::compare(int a, int b)
{
    ++compareCount;
    std::ostringstream oss;
    oss << "Comparing " << setw(NUM_WIDTH) << a << " <-> " << setw(NUM_WIDTH) << b << setw(20) << " Compare count: " << compareCount;
    log(oss.str());
    return (sortType == ASCENDING) ? a > b : a < b;
}

void Sorting::bubbleSort()
{
    int n = data.size();
    for (int i = 0; i < n - 1; ++i)
    {
        for (int j = 0; j < n - i - 1; ++j)
        {
            if (compare(data[j], data[j + 1]))
            {
                swap(data[j], data[j + 1]);
            }
        }
    }
}

void Sorting::shellSort()
{
    int n = data.size();
    for (int gap = n / 2; gap > 0; gap /= 2)
    {
        for (int i = gap; i < n; ++i)
        {
            int temp = data[i];
            int j;
            for (j = i; j >= gap && compare(data[j - gap], temp); j -= gap)
            {
                swap(data[j], data[j - gap]);
            }
            data[j] = temp;
        }
    }
}

void Sorting::quickSort(int left, int right)
{
    if (left < right)
    {
        int pivot = data[right];
        int i = left - 1;
        for (int j = left; j < right; ++j)
        {
            if (compare(pivot, data[j]))
            {
                ++i;
                swap(data[i], data[j]);
            }
        }
        swap(data[i + 1], data[right]);
        int partitionIndex = i + 1;
        quickSort(left, partitionIndex - 1);
        quickSort(partitionIndex + 1, right);
    }
}

void Sorting::loadDataFromFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            std::stringstream ss(line);
            std::string token;
            while (std::getline(ss, token, ','))
            {
                data.push_back(std::stoi(token));
            }
        }
        file.close();
    }
}

void Sorting::setSortType(SortType type)
{
    sortType = type;
}

void Sorting::setSortMethod(SortMethod method)
{
    sortMethod = method;
}

void Sorting::testSort()
{
    loadDataFromFile("../data/input.txt");

    log("------------------------------------");
    setSortType(ASCENDING);
    setSortMethod(QUICK_SORT);
    performSort();

    log("------------------------------------");
    setSortType(ASCENDING);
    setSortMethod(SHELL_SORT);
    performSort();

    log("------------------------------------");
    setSortType(DESCENDING);
    setSortMethod(BPLUS_TREE_SORT);
    performSort();
}


void Sorting::bPlusTreeSort() {
    // 简化的B+树实现
    struct BPlusTreeNode {
        bool isLeaf;
        std::vector<int> keys;
        std::vector<BPlusTreeNode*> children;
        BPlusTreeNode* nextLeaf;

        BPlusTreeNode(bool leaf) : isLeaf(leaf), nextLeaf(nullptr) {}
    };

    struct BPlusTree {
        BPlusTreeNode* root;
        int degree;

        BPlusTree(int _degree) : degree(_degree), root(new BPlusTreeNode(true)) {}

        void insert(int key) {
            BPlusTreeNode* root = this->root;
            if (root->keys.size() == 2 * degree - 1) {
                BPlusTreeNode* newRoot = new BPlusTreeNode(false);
                newRoot->children.push_back(root);
                splitChild(newRoot, 0, root);
                root = newRoot;
            }
            insertNonFull(root, key);
            this->root = root;
        }

        void splitChild(BPlusTreeNode* parent, int index, BPlusTreeNode* child) {
            BPlusTreeNode* newNode = new BPlusTreeNode(child->isLeaf);
            newNode->keys.assign(child->keys.begin() + degree, child->keys.end());
            if (!child->isLeaf) {
                newNode->children.assign(child->children.begin() + degree, child->children.end());
            }
            child->keys.resize(degree - 1);
            child->children.resize(degree);
            parent->children.insert(parent->children.begin() + index + 1, newNode);
            parent->keys.insert(parent->keys.begin() + index, child->keys[degree - 1]);
        }

        void insertNonFull(BPlusTreeNode* node, int key) {
            int i = node->keys.size() - 1;
            if (node->isLeaf) {
                node->keys.push_back(0);
                while (i >= 0 && key < node->keys[i]) {
                    node->keys[i + 1] = node->keys[i];
                    --i;
                }
                node->keys[i + 1] = key;
            } else {
                while (i >= 0 && key < node->keys[i]) {
                    --i;
                }
                ++i;
                if (node->children[i]->keys.size() == 2 * degree - 1) {
                    splitChild(node, i, node->children[i]);
                    if (key > node->keys[i]) {
                        ++i;
                    }
                }
                insertNonFull(node->children[i], key);
            }
        }

        std::vector<int> getSortedKeys() {
            std::vector<int> result;
            BPlusTreeNode* node = root;
            while (!node->isLeaf) {
                node = node->children[0];
            }
            while (node != nullptr) {
                result.insert(result.end(), node->keys.begin(), node->keys.end());
                node = node->nextLeaf;
            }
            return result;
        }
    };

    BPlusTree bPlusTree(3); // 假设B+树的度为3
    for (int key : data) {
        bPlusTree.insert(key);
    }
    data = bPlusTree.getSortedKeys();
}

void Sorting::performSort() {
    compareCount = 0;
    swapCount = 0;

    switch (sortMethod) {
    case BUBBLE_SORT:
        bubbleSort();
        break;
    case SHELL_SORT:
        shellSort();
        break;
    case QUICK_SORT:
        quickSort(0, data.size() - 1);
        break;
    case BPLUS_TREE_SORT:
        bPlusTreeSort();
        break;
    }
    printData();
    log("Total compare: " + std::to_string(compareCount) + "  Total swap: " + std::to_string(swapCount));
}

void Sorting::printData()
{
    if (!data.empty())
    {
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < data.size(); ++i)
        {
            oss << std::setw(NUM_WIDTH) << data[i] << ((i != data.size() - 1) ? ", " : "");
        }
        oss << "]";
        log(oss.str());
    }
}

// 新增函数：获取格式化的时间戳
std::string Sorting::getCurrentTimestamp()
{
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    auto value = now_ms.time_since_epoch();
    long long duration = value.count();
    long long milliseconds = duration % 1000;
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now_ms);
    std::tm now_tm = *std::localtime(&now_time_t);

    std::ostringstream oss;
    oss << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S") << "." << std::setw(3) << std::setfill('0') << milliseconds;
    return oss.str();
}