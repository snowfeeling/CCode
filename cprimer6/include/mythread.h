#pragma once

#ifndef MYTHREAD_H
#define MYTHREAD_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>

#include "tinycthread.h"
#include "mybase.h"
#include "mylog.h"

#if defined(_WIN32)
typedef DWORD my_thread_t;
typedef struct thread_attr
{
    DWORD dwStackSize;
    int detachstate;
} my_thread_attr_t;
#else
typedef pthread_t my_thread_t;
typedef pthread_attr_t my_thread_attr_t;
#endif

#define NUM_THREADS 10
#define TOTAL_TASKS 100
#define TASK_INTERVAL 3  // 每3秒执行一个任务
#define BUFFER_SIZE 8    // 生产产品的缓冲区大小
#define PRODUCE_ITEMS 20 // 每个生产者生产的总数量
#define CONSUME_ITEMS 8  // 每个消费者消费的总数量

#ifndef MAX_LINE_LENGTH
#define MAX_LINE_LENGTH 256
#endif

#define TIMEOUT_SECONDS 30 // 30秒-如果有空闲就停止
#define PRODUCER_COUNT 3   // 生产者线程数量
#define CONSUMER_COUNT 4   // 消费者线程数量
#define MONITOR_COUNT 3    // 监控线程数量

// #define showThrdMsg(msg) printf("%s\n", msg)
#define showThrdMsg(msg) logMessage(msg)



typedef enum
{
    PENDING,
    RUNNING,
    SUSPENDED,
    COMPLETED,
    WAITING,
    STOPPED
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
typedef enum
{
    PRODUCER,
    CONSUMER,
    MONITOR
} ThreadType;
typedef TaskStatus ThreadStatus;
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
    unsigned long thread_id;
    ThreadType type;
    ThreadStatus status;
    pid_t processId;
    unsigned long threadId;
    thrd_t thread;
} ThreadArgs;

void testThread();

#endif