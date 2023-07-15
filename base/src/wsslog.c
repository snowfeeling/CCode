
#include <stdio.h>
#include <stdbool.h>

#include "../inc/wsslog.h"

size_t get_current_time( char * time_info)
{
    time_t current_time;
    struct tm *ptime;

    time(&current_time);
    ptime = localtime(&current_time);
    size_t result_num = strftime (time_info, 20,"%Y-%m-%d %H:%M:%S",ptime);
    return result_num;
}

/* ====== 初始化文件，以appendd模式打开 =======*/
bool init_log_file(FILE **fp, char *fname)
{
    (*fp) = fopen(fname, "a");
    return  (*fp ==NULL ) ? false : true;
}

/*====== 把信息写入日志文件 =====
    timeflag = 0 不写日期时间
            = 1 自动写入日期时间
*/
int send_to_log(FILE *fp, char  *str, int timeflag)
{
    char cur_time[20];
    char cur_str[1000];

    if (timeflag)
    {
        get_current_time(cur_time);
        sprintf(cur_str, "[%s] %s", cur_time, str);
    }
    else
    {
        sprintf(cur_str, "%s", str);
    }
    return( fputs(cur_str, fp));
}

int close_log_file(FILE *fp)
{
    return (fclose(fp));
}
