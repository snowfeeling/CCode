
#ifndef MYLOG_C_GUARD
#define MYLOG_C_GUARD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/mylog.h"

FILE *logFile;
mtx_t mutex;

// 新增函数：获取当前时间戳，精确到毫秒
char *getCurrentTimestamp()
{
    static char timestamp[30];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    // 获取毫秒部分
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    sprintf(timestamp + strlen(timestamp), ".%03ld", ts.tv_nsec / 1000000);

    return timestamp;
}

int initLogFile()
{
    logFile = fopen("../log/program.log", "w"); // 以写入模式打开日志文件
    if (logFile == NULL)
    {
        perror("Error opening log file");
        return -1;
    }
    mtx_init(&mutex, mtx_plain);
    return 0;
}
// 新增函数：记录程序日志
void logMessage(const char *message)
{
    mtx_lock(&mutex);
    char *timestamp = getCurrentTimestamp();
    fprintf(logFile, "[%s] %s\n", timestamp, message); // 写入日志信息
    fflush(logFile);
    mtx_unlock(&mutex);
}

void closeLogFile()
{
    mtx_destroy(&mutex);
    fclose(logFile); // 关闭文件
}
#endif