
#ifndef MYLOG_C_GUARD
#define MYLOG_C_GUARD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/mylog.h"

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

// 新增函数：记录程序日志
void logMessage(const char *message)
{
    FILE *logFile = fopen("../log/program.log", "a"); // 以追加模式打开日志文件
    if (logFile == NULL)
    {
        perror("Error opening log file");
        return;
    }

    char *timestamp = getCurrentTimestamp();
    fprintf(logFile, "[%s] %s\n", timestamp, message); // 写入日志信息
    fclose(logFile);                                   // 关闭文件
}

#endif