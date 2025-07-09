#ifndef MYTHREAD_C_GUARD
#define MYTHREAD_C_GUARD
#include "../include/mythread.h"

#ifdef _WIN32
#include <windows.h>
typedef DWORD thread_id_t;

thread_id_t getThreadId(void)
{
    return GetCurrentThreadId();
}

const char *format_thread_id(thread_id_t id)
{
    static char buffer[32];
    sprintf(buffer, "%lu", id);
    return buffer;
}

#else
#include <pthread.h>
typedef pthread_t thread_id_t;

thread_id_t getThreadId(void)
{
    return pthread_self();
}

const char *format_thread_id(thread_id_t id)
{
    static char buffer[32];
    sprintf(buffer, "%lu", (unsigned long)id);
    return buffer;
}
#endif
void initThreadPool(ThreadPool *pool)
{
    pool->task_count = 0;
    pool->running = true;
    pool->completed_tasks = 0;
    mtx_init(&pool->lock, mtx_plain);
    cnd_init(&pool->notify);

    for (int i = 0; i < TOTAL_TASKS; ++i)
    {
        Task *task = (Task *)calloc(1, sizeof(Task));
        task->id = i + 1;
        task->status = PENDING;
        task->execute_time = time(NULL); // 初始立即执行
        pool->tasks[i] = task;
    }
}


int workerThread(void *arg)
{
    ThreadPool *pool = (ThreadPool *)arg;
    while (pool->running)
    {
        mtx_lock(&pool->lock);

        // 查找下一个可执行任务
        Task *next_task = NULL;
        for (int i = 0; i < TOTAL_TASKS; ++i)
        {
            if (pool->tasks[i]->status == PENDING && difftime(time(NULL), pool->tasks[i]->execute_time) >= 0)
            {
                next_task = pool->tasks[i];
                next_task->status = RUNNING;
                printf("Thread %lu is executing task %d\n", (unsigned long)getThreadId(), next_task->id);
                // next_task->status = SUSPENDED; // 执行完成后挂起
                mtx_unlock(&pool->lock);
                // 模拟执行耗时3秒钟
                thrd_sleep(&(struct timespec){.tv_sec = TASK_INTERVAL}, NULL);

                mtx_lock(&pool->lock);
                next_task->status = COMPLETED; // 执行后设置为完成
                pool->completed_tasks++;
                mtx_unlock(&pool->lock);
                break;
            }
        }

        if (next_task == NULL)
        {
            mtx_unlock(&pool->lock);
        }
    }
    return 0;
}

void startThreadPool(ThreadPool *pool)
{
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        thrd_create(&pool->workers[i], workerThread, pool);
    }
}
int monitorThreadPool(void *arg)
{
    ThreadPool *pool = (ThreadPool *)arg;
    while (pool->running)
    {
        mtx_lock(&pool->lock);
        int statusCount[4] = {0};

        for (int i = 0; i < TOTAL_TASKS; ++i)
        {
            statusCount[pool->tasks[i]->status]++;
        }

        float percent_complete = ((float)statusCount[COMPLETED] / TOTAL_TASKS) * 100;
        printf("[Monitor] Running: %d | Suspended: %d | Pending: %d | Completed: %d (%.2f%%)\n",
               statusCount[RUNNING], statusCount[SUSPENDED], statusCount[PENDING], statusCount[COMPLETED], percent_complete);

        if (statusCount[COMPLETED] == TOTAL_TASKS)
        {
            // 运行状态改为false，停止！
            pool->running = false;
            printf("All tasks completed.\n");
        }

        mtx_unlock(&pool->lock);
        if (pool->running)
        {
            thrd_sleep(&(struct timespec){.tv_sec = 2}, NULL); // 每2秒监控一次
        }
    }
    return 0;
}

/***********************************************************************************
** 线程例子
************************************************************************************
*/

int runThread(void *arg)
{
    const char *threadName = "Thread1";
    const char *threadMsg = (const char *)arg;
    unsigned long threadId = (unsigned long)getThreadId();
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
    sprintf(logMsg, "%s(%5d-%5lu-%2lu) : %s.", strPC, thrdArg->processId, thrdArg->threadId, thrdArg->thread_id, strStatus);
    if (thrdArg->type == PRODUCER)
        printf("\033[%lu;1H", thrdArg->thread_id + 1);
    if (thrdArg->type == CONSUMER)
        printf("\033[%lu;1H", thrdArg->thread_id + 4 + 1);
    if (thrdArg->type == MONITOR)
        printf("\033[%lu;1H", thrdArg->thread_id + 9 + 1);
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

    mtx_lock(&pc->mutex);

    // 检查程序是否应该继续运行
    while (pc->count == BUFFER_SIZE && pc->running)
    {
        sprintf(logMsg, "Producer(%5d-%5lu-%2d): Buffer is FULL. Waiting...", thrdArg->processId, thrdArg->threadId, id);
        showThrdMsg(logMsg);

        showStatus(arg, WAITING);
        cnd_wait(&pc->not_full, &pc->mutex);
    }

    if (!pc->running)
    {
        mtx_unlock(&pc->mutex);
        return;
    }

    // 数据放入缓冲区中，更新活动时间。
    pc->buffer[pc->in] = item;
    pc->producerId[pc->in] = id;
    pc->count++;
    pc->last_activity = get_current_time(); // 更新活动时间
    sprintf(logMsg, "Producer(%5d-%5lu-%2d): [data: %d] (Pos:%d, count:%d, pID:%d)", thrdArg->processId, thrdArg->threadId, id, item, pc->in, pc->count, id);
    showThrdMsg(logMsg);

    pc->in = (pc->in + 1) % BUFFER_SIZE;
    showStatus(arg, RUNNING);
    // 已有数据放入，广播通知缓冲区非空；可能有消费者在等待，唤醒
    cnd_broadcast(&pc->not_empty);
    mtx_unlock(&pc->mutex);
}

// 消费者取出数据
int consume(ThreadArgs *arg)
{
    ThreadArgs *thrdArg = (ThreadArgs *)arg;
    ProducerConsumer *pc = thrdArg->pc;
    int id = thrdArg->thread_id;
    char logMsg[MAX_LINE_LENGTH];

    mtx_lock(&pc->mutex);

    // 检查数据区为空，且线程未终止
    while (pc->count == 0 && pc->running)
    {
        sprintf(logMsg, "Consumer(%5d-%5lu-%2d): Buffer is EMPTY. Waiting...", thrdArg->processId, thrdArg->threadId, id);
        showThrdMsg(logMsg);
        showStatus(arg, WAITING);
        // 等待非空，同时锁释放
        cnd_wait(&pc->not_empty, &pc->mutex);
    }
    // 如果线程已终止，则释放锁，返回-1
    if (!pc->running)
    {
        mtx_unlock(&pc->mutex);
        return -1; // 表示程序已终止
    }

    // 取出数据，记录更新时间，
    int item = pc->buffer[pc->out];
    pc->count--;
    pc->last_activity = get_current_time(); // 更新活动时间
    sprintf(logMsg, "Consumer(%5d-%5lu-%2d): [data: %d] (Pos:%d, count:%d, pID:%d)", thrdArg->processId, thrdArg->threadId, id, item, pc->out, pc->count, pc->producerId[pc->out]);
    showThrdMsg(logMsg);

    pc->out = (pc->out + 1) % BUFFER_SIZE;
    showStatus(arg, RUNNING);
    // 生产队列有空位置，广播通知，唤醒等待的生产者
    cnd_broadcast(&pc->not_full);
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
    thrdArg->threadId = (unsigned long) getThreadId();

    sprintf(logMsg, "Monitor (%5d-%5lu-%2d): Monitor thread started.", thrdArg->processId, thrdArg->threadId, id);
    showThrdMsg(logMsg);

    while (pc->running)
    {
        mtx_lock(&pc->mutex);

        int n = get_current_time() - pc->last_activity;
        if (n >= TIMEOUT_SECONDS)
        {
            sprintf(logMsg, "Monitor (%5d-%5lu-%2d): Timeout for %d seconds. Terminating...", thrdArg->processId, thrdArg->threadId, id, TIMEOUT_SECONDS);
            showThrdMsg(logMsg);
            pc->running = false;
            showStatus(arg, WAITING);

            // 唤醒所有等待的线程
            cnd_broadcast(&pc->not_full);
            cnd_broadcast(&pc->not_empty);
        }
        else
        {
            showStatus(arg, RUNNING);
            sprintf(logMsg, "Monitor (%5d-%5lu-%2d): Checking the status - no activity in %d seconds.", thrdArg->processId, thrdArg->threadId, id, n);
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
    thrdArg->threadId = (unsigned long) getThreadId();

    sprintf(logMsg, "Producer(%5d-%5lu-%2d): Started.", thrdArg->processId, thrdArg->threadId, id);
    showThrdMsg(logMsg);

    for (int i = 0; i < PRODUCE_ITEMS && pc->running; i++)
    {
        produce(arg, id * 10 + i);
        thrd_sleep(&(struct timespec){.tv_nsec = 1000000000}, NULL); // 0.1秒
    }

    sprintf(logMsg, "Producer(%5d-%5lu-%2d): Exit.", thrdArg->processId, (unsigned long) thrdArg->threadId, id);
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
    thrdArg->threadId = (unsigned long) getThreadId(); // thrd_current();

    sprintf(logMsg, "Consumer(%5d-%5lu-%2d): Started.", thrdArg->processId, (unsigned long) thrdArg->threadId, id);
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

    sprintf(logMsg, "Consumer(%5d-%5lu-%2d): exit.", thrdArg->processId, thrdArg->threadId, id);
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

int testThreadPool()
{
    ThreadPool pool;
    initThreadPool(&pool);
    startThreadPool(&pool);

    thrd_t monitor;
    thrd_create(&monitor, monitorThreadPool, &pool);

    // 等待监控线程结束
    thrd_join(monitor, NULL);
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        thrd_join(pool.workers[i], NULL);
    }

    // 清理资源
    for (int i = 0; i < TOTAL_TASKS; ++i)
    {
        free(pool.tasks[i]);
    }

    return 0;
}

#define PRODUCER_MIN_THREADS 2
#define PRODUCER_MAX_THREADS 10
#define CONSUMER_MIN_THREADS 2
#define CONSUMER_MAX_THREADS 10
#define IDLE_TIMEOUT_MS 2000     // 空闲超时时间(毫秒)
#define MONITOR_INTERVAL_MS 1000 // 监控间隔

// 数据池结构
typedef struct
{
    int *buffer;     // 数据缓冲区
    size_t capacity; // 容量
    size_t count;    // 当前数据量
    size_t front;    // 队列头
    size_t rear;     // 队列尾
    mtx_t lock;      // 互斥锁
    cnd_t not_empty; // 非空条件
    cnd_t not_full;  // 非满条件
} DataPool;

// 线程池结构
typedef struct
{
    thrd_t *threads;      // 线程数组
    size_t count;         // 当前线程数
    size_t active;        // 活动线程数
    size_t min_threads;   // 最小线程数
    size_t max_threads;   // 最大线程数
    mtx_t lock;           // 互斥锁
    cnd_t work_available; // 工作条件
    bool shutdown;        // 关闭标志
    bool has_work;        // 新增：是否有工作标志
    const char *name;     // 线程池名称
} MulityThreadPool;

// 全局变量
DataPool data_pool;
MulityThreadPool producer_pool;
MulityThreadPool consumer_pool;

// 初始化数据池
void data_pool_init(DataPool *pool, size_t capacity)
{
    pool->buffer = malloc(capacity * sizeof(int));
    pool->capacity = capacity;
    pool->count = 0;
    pool->front = 0;
    pool->rear = 0;
    mtx_init(&pool->lock, mtx_plain);
    cnd_init(&pool->not_empty);
    cnd_init(&pool->not_full);
}

// 销毁数据池
void data_pool_destroy(DataPool *pool)
{
    mtx_destroy(&pool->lock);
    cnd_destroy(&pool->not_empty);
    cnd_destroy(&pool->not_full);
    free(pool->buffer);
}

// 向数据池添加数据
void data_pool_put(DataPool *pool, int value)
{
    mtx_lock(&pool->lock);

    // 等待空间可用
    while (pool->count == pool->capacity)
    {
        cnd_wait(&pool->not_full, &pool->lock);
    }

    // 添加数据
    pool->buffer[pool->rear] = value;
    pool->rear = (pool->rear + 1) % pool->capacity;
    pool->count++;

    // 通知消费者
    cnd_signal(&pool->not_empty);
    mtx_unlock(&pool->lock);
}

// 从数据池获取数据
int data_pool_get(DataPool *pool)
{
    mtx_lock(&pool->lock);

    // 等待数据可用
    while (pool->count == 0)
    {
        cnd_wait(&pool->not_empty, &pool->lock);
    }

    // 获取数据
    int value = pool->buffer[pool->front];
    pool->front = (pool->front + 1) % pool->capacity;
    pool->count--;

    // 通知生产者
    cnd_signal(&pool->not_full);
    mtx_unlock(&pool->lock);

    return value;
}

// 生产者任务
int producer_task(void *arg)
{
    MulityThreadPool *pool = (MulityThreadPool *)arg;

    while (true)
    {
        mtx_lock(&pool->lock);

        // 等待工作或关闭信号
        struct timespec ts;
        timespec_get(&ts, TIME_UTC);
        ts.tv_sec += IDLE_TIMEOUT_MS / 1000;
        ts.tv_nsec += (IDLE_TIMEOUT_MS % 1000) * 1000000;

        while (!pool->shutdown && !pool->has_work)
        {
            if (cnd_timedwait(&pool->work_available, &pool->lock, &ts) == thrd_timedout)
            {
                // 空闲超时且线程数大于最小值
                if (pool->count > pool->min_threads)
                {
                    pool->count--;
                    mtx_unlock(&pool->lock);
                    thrd_exit(0);
                }
            }
        }

        if (pool->shutdown)
        {
            mtx_unlock(&pool->lock);
            break;
        }

        pool->active++;
        mtx_unlock(&pool->lock);

        // 生产数据 (模拟工作)
        int value = rand() % 100;
        data_pool_put(&data_pool, value);

        mtx_lock(&pool->lock);
        pool->has_work = false;
        pool->active--;
        mtx_unlock(&pool->lock);
    }
    return 0;
}

// 消费者任务
int consumer_task(void *arg)
{
    MulityThreadPool *pool = (MulityThreadPool *)arg;

    while (true)
    {
        int value = data_pool_get(&data_pool);

        // 消费数据 (模拟工作)
        printf("Consumed: %d\n", value);
        thrd_sleep(&(struct timespec){.tv_sec = 0, .tv_nsec = 50000000}, NULL); // 50ms

        // 动态缩减检查
        mtx_lock(&pool->lock);
        if (pool->count > pool->min_threads)
        {
            struct timespec now;
            timespec_get(&now, TIME_UTC);

            // 空闲超时检查
            if (pool->active == 0)
            {
                pool->count--;
                mtx_unlock(&pool->lock);
                thrd_exit(0);
            }
        }
        mtx_unlock(&pool->lock);
    }
    return 0;
}

// 初始化线程池
void thread_pool_init(MulityThreadPool *pool, size_t min, size_t max, const char *name, thrd_start_t func)
{
    pool->min_threads = min;
    pool->max_threads = max;
    pool->count = min;
    pool->active = 0;
    pool->shutdown = false;
    pool->has_work = false; // 初始化为无工作状态
    pool->name = name;

    mtx_init(&pool->lock, mtx_plain);
    cnd_init(&pool->work_available);

    pool->threads = malloc(max * sizeof(thrd_t));
    for (size_t i = 0; i < min; i++)
    {
        thrd_create(&pool->threads[i], func, pool);
    }
}

// 动态扩展线程池
void thread_pool_expand(MulityThreadPool *pool, thrd_start_t func)
{
    mtx_lock(&pool->lock);
    if (pool->count < pool->max_threads)
    {
        thrd_create(&pool->threads[pool->count], func, pool);
        pool->count++;
        printf("%s expanded to %zu threads\n", pool->name, pool->count);
    }
    mtx_unlock(&pool->lock);
}

// 监控线程
int monitor_task(void *arg)
{
    (void)arg;

    while (true)
    {
        thrd_sleep(&(struct timespec){.tv_sec = 0, .tv_nsec = MONITOR_INTERVAL_MS * 1000000}, NULL);

        mtx_lock(&producer_pool.lock);
        size_t prod_count = producer_pool.count;
        size_t prod_active = producer_pool.active;
        mtx_unlock(&producer_pool.lock);

        mtx_lock(&consumer_pool.lock);
        size_t cons_count = consumer_pool.count;
        size_t cons_active = consumer_pool.active;
        mtx_unlock(&consumer_pool.lock);

        printf("[MONITOR] Producers: %zu/%zu active | Consumers: %zu/%zu active\n",
               prod_active, prod_count, cons_active, cons_count);

        // 动态扩展检查
        if (prod_active == prod_count && prod_count < producer_pool.max_threads)
        {
            thread_pool_expand(&producer_pool, producer_task);
        }

        if (cons_active == cons_count && cons_count < consumer_pool.max_threads)
        {
            thread_pool_expand(&consumer_pool, consumer_task);
        }
    }
    return 0;
}

int testMultiplyPool(void)
{
    srand(time(NULL));

    // 初始化数据池 (容量10)
    data_pool_init(&data_pool, 10);

    // 初始化线程池
    thread_pool_init(&producer_pool, PRODUCER_MIN_THREADS, PRODUCER_MAX_THREADS, "Producer", producer_task);
    thread_pool_init(&consumer_pool, CONSUMER_MIN_THREADS, CONSUMER_MAX_THREADS, "Consumer", consumer_task);

    // 创建监控线程
    thrd_t monitor_thread;
    thrd_create(&monitor_thread, monitor_task, NULL);

    // 主线程模拟任务提交
    for (int i = 0; i < 500; i++)
    {
        mtx_lock(&producer_pool.lock);
        producer_pool.has_work = true;             // 设置工作标志
        cnd_signal(&producer_pool.work_available); // 通知生产者工作
        mtx_unlock(&producer_pool.lock);
        thrd_sleep(&(struct timespec){.tv_sec = 0, .tv_nsec = 800000000}, NULL); // 100ms
    }

    // 等待退出
    thrd_sleep(&(struct timespec){.tv_sec = 5, .tv_nsec = 0}, NULL);
    return 0;
}
void testThread()
{
    //testProducerConsumer();
    //testThreadPool();
    testMultiplyPool();
}
#endif