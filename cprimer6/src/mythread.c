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
                break;
            }
        }

        if (next_task != NULL)
        {
            printf("Thread %lu is executing task %d\n", (unsigned long)GetCurrentThreadId(), next_task->id);
            next_task->status = SUSPENDED; // 执行完成后挂起
            mtx_unlock(&pool->lock);

            Sleep(TASK_INTERVAL); // 模拟执行耗时

            mtx_lock(&pool->lock);
            next_task->status = COMPLETED; // 执行后设置为完成
            pool->completed_tasks++;
            mtx_unlock(&pool->lock);
        }
        else
        {
            mtx_unlock(&pool->lock);
            thrd_sleep(&(struct timespec){.tv_sec = 1}, NULL);
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
int testThreadPool()
{
    ThreadPool pool;
    initThreadPool(&pool);
    startThreadPool(&pool);

    thrd_t monitor;
    thrd_create(&monitor, monitorThreadPool, &pool);

    // 等待监控线程结束
    thrd_join(monitor, NULL);

    // 清理资源
    for (int i = 0; i < TOTAL_TASKS; ++i)
    {
        free(pool.tasks[i]);
    }

    return 0;
}

#endif