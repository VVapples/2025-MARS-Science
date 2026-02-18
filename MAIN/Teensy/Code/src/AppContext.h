#pragma once

#include <Arduino.h>

#include "CommandSequencer.h"
#include "RoverConfig.h"
#include "SensorRelay.h"

namespace RoverApp {

// Shared application objects
extern CommandSequencer gSequencer;
extern SensorRelay* gSensorRelay;

// Safety/health shared flags
extern volatile uint32_t gLastHeartbeatMs;
extern volatile bool gEmergencyStopLatched;
extern volatile uint32_t gControlTickCounter;

// Logger API (implemented in Thread_Logger.cpp)
bool loggerInit();
void loggerUpdate();
void appLog(RoverConfig::LogLevel level, RoverConfig::LogTag tag, const String& message);
void threadLogger();

// Cross-thread helpers
void updateHeartbeat(uint32_t nowMs);
void requestEmergencyStop(const String& reason);

// Thread entry points
void threadComm();
void threadSafety();

// 1kHz control timer hooks
bool startControlLoop1kHz();
void processControlTicks();

}  // namespace RoverApp
