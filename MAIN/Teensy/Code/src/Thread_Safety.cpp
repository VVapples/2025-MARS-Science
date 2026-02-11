#include <Arduino.h>
#include <TeensyThreads.h>

#include "CommandSequencer.h"
#include "Thread_Logger.h"

extern CommandSequencer *g_sequencer;

static const uint32_t HEARTBEAT_TIMEOUT_MS = 3000;

volatile uint32_t g_lastHeartbeatMs = 0;

void safetySetHeartbeat() {
  g_lastHeartbeatMs = millis();
}

void safetyTriggerEStop(const char *reason) {
  if (g_sequencer) {
    g_sequencer->stopAll();
  }
  if (reason) {
    loggerLogf(LogLevel::WARN, LogTag::SYS, "E-STOP %s", reason);
  } else {
    loggerLog(LogLevel::WARN, LogTag::SYS, "E-STOP triggered");
  }
}

void safetyThread() {
  while (true) {
    if (g_lastHeartbeatMs > 0) {
      uint32_t now = millis();
      if ((now - g_lastHeartbeatMs) > HEARTBEAT_TIMEOUT_MS) {
        safetyTriggerEStop("heartbeat timeout");
        g_lastHeartbeatMs = now;
      }
    }
    threads.yield();
    delay(20);
  }
}
