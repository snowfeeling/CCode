#ifndef WSSYLOG_H_
#define WSSYLOG_H_

#include <stdio.h>
#include <stdbool.h>
#include <time.h>


size_t get_current_time( char * time_info);

bool init_log_file(FILE **fp, char *fname);

int send_to_log(FILE *fp, char  *str, int timeflag);

int close_log_file(FILE *fp);

#endif