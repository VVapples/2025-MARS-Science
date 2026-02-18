#include "AppContext.h"

IntervalTimer gControlTimer;

namespace RoverApp {

namespace {
volatile bool gControlTickFlag = false;
}

void controlTickIsr() {
  gControlTickFlag = true;
  ++gControlTickCounter;
}

bool startControlLoop1kHz() {
  // 1kHz => 1000us period
  return gControlTimer.begin(controlTickIsr, 1000);
}

void processControlTicks() {
  if (!gControlTickFlag) return;
  gControlTickFlag = false;

  const uint32_t now = millis();

  if (!gEmergencyStopLatched) {
    gSequencer.update(now);
  }

  if (gSensorRelay != nullptr) {
    gSensorRelay->update(now);

    if (gSequencer.consumeSensorRequestFlag()) {
      gSensorRelay->refreshNow(now);
      appLog(RoverConfig::LogLevel::DATA,
             RoverConfig::LogTag::SENSOR,
             String("SENS,") + gSensorRelay->latestCsv());
      Serial.print("SENS,");
      Serial.println(gSensorRelay->latestCsv());
    }
  }

  if (gSequencer.consumeHeartbeatFlag()) {
    updateHeartbeat(now);
  }
}

}  // namespace RoverApp
