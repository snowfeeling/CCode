#include <stdio.h>
#include <stdbool.h>

#include "../inc/mylog.h"

bool InitLogfile(FILE **fp, char *fname)
{
    (*fp) = fopen(fname, "a");
    return  (*fp ==NULL ) ? false : true;
}

int SendToLog(FILE *fp, char  *str)
{
   return( fputs(str, fp));
}

int CloseLogfile(FILE *fp)
{
    return (fclose(fp));
}
