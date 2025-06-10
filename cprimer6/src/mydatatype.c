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
#include <time.h>
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
int runThread(void *arg)
{
    const char *threadName = "Thread1";
    const char *threadMsg = (const char *)arg;
    unsigned long threadId = (unsigned long)GetCurrentThreadId();
    printf("Thread %s(%lu): %s\n", threadName, (unsigned long)threadId, threadMsg);
    return 0;
}
int progress[100];
#define BUFFER_SIZE 5    // 缓冲区大小
#define PRODUCE_ITEMS 20 // 生产者生产的总数量
#define CONSUME_ITEMS 8  // 消费者消费的总数量
#define MAX_LINE_LENGTH 256

#define showThrdMsg(msg) printf("%s\n", msg)
// #define showThrdMsg(msg)  logMessage( msg)

// 缓冲区结构
int buffer[BUFFER_SIZE];
int producerId[BUFFER_SIZE];
int in = 0;    // 生产者放入位置
int out = 0;   // 消费者取出位置
int count = 0; // 缓冲区中当前元素数量
bool qStop = false;

// 同步原语
mtx_t mutex;     // 互斥锁，保护缓冲区
cnd_t not_full;  // 条件变量：缓冲区非满时通知生产者
cnd_t not_empty; // 条件变量：缓冲区非空时通知消费者
void showQueue()
{
    char bufferStr[256]; // Buffer to hold the constructed string
    int offset = 0;      // Offset for appending to the buffer

    offset += snprintf(bufferStr + offset, sizeof(bufferStr) - offset,
                       "(Count:%d, Head:%d, Tail:%d)", count, in, out);

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        offset += snprintf(bufferStr + offset, sizeof(bufferStr) - offset,
                           "[pid:%d data:%d] ", producerId[i], buffer[i]);
    }

    showThrdMsg(bufferStr); // Print the complete string
}
int producer(void *arg)
{
    int id = *(int *)arg;
    char logMsg[MAX_LINE_LENGTH];
    for (int i = 0; i < PRODUCE_ITEMS && !qStop; i++)
    {
        // 生成随机数模拟数据
        int item = rand() % 100;
        // 加锁保护缓冲区
        mtx_lock(&mutex);
        // 缓冲区已满，进入等待状态
        while (count == BUFFER_SIZE && !qStop)
        {

            sprintf(logMsg, "Producer %d: Buffer FULL, waiting...", id);
            showThrdMsg(logMsg);
            cnd_wait(&not_full, &mutex); // 释放锁并等待通知
        }
        if (qStop)
        {
            mtx_unlock(&mutex); // 释放锁
            break;
        }
        // 放入数据到缓冲区
        buffer[in] = item;
        producerId[in] = id;

        sprintf(logMsg, "Producer %d(%d/%d): Data[%d] (Pos: %d)", id, i + 1, PRODUCE_ITEMS, item, in);
        showThrdMsg(logMsg);
        in = (in + 1) % BUFFER_SIZE;
        count++;
        showQueue();
        // 通知消费者缓冲区非空
        cnd_signal(&not_empty);
        mtx_unlock(&mutex); // 释放锁

        // 模拟生产耗时
        Sleep(rand() % 200); // 0.2秒内随机延迟
    }

    sprintf(logMsg, "Producer %d: Produce completed.", id);
    showThrdMsg(logMsg);
    return 0;
}

int consumer(void *arg)
{
    int id = *(int *)arg;
    int item;
    char logMsg[MAX_LINE_LENGTH];

    for (int i = 0; i < CONSUME_ITEMS && !qStop; i++)
    {
        // 加锁保护缓冲区
        mtx_lock(&mutex);
        // 等待缓冲区非空
        while (count == 0)
        {
            sprintf(logMsg, "Consumer %d: Buffer EMPTY, waiting...", id);
            cnd_wait(&not_empty, &mutex); // 释放锁并等待通知
        }
        // 从缓冲区取出数据
        item = buffer[out];
        int pId = producerId[out];

        sprintf(logMsg, "Consumer %d(%d/%d): Data [%d] (pos: %d - Producer: %d)", id, i + 1, CONSUME_ITEMS, item, out, pId);
        showThrdMsg(logMsg);
        out = (out + 1) % BUFFER_SIZE;
        count--;

        // showQueue();
        //  通知生产者缓冲区非满
        cnd_signal(&not_full);
        mtx_unlock(&mutex); // 释放锁

        // 模拟消费耗时
        Sleep(rand() % 300); // 0.3秒内随机延迟
    }

    sprintf(logMsg, "Consumer %d: consume completed.", id);
    showThrdMsg(logMsg);
    return item;
}
int checkQueue(void *arg)
{
    int checkNum = 0;
    while (!qStop)
    {
        showQueue();
        Sleep(500);
        checkNum++;
        if (checkNum > 25)
        {
            qStop = true;
            // cnd_signal(&not_full);
            cnd_broadcast(&not_full);
        }
        printf("Check number: %d\n", checkNum);
    }
    showThrdMsg("Check thread completed.\n");
    return 0;
}
int testSimpleThread()
{
    thrd_t thread;
    const char *threadMsg = "Hello from thread!";
    // create a thread
    int result = thrd_create(&thread, runThread, (void *)threadMsg);
    if (result != thrd_success)
    {
        printf("Failed to create thread.\n");
        return -1;
    }
    int threadResult;
    if (thrd_join(thread, &threadResult) != thrd_success)
    {
        printf("Failed to join thread.\n");
        return -1;
    }

    printf("Thread exited with: %d\n", threadResult);
    return 0;
}

int testThread()
{
    testSimpleThread();
    for (int i = 0; i < 100; i++)
    {
        progress[i] = 0;
    }

    srand(time(NULL)); // 初始化随机数种子

    // 初始化同步原语
    mtx_init(&mutex, mtx_plain);
    cnd_init(&not_full);
    cnd_init(&not_empty);

    // 创建生产者和消费者线程
    thrd_t producers[2]; // 2个生产者
    thrd_t consumers[3]; // 3个消费者
    int producer_ids[2] = {1, 2};
    int consumer_ids[3] = {1, 2, 3};

    for (int i = 0; i < 2; i++)
    {
        thrd_create(&producers[i], producer, &producer_ids[i]);
    }
    showThrdMsg("All producer are all startd...");
    for (int i = 0; i < 3; i++)
    {
        thrd_create(&consumers[i], consumer, &consumer_ids[i]);
    }
    showThrdMsg("All consumer are all started...");

    thrd_t checkId;
    thrd_create(checkId, checkQueue, NULL);

    showThrdMsg("The monitor is started...");
    // 等待所有线程结束

    for (int i = 0; i < PRODUCE_ITEMS; i++)
    {
        thrd_join(producers[i], NULL);
    }
    for (int i = 0; i < CONSUME_ITEMS; i++)
    {
        thrd_join(consumers[i], NULL);
    }
    // thrd_join(checkId, NULL);
    showThrdMsg("All tasks are all started...");

    // 销毁同步原语
    cnd_destroy(&not_full);
    cnd_destroy(&not_empty);
    mtx_destroy(&mutex);

    showThrdMsg("All task are completed.");
    return 0;
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
    printf("*a = %p\n", *a);
    printf("a->x = %d\n", a->index);
    printf("*b = %p\n", *b);
    printf("b = %p\n", b);
    printf("b->x = %d\n", (*b)->index);
    printf("**b = %d\n", (**b).index);
    printOneNode(&q);
};

#define BUFFER_SIZE 5
#define TIMEOUT_SECONDS 30 // 3分钟
#define PRODUCER_COUNT 2
#define CONSUMER_COUNT 3

typedef struct
{
    int buffer[BUFFER_SIZE];
    int count;
    int in;
    int out;
    mtx_t mutex;
    cnd_t not_full;
    cnd_t not_empty;
    time_t last_activity; // 记录最后一次活动时间
    bool running;         // 程序运行状态
} ProducerConsumer;

typedef struct
{
    ProducerConsumer *pc;
    int thread_id;
} ThreadArgs;

// 获取当前时间戳（秒）
time_t get_current_time()
{
    return time(NULL);
}

// 初始化缓冲区
void init_buffer(ProducerConsumer *pc)
{
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        pc->buffer[i] = 0;
    }
    pc->count = 0;
    pc->in = 0;
    pc->out = 0;
    pc->last_activity = get_current_time();
    pc->running = true;
    mtx_init(&pc->mutex, mtx_plain);
    cnd_init(&pc->not_full);
    cnd_init(&pc->not_empty);
}

// 生产者放入数据
void produce(ThreadArgs *arg, int item)
{
    ThreadArgs *thread_args = (ThreadArgs *)arg;
    ProducerConsumer *pc = thread_args->pc;
    int id = thread_args->thread_id;

    char logMsg[MAX_LINE_LENGTH];
    mtx_lock(&pc->mutex);

    // 检查程序是否应该继续运行
    while (pc->count == BUFFER_SIZE && pc->running)
    {
        sprintf(logMsg, "Producer %d: Buffer is full. Waiting...", id);
        showThrdMsg(logMsg);
        cnd_wait(&pc->not_full, &pc->mutex);
    }

    if (!pc->running)
    {
        mtx_unlock(&pc->mutex);
        return;
    }

    // 放入数据
    pc->buffer[pc->in] = item;
    pc->count++;
    pc->last_activity = get_current_time(); // 更新活动时间
    sprintf(logMsg, "Producer(%d): [data: %d] (Pos: %d, count=%d)", id, item, pc->in, pc->count);
    showThrdMsg(logMsg);

    pc->in = (pc->in + 1) % BUFFER_SIZE;
    // 广播通知可能有消费者在等待
    cnd_broadcast(&pc->not_empty);

    mtx_unlock(&pc->mutex);
}

// 消费者取出数据
int consume(ThreadArgs *arg)
{
    ThreadArgs *thread_args = (ThreadArgs *)arg;
    ProducerConsumer *pc = thread_args->pc;
    int id = thread_args->thread_id;
    char logMsg[MAX_LINE_LENGTH];

    mtx_lock(&pc->mutex);

    // 检查程序是否应该继续运行
    while (pc->count == 0 && pc->running)
    {
        sprintf(logMsg, "Consumer %d: Buffer is empty. Waiting...", id);
        showThrdMsg(logMsg);
        cnd_wait(&pc->not_empty, &pc->mutex);
    }

    if (!pc->running)
    {
        mtx_unlock(&pc->mutex);
        return -1; // 表示程序已终止
    }

    // 取出数据
    int item = pc->buffer[pc->out];
    pc->out = (pc->out + 1) % BUFFER_SIZE;
    pc->count--;
    pc->last_activity = get_current_time(); // 更新活动时间
    sprintf(logMsg, "Consumer(%d): [data: %d] (Pos: %d, count=%d)", id, item, pc->out, pc->count);
    showThrdMsg(logMsg);

    // 广播通知可能有生产者在等待
    cnd_broadcast(&pc->not_full);

    mtx_unlock(&pc->mutex);
    return item;
}

// 监控线程函数
int monitor_thread(void *arg)
{
    ProducerConsumer *pc = (ProducerConsumer *)arg;

    while (pc->running)
    {
        mtx_lock(&pc->mutex);

        time_t current_time = get_current_time();
        if (current_time - pc->last_activity >= TIMEOUT_SECONDS)
        {
            printf("Timeout: No activity for %d seconds. Terminating...\n", TIMEOUT_SECONDS);
            pc->running = false;

            // 唤醒所有等待的线程
            cnd_broadcast(&pc->not_full);
            cnd_broadcast(&pc->not_empty);
        }

        mtx_unlock(&pc->mutex);

        // 每隔10秒检查一次
        thrd_sleep(&(struct timespec){.tv_sec = 10}, NULL);
    }

    return 0;
}

// 生产者线程函数
int producer_thread(void *arg)
{
    ThreadArgs *thread_args = (ThreadArgs *)arg;
    ProducerConsumer *pc = thread_args->pc;
    int id = thread_args->thread_id;
    ThreadArgs newArg = {.pc = pc, .thread_id = id};

    for (int i = 0; i < PRODUCE_ITEMS && pc->running; i++)
    {
        produce(&newArg, id * 10 + i);
        thrd_sleep(&(struct timespec){.tv_nsec = 100000000}, NULL); // 0.1秒
    }

    printf("Producer %d exiting\n", id);
    return 0;
}

// 消费者线程函数
int consumer_thread(void *arg)
{
    ThreadArgs *thread_args = (ThreadArgs *)arg;
    ProducerConsumer *pc = thread_args->pc;
    int id = thread_args->thread_id;

    while (pc->running)
    {
        int item = consume(arg);
        if (item == -1)
            break; // 程序已终止

        // 使用取出的数据进行处理
        (void)item;
        thrd_sleep(&(struct timespec){.tv_nsec = 800000000}, NULL); // 0.8秒
    }

    printf("Consumer %d exiting\n", id);
    return 0;
}

int testProducerConsumer()
{
    ProducerConsumer pc;
    init_buffer(&pc);

    // 创建线程ID
    int producer_ids[PRODUCER_COUNT];
    for (int i = 0; i < PRODUCER_COUNT; i++)
    {
        producer_ids[i] = i + 1;
    }
    int consumer_ids[CONSUMER_COUNT];
    for (int i = 0; i < CONSUMER_COUNT; i++)
    {
        consumer_ids[i] = i * 10 + 1;
    }

    // 创建线程
    thrd_t producerThreads[PRODUCER_COUNT], consumerThreads[CONSUMER_COUNT], monitor;

    for (int i = 0; i < PRODUCER_COUNT; i++)
    {
        ThreadArgs args = {&pc, producer_ids[i]};
        int result = thrd_create(&producerThreads[i], producer_thread, (void *)&args);
        if (result != thrd_success)
        {
            printf("Failed to create thread.\n");
            return -1;
        }
    }

    for (int i = 0; i < CONSUMER_COUNT; i++)
    {
        ThreadArgs args = {&pc, consumer_ids[i]};

        int result = thrd_create(&consumerThreads[i], consumer_thread, &args);
        if (result != thrd_success)
        {
            printf("Failed to create thread.\n");
            return -1;
        }
    }
    thrd_create(&monitor, monitor_thread, &pc);

    // 等待生产者完成（消费者会被监控线程终止）
    for (int i = 0; i < PRODUCER_COUNT; i++)
    {
        thrd_join(producerThreads[i], NULL);
    }
    // 等待监控线程完成
    thrd_join(monitor, NULL);
    // 等待消费者完成
    for (int i = 0; i < CONSUMER_COUNT; i++)
    {
        thrd_join(consumerThreads[i], NULL);
    }

    // 清理资源
    cnd_destroy(&pc.not_empty);
    cnd_destroy(&pc.not_full);
    mtx_destroy(&pc.mutex);

    printf("Program terminated normally\n");
    return 0;
}
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
    // testThread();
    testProducerConsumer();
    freeList(&list);
    return 0;
}
#endif