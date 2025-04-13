#pragma once
#ifndef MYLOG_H
#define MYLOG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char *getCurrentTimestamp();
void logMessage(const char *message);

#endif