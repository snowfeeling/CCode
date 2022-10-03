#ifndef MYLOG_H_
#define MYLOG_H_


bool InitLogfile(FILE **fp, char *fname);

int SendToLog(FILE *fp, char  *str);

int CloseLogfile(FILE *fp);

#endif