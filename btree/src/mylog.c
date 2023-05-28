#include <stdio.h>
#include <stdbool.h>

#include "../inc/mylog.h"

size_t get_current_time( char * time_info)
{
    time_t current_time;
    struct tm *ptime;

    time(&current_time);
    ptime = localtime(&current_time);
    size_t result_num = strftime (time_info, 20,"%Y-%m-%d %H:%M:%S",ptime);
    return result_num;
}

bool init_log_file(FILE **fp, char *fname)
{
    (*fp) = fopen(fname, "a");
    return  (*fp ==NULL ) ? false : true;
}

int send_to_log(FILE *fp, char  *str)
{
    char cur_time[20];
    get_current_time(cur_time);
    char cur_str[1000];
    sprintf(cur_str, "[%s] %s", cur_time, str);
    return( fputs(cur_str, fp));
}

int close_log_file(FILE *fp)
{
    return (fclose(fp));
}
