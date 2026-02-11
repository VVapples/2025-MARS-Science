#ifndef COMMAND_SEQUENCER_H
#define COMMAND_SEQUENCER_H

#include <Arduino.h>

#include "RoverConfig.h"
#include "MotorDrivers.h"

class CommandSequencer {
public:
  enum class EventType : uint8_t {
    MotorStart = 0,
    MotorStop = 1,
    SensorRead = 2,
    Heartbeat = 3
  };

  struct Event {
    uint32_t timeMs;
    EventType type;
    ComponentMotorId motorId;
    float speed;
  };

  static const int MAX_MOTORS = 13;
  static const int MAX_EVENTS = 96;

  CommandSequencer(MotorBase *motors[MAX_MOTORS + 1]);

  void begin();
  void update();
  void stopAll();

  void setSensorCallback(void (*cb)());
  void setHeartbeatCallback(void (*cb)());

  bool parseLine(const char *line);

private:
  MotorBase *motors_[MAX_MOTORS + 1];
  Event events_[MAX_EVENTS];
  int eventCount_;

  void (*sensorCallback_)();
  void (*heartbeatCallback_)();

  void scheduleMotor(ComponentMotorId motorId,
                     float speed,
                     uint32_t baseTimeMs,
                     uint32_t offsetMs,
                     uint32_t durationMs);
  void schedulePause(uint32_t *offsetMs, uint32_t pauseMs);
  void scheduleSensor(uint32_t baseTimeMs, uint32_t offsetMs);
  void scheduleHeartbeat(uint32_t baseTimeMs, uint32_t offsetMs);

  bool addEvent(const Event &event);
  MotorBase *getMotor(ComponentMotorId motorId);

  bool tryParseMotorToken(const char *token, ComponentMotorId *outId);
  bool tryParseGcodeId(const char *token, GcodeMotorId *outId);
  ComponentMotorId mapGcodeToComponent(GcodeMotorId gcodeId);
};

#endif  // COMMAND_SEQUENCER_H
