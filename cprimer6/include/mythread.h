#ifndef MYTHREAD_H
#define MYTHREAD_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "tinycthread.h"

#define NUM_THREADS 10
#define TOTAL_TASKS 200
#define TASK_INTERVAL 3000 // 每3秒执行一个任务

typedef enum
{
    PENDING,
    RUNNING,
    SUSPENDED,
    COMPLETED
} TaskStatus;

typedef struct Task
{
    int id;
    TaskStatus status;
    time_t execute_time; // 下一次执行时间
} Task;

typedef struct ThreadPool
{
    thrd_t workers[NUM_THREADS];
    Task *tasks[TOTAL_TASKS];
    int task_count;
    mtx_t lock;
    cnd_t notify;
    bool running;
    int completed_tasks;
} ThreadPool;

int testThreadPool();

#endif