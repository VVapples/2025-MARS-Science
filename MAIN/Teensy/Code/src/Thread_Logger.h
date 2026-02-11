#ifndef THREAD_LOGGER_H
#define THREAD_LOGGER_H

#include <Arduino.h>

#include "RoverConfig.h"

void loggerInit();
void loggerLog(LogLevel level, LogTag tag, const char *message);
void loggerLogf(LogLevel level, LogTag tag, const char *fmt, ...);
void loggerFlush();
void loggerThread();

#endif  // THREAD_LOGGER_H
