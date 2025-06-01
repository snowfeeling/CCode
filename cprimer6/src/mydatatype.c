#ifndef MYDATATYPE_C_GUARD
#define MYDATATYPE_C_GUARD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
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
    typeof(ch) nch;

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
    PRINT_TYPE_INFO(nch, "typeof() declare char");
}

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
typedef int (*MathOperation)(int, int);

int testFuncPtr()
{

    MathOperation *mathOperation;
    mathOperation = (MathOperation *)malloc(4 * sizeof(MathOperation));

    mathOperation[0] = &add;
    mathOperation[1] = &subtract;
    mathOperation[2] = &multiply;
    mathOperation[3] = &divide;
    int a = 10;
    int b = 2;
    for (int i = 0; i < 4; i++)
    {
        int result = mathOperation[i](a, b);
        switch (i)
        {
        case 0:
        case 1:
        case 2:
        case 3:
            printf("%d %c %d = %d\n", a, "+-*/"[i], b, result);
            break;
        default:
            break;
        }
    }
    free(mathOperation);
}

DataNode *createNode(int index, const char *name, double value)
{
    DataNode *newNode = (DataNode *)malloc(sizeof(DataNode));
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
    }
    else
    {
        list->tail->next = newNode;
        newNode->prev = list->tail;
        list->tail = newNode;
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
    DataNode *current = list->head;
    while (current != NULL)
    {
        printOneNode(current);
        current = current->next;
    }
}

void printListFromTail(const DataList *list)
{
    DataNode *current = list->tail;
    while (current != NULL)
    {
        printOneNode(current);
        current = current->prev;
    }
}
// 比较 index
bool compareIndex(DataNode *node, void *target)
{
    int targetIndex = *(int *)target;
    return node->index == targetIndex;
}

// 比较 value（使用精度判断）
bool compareValue(DataNode *node, void *target)
{
    double targetValue = *(double *)target;
    return fabs(node->value - targetValue) < 1e-6;
}

// 比较 name
bool compareName(DataNode *node, void *target)
{
    const char *targetName = (const char *)target;
    return strcmp(node->name, targetName) == 0;
}
DataNodeMatchResult findNodesByCriteria(CriteriaNode *criteriaNode)
{
    DataNode *current = criteriaNode->head;
    void *target = criteriaNode->target;
    CompareFunc compare = criteriaNode->compareFunc;

    int capacity = 4;
    int count = 0;

    DataNode **resultArray = (DataNode **)malloc(capacity * sizeof(DataNode *));
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
DataNode *findNodeByIndex(DataNode *head, int targetIndex)
{
    DataNode *current = head;
    while (current != NULL)
    {
        if (current->index == targetIndex)
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

void find(DataList *list)
{
    int searchIndex = 19;
    CriteriaNode criteriaNode;
    criteriaNode.head = list->head;
    criteriaNode.target = &searchIndex;
    criteriaNode.compareFunc = compareIndex;

    DataNodeMatchResult result = findNodesByCriteria(&criteriaNode);
    printf("Found %d node(s) with Index %d:\n", result.count, searchIndex);
    for (int i = 0; i < result.count; i++)
    {
        printOneNode(result.nodes[i]);
    }
    freeNodeMatchResult(&result);

    // 查找 value == 90 的节点
    double searchValue = 190.0;
    criteriaNode.head = list->head;
    criteriaNode.target = &searchValue;
    criteriaNode.compareFunc = compareValue;
    DataNodeMatchResult result1 = findNodesByCriteria(&criteriaNode);
    printf("Found %d nodes with Value %.2f\n", result1.count, searchValue);
    for (int i = 0; i < result1.count; i++)
    {
        printOneNode(result1.nodes[i]);
    }
    freeNodeMatchResult(&result1);

    // 查找 name == "zhaoliu"
    const char *searchName = "zhaowu";
    criteriaNode.head = list->head;
    criteriaNode.target = (void *)searchName;
    criteriaNode.compareFunc = compareName;
    DataNodeMatchResult result2 = findNodesByCriteria(&criteriaNode);
    printf("Found %d nodes with Name '%s'\n", result2.count, searchName);
    for (int i = 0; i < result2.count; i++)
    {
        printOneNode(result2.nodes[i]);
    }
    freeNodeMatchResult(&result2);
}
void findByIndex(DataNode *head)
{
    int searchIndex = 2;
    DataNode *result = findNodeByIndex(head, searchIndex);
    if (result != NULL)
    {
        printf("Found node with ");
        printOneNode(result);
    }
    else
    {
        printf("No node found with Index %d\n", searchIndex);
    }
}
int testNodeList()
{
    DataList list = createNodeListFromFile();
    if (list.head == NULL)
    {
        printf("Failed to create list from file.\n");
        return -1;
    }

    printf("Print from head:\n");
    printListFromHead(&list);

    printf("\nPrint from tail:\n");
    printListFromTail(&list);

    find(&list); 
    findByIndex(list.head);

    freeList(&list);
    return 0;
}
#endif