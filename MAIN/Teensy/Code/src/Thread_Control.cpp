#include <Arduino.h>
#include <IntervalTimer.h>

#include "CommandSequencer.h"

CommandSequencer *g_sequencer = nullptr;

static IntervalTimer g_controlTimer;

static void controlTick() {
  if (g_sequencer) {
    g_sequencer->update();
  }
}

void controlInit(CommandSequencer *sequencer) {
  g_sequencer = sequencer;
  g_controlTimer.begin(controlTick, 1000);
}
