#ifndef MYDATATYPE_C_GUARD
#define MYDATATYPE_C_GUARD

#include "../include/mylog.h"
#include "../include/mydatatype.h"

void showDataTypeSize(void)
{
    char typeName[10];
    size_t typeSize;

    char ch;
    short sh;
    int num;
    float f;
    unsigned ui;
    signed si;
    void *ptr;
    _Bool b;
    // typeof(ch) nch;

    GET_TYPE_NAME(typeName, b);
    typeSize = GET_TYPE_SIZE(b);
    printf("Type: %s, Size: %zu byte(s)\n", typeName, typeSize);

    PRINT_TYPE_INFO(typeSize, "size_t");
    PRINT_TYPE_INFO(sh, "short");
    PRINT_TYPE_INFO(ch, "char");
    PRINT_TYPE_INFO(num, "int");
    PRINT_TYPE_INFO(f, "float");
    PRINT_TYPE_INFO(typeName, "char []");
    PRINT_TYPE_INFO(ui, "unsigned");
    PRINT_TYPE_INFO(si, "signed");
    PRINT_TYPE_INFO(ptr, "void *");
    PRINT_TYPE_INFO(b, "_Bool");
    PRINT_TYPE_INFO(sizeof(int), "sizeof(int)");
    // PRINT_TYPE_INFO(nch, "typeof() declare char");
}
// 定义四个函数 加减乘数
int add(int a, int b)
{
    return a + b;
}
int subtract(int a, int b)
{
    return a - b;
}
int multiply(int a, int b)
{
    return a * b;
}
int divide(int a, int b)
{
    if (b == 0)
    {
        printf("Error: devision by ZERO!");
        return 0;
    }
    return a / b;
}
// 定义指针函数
typedef int (*MathOperation)(int, int);

int testFuncPtr()
{
    // 函数指针，申请四个指针；其实也可以直接用数组，不过，实际开发中，用指针的场景更多些。
    MathOperation *mathOperation;
    mathOperation = (MathOperation *)calloc(4, sizeof(MathOperation));
    // 给函数指针赋值
    mathOperation[0] = &add;
    mathOperation[1] = &subtract;
    mathOperation[2] = &multiply;
    mathOperation[3] = &divide;
    int a = 10;
    int b = 2;
    for (int i = 0; i < 4; i++)
    {
        // 调用函数指针指向不同的函数
        int result = mathOperation[i](a, b);
        switch (i)
        {
        case 0:
        case 1:
        case 2:
        case 3:
            // 输出结果，这里有个小技巧
            printf("%d %c %d = %d\n", a, "+-*/"[i], b, result);
            break;
        default:
            break;
        }
    }
    free(mathOperation);
    return 0;
}

DataNode *createNode(int index, const char *name, double value)
{
    DataNode *newNode = (DataNode *)calloc(1, sizeof(DataNode));
    if (!newNode)
    {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    newNode->index = index;
    strncpy(newNode->name, name, sizeof(newNode->name) - 1);
    newNode->name[sizeof(newNode->name) - 1] = '\0'; // Ensure null-termination
    newNode->value = value;
    newNode->prev = NULL;
    newNode->next = NULL;
    return newNode;
}

void appendNode(DataList *list, int index, const char *name, double value)
{
    DataNode *newNode = createNode(index, name, value);

    if (list->head == NULL)
    {
        list->head = newNode;
        list->tail = newNode;
        list->size = 1;
    }
    else
    {
        list->tail->next = newNode;
        newNode->prev = list->tail;
        list->tail = newNode;
        list->size++;
    }
}
void freeList(DataList *list)
{
    DataNode *tmp;
    DataNode *head = list->head;
    while (head != NULL)
    {
        tmp = head;
        head = head->next;
        free(tmp);
    }
    list->head = NULL;
    list->tail = NULL;
}

DataList createNodeListFromFile()
{
    FILE *file = fopen("../data/testv.data", "r");
    if (!file)
    {
        perror("Failed to open file");
        DataList emptyList = {.head = NULL, .tail = NULL};
        return emptyList;
    }

    DataList list = {.head = NULL, .tail = NULL};
    char line[1024];

    while (fgets(line, sizeof(line), file))
    {
        int index;
        char name[256];
        double value;

        if (sscanf(line, "%d,%255[^,],%lf", &index, name, &value) == 3)
        {
            appendNode(&list, index, name, value);
        }
    }

    fclose(file);
    return list;
}
void printOneNode(DataNode *node)
{
    printf("Index: %3d, Value: %6.2f, Name: %s\n", node->index, node->value, node->name);
}
void printListFromHead(const DataList *list)
{
    for (DataNode *current = list->head; current != NULL; current = current->next)
    {
        printOneNode(current);
    }
}

void printListFromTail(const DataList *list)
{
    DataNode *current = list->tail;
    for (DataNode *current = list->tail; current != NULL; current = current->prev)
    {
        printOneNode(current);
    }
}
// 比较函数：index
bool compareIndex(DataNode *node, void *target)
{
    int targetIndex = *(int *)target;
    return node->index == targetIndex;
}

// 比较函数： value（使用精度判断）
bool compareValue(DataNode *node, void *target)
{
    double targetValue = *(double *)target;
    return fabs(node->value - targetValue) < 1e-6;
}
// 比较函数： name
bool compareName(DataNode *node, void *target)
{
    const char *targetName = (const char *)target;
    return strcmp(node->name, targetName) == 0;
}
// 比较函数： 模糊比较name（支持子字符串匹配）
bool compareNameFuzzy(DataNode *node, void *target)
{
    const char *targetName = (const char *)target;
    return strstr(node->name, targetName) != NULL;
}
DataNodeMatchResult findNodesByCriteria(CriteriaNode *criteriaNode)
{
    DataNode *current = criteriaNode->head;
    void *target = criteriaNode->target;
    CompareFunc compare = criteriaNode->compareFunc;
    int capacity = 4;
    int count = 0;

    DataNode **resultArray = (DataNode **)calloc(capacity, sizeof(DataNode *));
    if (!resultArray)
    {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    while (current != NULL)
    {
        if (compare(current, target))
        {
            if (count >= capacity)
            {
                capacity *= 2;
                resultArray = (DataNode **)realloc(resultArray, capacity * sizeof(DataNode *));
                if (!resultArray)
                {
                    perror("Memory reallocation failed");
                    exit(EXIT_FAILURE);
                }
            }
            resultArray[count++] = current;
        }
        current = current->next;
    }

    DataNodeMatchResult result = {.nodes = resultArray, .count = count};
    return result;
}
DataNode *findFirstNodeByCriteria(CriteriaNode *criteriaNode)
{
    DataNode *current = criteriaNode->head;
    void *target = criteriaNode->target;
    CompareFunc compare = criteriaNode->compareFunc;

    while (current != NULL)
    {
        if (compare(current, target))
        {
            return current; // 找到匹配的节点
        }
        current = current->next;
    }
    return NULL; // 未找到
}

void freeNodeMatchResult(DataNodeMatchResult *result)
{
    if (result && result->nodes)
    {
        free(result->nodes);
        result->nodes = NULL;
        result->count = 0;
    }
}
int findByCriteria(CriteriaNode *criteriaNode)
{
    DataNodeMatchResult result = findNodesByCriteria(criteriaNode);
    for (int i = 0; i < result.count; i++)
    {
        printOneNode(result.nodes[i]);
    }
    int resultCount = result.count;
    freeNodeMatchResult(&result);
    return resultCount;
}
// Delare the array of comparison functions
CompareFunc comparFuncs[] = {compareIndex, compareName, compareValue, compareNameFuzzy};

void find(DataList *list)
{
    int searchIndex = 19;
    CriteriaNode criteriaNode;
    criteriaNode.head = list->head;
    criteriaNode.target = &searchIndex;
    criteriaNode.compareFunc = comparFuncs[0];
    int resultCount = findByCriteria(&criteriaNode);
    printf("Found %d node(s) with Index: %d\n\n", resultCount, searchIndex);

    // 查找 value == 90 的节点
    double searchValue = 23.5;
    criteriaNode.head = list->head;
    criteriaNode.target = &searchValue;
    criteriaNode.compareFunc = comparFuncs[2];
    resultCount = findByCriteria(&criteriaNode);
    printf("Found %d nodes with Value: %.2f\n\n", resultCount, searchValue);

    // 查找 name
    const char *searchName = "zhaowu";
    criteriaNode.head = list->head;
    criteriaNode.target = (void *)searchName;
    criteriaNode.compareFunc = comparFuncs[1];
    resultCount = findByCriteria(&criteriaNode);
    printf("Found %d nodes with Name: '%s'\n\n", resultCount, searchName);

    // 查找 Fuzzy name
    const char *searchFuzzyName = "Tom";
    criteriaNode.head = list->head;
    criteriaNode.target = (void *)searchFuzzyName;
    criteriaNode.compareFunc = comparFuncs[3];
    resultCount = findByCriteria(&criteriaNode);
    printf("Found %d nodes with Name: '%s'\n\n", resultCount, searchFuzzyName);
}

void findByIndex(DataNode *head)
{
    int searchIndex = 2;
    CriteriaNode criteriaNode;
    criteriaNode.head = head;
    criteriaNode.target = &searchIndex;
    criteriaNode.compareFunc = comparFuncs[0];
    DataNode *result = findFirstNodeByCriteria(&criteriaNode);
    if (result != NULL)
    {
        printOneNode(result);
        printf("Found the node with Index %d\n", searchIndex);
    }
    else
    {
        printf("Found 0 node found with Index %d\n", searchIndex);
    }
}
void printNodeData(DataNode *node)
{
    printf("[%3d,%6.2f,%6s]", node->index, node->value, node->name);
}
void printArray(DataNode (*arr)[4])
{
    // DataNode(*pRow)[] = &arr[1];
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            DataNode node = arr[i][j];
            printNodeData(&arr[i][j]);
        }
        printf("\n");
    }
}
void testArray()
{
    DataNode arr[2][4] = {
        {{1, "Node1", 10.5}, {2, "Node2", 20.5}, {3, "Node3", 30.5}, {4, "Node4", 40.5}},
        {{5, "Node5", 50.5}, {6, "Node6", 60.5}, {7, "Node7", 70.5}, {8, "Node8", 80.6}}};
    printArray(arr);
    DataNode(*arrPtr)[4];
    arrPtr = arr;
    printArray(arrPtr);
    printNodeData(&arrPtr[0][2]);

    printNodeData(&(arrPtr + 1)[0][3]);
    printNodeData((arrPtr + 1)[0]);
    arrPtr++;
    printNodeData(&(arrPtr)[0][3]);

    // printArray(arrPtr);
    printNodeData(arrPtr[0]);
    printNodeData(&(arrPtr)[0][0]);
}

void testStruct()
{
    DataNode node1 = {1, "Node1", 10.5, NULL, NULL};
    DataNode node2 = {2, "Node2", 20.5};
    DataNode node3 = {3, "Node3", 30.5};
    DataNode node4;
    DataNode *pNode1, *pNode2, *pNode3, *pNode4;
    node4 = (DataNode){.index = 4, .name = "Node4", .value = 40.5};
    pNode1 = &node1;
    pNode2 = &node2;
    pNode3 = &node3;
    pNode4 = &node4;
    printOneNode(pNode1);
    printOneNode(pNode2);
    printOneNode(pNode3);
    printOneNode(pNode4);

    DataNode *a = &node1, **b = &a, p, q;
    q = **b;

    printf("a = %p\n", a);
    printf("*a = %p\n", (void*)a);
    printf("a->x = %d\n", a->index);
    printf("*b = %p\n", *b);
    printf("b = %p\n", b);
    printf("b->x = %d\n", (*b)->index);
    printf("**b = %d\n", (**b).index);
    printOneNode(&q);
};
int testNodeList()
{
    DataList list = createNodeListFromFile();
    if (list.head == NULL)
    {
        printf("Failed to create list from file.\n");
        return -1;
    }
    /*
    printf("Print from head:\n");
    printListFromHead(&list);

    printf("\nPrint from tail:\n");
    printListFromTail(&list);

    find(&list);
    findByIndex(list.head);

    printf("The size of list is %d\n", list.size);
    */
    // testArray();
    // testStruct();
    freeList(&list);
    return 0;
}
#endif

/*
git config --global --get http.proxy
http://127.0.0.1:7890

git config --global --unset http.proxy
git config --global --unset https.proxy
git config --global http.proxy http://127.0.0.1:7890
git config --global https.proxy http://127.0.0.1:7890
git config --global http.proxy 127.0.0.1:29290
git config --global https.proxy 127.0.0.1:29290
*/