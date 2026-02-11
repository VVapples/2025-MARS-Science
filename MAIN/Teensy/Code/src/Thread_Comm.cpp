#include <Arduino.h>
#include <TeensyThreads.h>

#include "CommandSequencer.h"
#include "Thread_Logger.h"

extern CommandSequencer *g_sequencer;

static const int LINE_BUFFER_SIZE = 192;

void commInit(CommandSequencer *sequencer) {
  g_sequencer = sequencer;
}

void commThread() {
  static char lineBuffer[LINE_BUFFER_SIZE];
  int index = 0;

  while (true) {
    while (Serial.available() > 0) {
      char c = (char)Serial.read();
      if (c == '\r') {
        continue;
      }
      if (c == '\n') {
        lineBuffer[index] = '\0';
        if (index > 0 && g_sequencer) {
          loggerLogf(LogLevel::DATA, LogTag::SYS, "RX %s", lineBuffer);
          g_sequencer->parseLine(lineBuffer);
        }
        index = 0;
      } else if (index < LINE_BUFFER_SIZE - 1) {
        lineBuffer[index++] = c;
      }
    }
    threads.yield();
    delay(2);
  }
}
