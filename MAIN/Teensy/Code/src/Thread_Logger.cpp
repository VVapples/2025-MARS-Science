#include "Thread_Logger.h"

#include <SD.h>
#include <SPI.h>
#include <TeensyThreads.h>
#include <stdarg.h>

static File g_logFile;
static Threads::Mutex g_logMutex;
static uint32_t g_lastFlushMs = 0;
static int g_pendingLines = 0;

static const uint32_t FLUSH_INTERVAL_MS = 500;
static const int FLUSH_LINES = 10;

static const char *levelToString(LogLevel level) {
  switch (level) {
    case LogLevel::INFO:
      return "INFO";
    case LogLevel::DATA:
      return "DATA";
    case LogLevel::WARN:
      return "WARN";
    case LogLevel::ERROR:
      return "ERROR";
    default:
      return "UNK";
  }
}

static const char *tagToString(LogTag tag) {
  switch (tag) {
    case LogTag::SYS:
      return "SYS";
    case LogTag::MOTOR:
      return "MOTOR";
    case LogTag::SENSOR:
      return "SENSOR";
    case LogTag::CAN:
      return "CAN";
    default:
      return "UNK";
  }
}

static bool openNextLogFile() {
  for (int i = 0; i < 1000; ++i) {
    char name[16];
    snprintf(name, sizeof(name), "/log_%03d.csv", i);
    if (!SD.exists(name)) {
      g_logFile = SD.open(name, FILE_WRITE);
      if (g_logFile) {
        g_logFile.println("Time_ms,Level,Tag,Message");
        g_logFile.flush();
        return true;
      }
      return false;
    }
  }
  return false;
}

void loggerInit() {
  if (!SD.begin(BUILTIN_SDCARD)) {
    return;
  }
  openNextLogFile();
  g_lastFlushMs = millis();
}

void loggerLog(LogLevel level, LogTag tag, const char *message) {
  if (!g_logFile) {
    return;
  }

  g_logMutex.lock();
  g_logFile.print(millis());
  g_logFile.print(',');
  g_logFile.print(levelToString(level));
  g_logFile.print(',');
  g_logFile.print(tagToString(tag));
  g_logFile.print(',');
  g_logFile.println(message ? message : "");

  g_pendingLines++;
  g_logMutex.unlock();
}

void loggerLogf(LogLevel level, LogTag tag, const char *fmt, ...) {
  char buffer[128];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);
  loggerLog(level, tag, buffer);
}

void loggerFlush() {
  if (!g_logFile) {
    return;
  }

  g_logMutex.lock();
  g_logFile.flush();
  g_pendingLines = 0;
  g_lastFlushMs = millis();
  g_logMutex.unlock();
}

void loggerThread() {
  while (true) {
    uint32_t now = millis();
    if ((g_pendingLines >= FLUSH_LINES) || (now - g_lastFlushMs >= FLUSH_INTERVAL_MS)) {
      loggerFlush();
    }
    threads.yield();
    delay(20);
  }
}
