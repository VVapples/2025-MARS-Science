#include "AppContext.h"

#include <TeensyThreads.h>

namespace RoverApp {

void updateHeartbeat(uint32_t nowMs) {
  gLastHeartbeatMs = nowMs;
}

void requestEmergencyStop(const String& reason) {
  if (gEmergencyStopLatched) return;

  gEmergencyStopLatched = true;
  gSequencer.stopAll();

  appLog(RoverConfig::LogLevel::ERROR,
         RoverConfig::LogTag::SYS,
         String("EMERGENCY_STOP,") + reason);
  Serial.print("EMERGENCY_STOP,");
  Serial.println(reason);
}

void threadSafety() {
  pinMode(RoverConfig::Pins::MD1_ALM, INPUT_PULLUP);
  pinMode(RoverConfig::Pins::MD2_ALM, INPUT_PULLUP);

  updateHeartbeat(millis());

  while (true) {
    const uint32_t now = millis();

    const bool hbTimeout = (now - gLastHeartbeatMs) > 3000U;
    if (hbTimeout) {
      requestEmergencyStop("Heartbeat timeout");
    }

    // Alarm pins from drill drivers (assumed active-low)
    const bool md1Alarm = (digitalRead(RoverConfig::Pins::MD1_ALM) == LOW);
    const bool md2Alarm = (digitalRead(RoverConfig::Pins::MD2_ALM) == LOW);
    if (md1Alarm || md2Alarm) {
      requestEmergencyStop("Motor alarm pin active");
    }

    // If STOP latched, keep queue cleared and motors halted.
    if (gEmergencyStopLatched) {
      gSequencer.stopAll();
    }

    threads.delay(20);
  }
}

}  // namespace RoverApp
