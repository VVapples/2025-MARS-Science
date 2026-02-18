#include "AppContext.h"

#include <SD.h>
#include <SPI.h>
#include <TeensyThreads.h>

namespace RoverApp {
namespace {

struct LogItem {
  uint32_t timeMs;
  RoverConfig::LogLevel level;
  RoverConfig::LogTag tag;
  char message[128];
};

constexpr uint16_t kLogQueueSize = 128;
volatile uint16_t gLogHead = 0;
volatile uint16_t gLogTail = 0;
volatile uint16_t gLogCount = 0;
LogItem gLogQueue[kLogQueueSize];

File gLogFile;
bool gLoggerReady = false;
uint32_t gLastFlushMs = 0;
uint16_t gBufferedLines = 0;

const char* levelToText(RoverConfig::LogLevel level) {
  switch (level) {
    case RoverConfig::LogLevel::INFO:
      return "INFO";
    case RoverConfig::LogLevel::DATA:
      return "DATA";
    case RoverConfig::LogLevel::WARN:
      return "WARN";
    case RoverConfig::LogLevel::ERROR:
      return "ERROR";
    default:
      return "INFO";
  }
}

const char* tagToText(RoverConfig::LogTag tag) {
  switch (tag) {
    case RoverConfig::LogTag::SYS:
      return "SYS";
    case RoverConfig::LogTag::MOTOR:
      return "MOTOR";
    case RoverConfig::LogTag::SENSOR:
      return "SENSOR";
    case RoverConfig::LogTag::CAN:
      return "CAN";
    default:
      return "SYS";
  }
}

bool enqueueLog(const LogItem& item) {
  noInterrupts();
  if (gLogCount >= kLogQueueSize) {
    interrupts();
    return false;
  }
  gLogQueue[gLogTail] = item;
  gLogTail = static_cast<uint16_t>((gLogTail + 1) % kLogQueueSize);
  ++gLogCount;
  interrupts();
  return true;
}

bool dequeueLog(LogItem& out) {
  noInterrupts();
  if (gLogCount == 0) {
    interrupts();
    return false;
  }
  out = gLogQueue[gLogHead];
  gLogHead = static_cast<uint16_t>((gLogHead + 1) % kLogQueueSize);
  --gLogCount;
  interrupts();
  return true;
}

String pickNextLogFilename() {
  char name[20];
  for (uint16_t i = 0; i < 1000; ++i) {
    snprintf(name, sizeof(name), "/log_%03u.csv", i);
    if (!SD.exists(name)) {
      return String(name);
    }
  }
  return String("/log_999.csv");
}

void writeCsvLine(const LogItem& item) {
  if (!gLoggerReady || !gLogFile) return;

  gLogFile.print(item.timeMs);
  gLogFile.print(',');
  gLogFile.print(levelToText(item.level));
  gLogFile.print(',');
  gLogFile.print(tagToText(item.tag));
  gLogFile.print(',');
  gLogFile.println(item.message);
  ++gBufferedLines;
}

}  // namespace

bool loggerInit() {
  if (!SD.begin(BUILTIN_SDCARD)) {
    Serial.println("[LOGGER] SD init failed");
    gLoggerReady = false;
    return false;
  }

  const String filename = pickNextLogFilename();
  gLogFile = SD.open(filename.c_str(), FILE_WRITE);
  if (!gLogFile) {
    Serial.println("[LOGGER] open log file failed");
    gLoggerReady = false;
    return false;
  }

  gLogFile.println("Time_ms,Level,Tag,Message");
  gLogFile.flush();
  gLastFlushMs = millis();
  gBufferedLines = 0;
  gLoggerReady = true;

  Serial.print("[LOGGER] file: ");
  Serial.println(filename);
  return true;
}

void appLog(RoverConfig::LogLevel level, RoverConfig::LogTag tag, const String& message) {
  LogItem item;
  item.timeMs = millis();
  item.level = level;
  item.tag = tag;
  message.substring(0, sizeof(item.message) - 1).toCharArray(item.message, sizeof(item.message));

  if (!enqueueLog(item)) {
    Serial.println("[LOGGER] queue full; dropping message");
  }
}

void loggerUpdate() {
  LogItem item;
  uint8_t drained = 0;
  while (drained < 24 && dequeueLog(item)) {
    writeCsvLine(item);
    ++drained;
  }

  const uint32_t now = millis();
  if (gLoggerReady && gLogFile) {
    // Flush strategy:
    // - Flush every 16 lines (throughput-friendly)
    // - Or every 500ms to cap data loss on unexpected reset
    if (gBufferedLines >= 16 || (now - gLastFlushMs) >= 500U) {
      gLogFile.flush();
      gBufferedLines = 0;
      gLastFlushMs = now;
    }
  }
}

void threadLogger() {
  while (true) {
    loggerUpdate();
    threads.delay(10);
  }
}

}  // namespace RoverApp
