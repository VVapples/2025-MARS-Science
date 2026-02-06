#include <Arduino.h>

#include "CommandSequencer.h"

namespace {
constexpr uint32_t kControlPeriodUs = 1000;
CommandSequencer *g_sequencer = nullptr;
uint32_t g_lastTickUs = 0;
}

void ThreadControl_Init(CommandSequencer *sequencer) {
    g_sequencer = sequencer;
    g_lastTickUs = micros();
}

void ThreadControl_Update() {
    uint32_t now = micros();
    if (now - g_lastTickUs < kControlPeriodUs) {
        return;
    }
    g_lastTickUs += kControlPeriodUs;
    if (g_sequencer) {
        g_sequencer->update();
    }
}
