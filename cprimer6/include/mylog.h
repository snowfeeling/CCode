#pragma once
#ifndef MYLOG_H
#define MYLOG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tinycthread.h"

char *getCurrentTimestamp();
int initLogFile();
void logMessage(const char *message);
void closeLogFile();

#endif