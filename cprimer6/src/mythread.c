
#ifndef MYTHREAD_C_GUARD
#define MYTHREAD_C_GUARD
#include "../include/mythread.h"

void init_thread_pool(ThreadPool *pool)
{
    pool->task_count = 0;
    pool->running = true;
    pool->completed_tasks = 0;
    mtx_init(&pool->lock, mtx_plain);
    cnd_init(&pool->notify);

    for (int i = 0; i < TOTAL_TASKS; ++i)
    {
        Task *task = (Task *)malloc(sizeof(Task));
        task->id = i + 1;
        task->status = PENDING;
        task->execute_time = time(NULL); // 初始立即执行
        pool->tasks[i] = task;
    }
}
int worker_thread(void *arg)
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
            mtx_unlock(&pool->lock);

            printf("Thread %lu is executing task %d\n", (unsigned long)GetCurrentThreadId(), next_task->id);
            next_task->status = SUSPENDED; // 执行完成后挂起
            Sleep(TASK_INTERVAL); // 模拟执行耗时

            mtx_lock(&pool->lock);
            next_task->status = COMPLETED; // 执行完成后挂起
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

void start_thread_pool(ThreadPool *pool)
{
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        thrd_create(&pool->workers[i], worker_thread, pool);
    }
}
int monitor_thread(void *arg)
{
    ThreadPool *pool = (ThreadPool *)arg;
    while (pool->running)
    {
        mtx_lock(&pool->lock);
        int suspended = 0, running = 0, pending = 0, completed = 0;

        for (int i = 0; i < TOTAL_TASKS; ++i)
        {
            switch (pool->tasks[i]->status)
            {
            case PENDING:
                pending++;
                break;
            case RUNNING:
                running++;
                break;
            case SUSPENDED:
                suspended++;
                break;
            case COMPLETED:
                completed++;
                break;
            }
        }

        float percent_complete = ((float)completed / TOTAL_TASKS) * 100;
        printf("[Monitor] Running: %d | Suspended: %d | Pending: %d | Completed: %d (%.2f%%)\n",
               running, suspended, pending, completed, percent_complete);

        if (completed == TOTAL_TASKS)
        {
            pool->running = false;
            printf("All tasks completed.\n");
        }

        mtx_unlock(&pool->lock);
        thrd_sleep(&(struct timespec){.tv_sec = 2}, NULL); // 每5秒更新一次
    }
    return 0;
}
int testThreadPool()
{
    ThreadPool pool;
    init_thread_pool(&pool);
    start_thread_pool(&pool);

    thrd_t monitor;
    thrd_create(&monitor, monitor_thread, &pool);

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