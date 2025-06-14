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
    printf("*a = %p\n", *a);
    printf("a->x = %d\n", a->index);
    printf("*b = %p\n", *b);
    printf("b = %p\n", b);
    printf("b->x = %d\n", (*b)->index);
    printf("**b = %d\n", (**b).index);
    printOneNode(&q);
};
/***********************************************************************************
** 线程例子
************************************************************************************
*/
#define BUFFER_SIZE 8    // 生产产品的缓冲区大小
#define PRODUCE_ITEMS 20 // 每个生产者生产的总数量
#define CONSUME_ITEMS 8  // 每个消费者消费的总数量
#define MAX_LINE_LENGTH 256

#define TIMEOUT_SECONDS 30 // 30秒-如果有空闲就停止
#define PRODUCER_COUNT 3   // 生产者线程数量
#define CONSUMER_COUNT 4   // 消费者线程数量
#define MONITOR_COUNT 3    // 监控线程数量

// #define showThrdMsg(msg) printf("%s\n", msg)
#define showThrdMsg(msg) logMessage(msg)
int runThread(void *arg)
{
    const char *threadName = "Thread1";
    const char *threadMsg = (const char *)arg;
    unsigned long threadId = (unsigned long)GetCurrentThreadId();
    printf("Thread %s(%lu): %s\n", threadName, (unsigned long)threadId, threadMsg);
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

typedef enum
{
    PRODUCER,
    CONSUMER,
    MONITOR
} ThreadType;
typedef enum
{
    RUNNING,
    WAITING,
    STOPPED

} ThreadStatus;
typedef struct
{
    int buffer[BUFFER_SIZE];
    int producerId[BUFFER_SIZE];
    int count;
    int in;
    int out;

    mtx_t mutex;
    cnd_t not_full;
    cnd_t not_empty;
    time_t last_activity; // 记录最后一次活动时间
    bool running;         // 程序运行状态
    mtx_t showMutex;
} ProducerConsumer;

typedef struct
{
    ProducerConsumer *pc;
    int thread_id;
    ThreadType type;
    ThreadStatus status;
    pid_t processId;
    DWORD threadId;
    thrd_t thread;
} ThreadArgs;
// 获取当前时间戳（秒）
time_t get_current_time()
{
    return time(NULL);
}
#define CLEAR_SCREEN() printf("\033[2J\033[H")
#define GET_THREAD_TYPE_STRING(t) (t == PRODUCER ? "Producer" : (t == CONSUMER ? "Consumer" : "Monitor "))
#define GET_THREAD_STATUS_STRING(s) (s == WAITING ? "WAITING" : (s == RUNNING ? "RUNNING" : "STOPPED"))
int showStatus(ThreadArgs *arg, ThreadStatus status)
{
    ThreadArgs *thrdArg = (ThreadArgs *)arg;
    ProducerConsumer *pc = thrdArg->pc;
    int id = thrdArg->thread_id;
    thrdArg->status = status;
    char logMsg[MAX_LINE_LENGTH];

    mtx_lock(&pc->showMutex);

    sprintf(logMsg, "[Queue Length:%d, Head:%d, Tail:%d]", pc->count, pc->in, pc->out);
    printf("\033[%d;1H", 1);
    printf("\r%s", logMsg);
    char strPC[MAX_LINE_LENGTH], strStatus[MAX_LINE_LENGTH];
    sprintf(strPC, "Thread %d: %s", id, GET_THREAD_TYPE_STRING(thrdArg->type));
    sprintf(strStatus, "%s", GET_THREAD_STATUS_STRING(thrdArg->status));
    sprintf(logMsg, "%s(%5d-%5d-%2d) : %s.", strPC, thrdArg->processId, thrdArg->threadId, thrdArg->thread_id, strStatus);
    if (thrdArg->type == PRODUCER)
        printf("\033[%d;1H", thrdArg->thread_id + 1);
    if (thrdArg->type == CONSUMER)
        printf("\033[%d;1H", thrdArg->thread_id + 4 + 1);
    if (thrdArg->type == MONITOR)
        printf("\033[%d;1H", thrdArg->thread_id + 9 + 1);
    printf("\r%s", logMsg);
    mtx_unlock(&pc->showMutex);
    return 0;
}

// 初始化缓冲区
void init_buffer(ProducerConsumer *pc)
{
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        pc->buffer[i] = 0;
        pc->producerId[i] = 0;
    }
    pc->count = 0;
    pc->in = 0;
    pc->out = 0;
    pc->last_activity = get_current_time();
    pc->running = true;
    mtx_init(&pc->mutex, mtx_plain);
    cnd_init(&pc->not_full);
    cnd_init(&pc->not_empty);
    mtx_init(&pc->showMutex, mtx_plain);
}
void destroy_buffer(ProducerConsumer *pc)
{
    mtx_destroy(&pc->mutex);
    cnd_destroy(&pc->not_full);
    cnd_destroy(&pc->not_empty);
    mtx_destroy(&pc->showMutex);
}
// 生产者放入数据
void produce(ThreadArgs *arg, int item)
{
    ThreadArgs *thrdArg = (ThreadArgs *)arg;
    ProducerConsumer *pc = thrdArg->pc;
    int id = thrdArg->thread_id;
    char logMsg[MAX_LINE_LENGTH];
    thrdArg->processId = getpid();
    thrdArg->threadId = GetCurrentThreadId(); // thrd_current();

    mtx_lock(&pc->mutex);

    // 检查程序是否应该继续运行
    while (pc->count == BUFFER_SIZE && pc->running)
    {
        sprintf(logMsg, "Producer(%5d-%5d-%2d): Buffer is FULL. Waiting...", thrdArg->processId, thrdArg->threadId, id);
        showThrdMsg(logMsg);

        showStatus(arg, WAITING);
        cnd_wait(&pc->not_full, &pc->mutex);
    }

    if (!pc->running)
    {
        mtx_unlock(&pc->mutex);
        return;
    }

    // 放入数据
    pc->buffer[pc->in] = item;
    pc->producerId[pc->in] = id;
    pc->count++;
    pc->last_activity = get_current_time(); // 更新活动时间
    sprintf(logMsg, "Producer(%5d-%5d-%2d): [data: %d] (Pos:%d, count:%d, pID:%d)", thrdArg->processId, thrdArg->threadId, id, item, pc->in, pc->count, id);
    showThrdMsg(logMsg);

    pc->in = (pc->in + 1) % BUFFER_SIZE;
    showStatus(arg, RUNNING);
    // 广播通知可能有消费者在等待
    cnd_broadcast(&pc->not_empty);
    // cnd_signal(&pc->not_empty);
    mtx_unlock(&pc->mutex);
}

// 消费者取出数据
int consume(ThreadArgs *arg)
{
    ThreadArgs *thrdArg = (ThreadArgs *)arg;
    ProducerConsumer *pc = thrdArg->pc;
    int id = thrdArg->thread_id;
    char logMsg[MAX_LINE_LENGTH];
    thrdArg->processId = getpid();
    thrdArg->threadId = GetCurrentThreadId();

    mtx_lock(&pc->mutex);

    // 检查程序是否应该继续运行
    while (pc->count == 0 && pc->running)
    {
        sprintf(logMsg, "Consumer(%5d-%5d-%2d): Buffer is EMPTY. Waiting...", thrdArg->processId, thrdArg->threadId, id);
        showThrdMsg(logMsg);
        showStatus(arg, WAITING);
        cnd_wait(&pc->not_empty, &pc->mutex);
    }

    if (!pc->running)
    {
        mtx_unlock(&pc->mutex);
        return -1; // 表示程序已终止
    }

    // 取出数据
    int item = pc->buffer[pc->out];
    pc->count--;
    pc->last_activity = get_current_time(); // 更新活动时间
    sprintf(logMsg, "Consumer(%5d-%5d-%2d): [data: %d] (Pos:%d, count:%d, pID:%d)", thrdArg->processId, thrdArg->threadId, id, item, pc->out, pc->count, pc->producerId[pc->out]);
    showThrdMsg(logMsg);

    pc->out = (pc->out + 1) % BUFFER_SIZE;
    showStatus(arg, RUNNING);
    // 广播通知可能有生产者在等待
    cnd_broadcast(&pc->not_full);
    // cnd_signal(&pc->not_full);

    mtx_unlock(&pc->mutex);
    return item;
}

// 监控线程函数
int monitorThread(void *arg)
{
    ThreadArgs *thrdArg = (ThreadArgs *)arg;
    ProducerConsumer *pc = thrdArg->pc;
    int id = thrdArg->thread_id;
    char logMsg[MAX_LINE_LENGTH];
    thrdArg->processId = getpid();
    thrdArg->threadId = GetCurrentThreadId();
    sprintf(logMsg, "Monitor (%5d-%5d-%2d): Monitor thread started.", thrdArg->processId, thrdArg->threadId, id);
    showThrdMsg(logMsg);

    while (pc->running)
    {
        mtx_lock(&pc->mutex);

        int n = get_current_time() - pc->last_activity;
        if (n >= TIMEOUT_SECONDS)
        {
            sprintf(logMsg, "Monitor (%5d-%5d-%2d): Timeout for %d seconds. Terminating...", thrdArg->processId, thrdArg->threadId, id, TIMEOUT_SECONDS);
            showThrdMsg(logMsg);
            pc->running = false;
            // 唤醒所有等待的线程
            cnd_broadcast(&pc->not_full);
            cnd_broadcast(&pc->not_empty);
        }
        else
        {
            showStatus(arg, RUNNING);
            sprintf(logMsg, "Monitor (%5d-%5d-%2d): Checking the status - no activity in %d seconds.", thrdArg->processId, thrdArg->threadId, id, n);
            showThrdMsg(logMsg);
        }

        mtx_unlock(&pc->mutex);

        // 每隔10秒检查一次
        if (pc->running)
            thrd_sleep(&(struct timespec){.tv_sec = 10}, NULL);
    }

    showStatus(arg, STOPPED);
    return 0;
}

// 生产者线程函数
int producerThread(void *arg)
{
    ThreadArgs *thrdArg = (ThreadArgs *)arg;
    ProducerConsumer *pc = thrdArg->pc;
    int id = thrdArg->thread_id;
    char logMsg[MAX_LINE_LENGTH];
    thrdArg->processId = getpid();
    thrdArg->threadId = GetCurrentThreadId(); // thrd_current();
    sprintf(logMsg, "Producer(%5d-%5d-%2d): Started.", thrdArg->processId, thrdArg->threadId, id);
    showThrdMsg(logMsg);

    for (int i = 0; i < PRODUCE_ITEMS && pc->running; i++)
    {
        produce(arg, id * 10 + i);
        thrd_sleep(&(struct timespec){.tv_nsec = 1000000000}, NULL); // 0.1秒
    }

    sprintf(logMsg, "Producer(%5d-%5d-%2d): Exit.", thrdArg->processId, thrdArg->threadId, id);
    showThrdMsg(logMsg);
    showStatus(arg, STOPPED);
    return 0;
}

// 消费者线程函数
int consumerThread(void *arg)
{
    ThreadArgs *thrdArg = (ThreadArgs *)arg;
    ProducerConsumer *pc = thrdArg->pc;
    int id = thrdArg->thread_id;
    char logMsg[MAX_LINE_LENGTH];
    thrdArg->processId = getpid();
    thrdArg->threadId = GetCurrentThreadId(); // thrd_current();
    sprintf(logMsg, "Consumer(%5d-%5d-%2d): Started.", thrdArg->processId, thrdArg->threadId, id);
    showThrdMsg(logMsg);

    while (pc->running)
    {
        int item = consume(arg);
        if (item == -1)
            break; // 程序已终止

        // 使用取出的数据进行处理
        //(void)item;
        showStatus(arg, RUNNING);
        thrd_sleep(&(struct timespec){.tv_nsec = 800000000}, NULL); // 0.8秒
    }

    sprintf(logMsg, "Consumer(%5d-%5d-%2d): exit.", thrdArg->processId, thrdArg->threadId, id);
    showThrdMsg(logMsg);

    showStatus(arg, STOPPED);
    return 0;
}

int testProducerConsumer()
{
    ProducerConsumer pc;
    ThreadArgs producerArgs[PRODUCER_COUNT], consumerArgs[CONSUMER_COUNT], monitorArgs[MONITOR_COUNT];

    init_buffer(&pc);
    CLEAR_SCREEN();
    // 创建线程
    for (int i = 0; i < PRODUCER_COUNT; i++)
    {
        producerArgs[i] = (ThreadArgs){.pc = &pc, .thread_id = i + 1, .type = PRODUCER};
        int result = thrd_create(&producerArgs[i].thread, producerThread, (void *)&producerArgs[i]);
        if (result != thrd_success)
        {
            printf("Failed to create thread.\n");
            return -1;
        }
    }
    for (int i = 0; i < CONSUMER_COUNT; i++)
    {
        consumerArgs[i] = (ThreadArgs){.pc = &pc, .thread_id = i + 1, .type = CONSUMER};
        int result = thrd_create(&consumerArgs[i].thread, consumerThread, &consumerArgs[i]);
        if (result != thrd_success)
        {
            printf("Failed to create thread.\n");
            return -1;
        }
    }
    for (int i = 0; i < MONITOR_COUNT; i++)
    {
        monitorArgs[i] = (ThreadArgs){.pc = &pc, .thread_id = i + 1, .type = MONITOR};
        int result = thrd_create(&monitorArgs[i].thread, monitorThread, &monitorArgs[i]);
        if (result != thrd_success)
        {
            printf("Failed to create thread.\n");
        }
    }

    // 等待生产者完成（消费者会被监控线程终止）
    for (int i = 0; i < PRODUCER_COUNT; i++)
    {
        thrd_join(producerArgs[i].thread, NULL);
    }
    // 等待监控线程完成
    thrd_join(monitorArgs[0].thread, NULL);
    // 等待消费者完成
    for (int i = 0; i < CONSUMER_COUNT; i++)
    {
        thrd_join(consumerArgs[i].thread, NULL);
    }

    // 清理资源
    destroy_buffer(&pc);

    showThrdMsg("Program terminated normally.");
    printf("\033[15;1H");
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