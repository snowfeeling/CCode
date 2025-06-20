#ifndef MYTHREAD_C_GUARD
#define MYTHREAD_C_GUARD
#include "../include/mythread.h"

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
                printf("Thread %lu is executing task %d\n", (unsigned long)GetCurrentThreadId(), next_task->id);
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

    // 数据放入缓冲区中，更新活动时间。
    pc->buffer[pc->in] = item;
    pc->producerId[pc->in] = id;
    pc->count++;
    pc->last_activity = get_current_time(); // 更新活动时间
    sprintf(logMsg, "Producer(%5d-%5d-%2d): [data: %d] (Pos:%d, count:%d, pID:%d)", thrdArg->processId, thrdArg->threadId, id, item, pc->in, pc->count, id);
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
        sprintf(logMsg, "Consumer(%5d-%5d-%2d): Buffer is EMPTY. Waiting...", thrdArg->processId, thrdArg->threadId, id);
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
    sprintf(logMsg, "Consumer(%5d-%5d-%2d): [data: %d] (Pos:%d, count:%d, pID:%d)", thrdArg->processId, thrdArg->threadId, id, item, pc->out, pc->count, pc->producerId[pc->out]);
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
            showStatus(arg, WAITING);

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
    thrdArg->threadId = GetCurrentThreadId();

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

void testThread()
{
    testProducerConsumer();
    testThreadPool();
}
#endif