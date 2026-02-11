#include "CommandSequencer.h"

#include <stdlib.h>
#include <string.h>

CommandSequencer::CommandSequencer(MotorBase *motors[MAX_MOTORS + 1])
    : eventCount_(0), sensorCallback_(nullptr), heartbeatCallback_(nullptr) {
  for (int i = 0; i <= MAX_MOTORS; ++i) {
    motors_[i] = motors[i];
  }
}

void CommandSequencer::begin() {
  eventCount_ = 0;
}

void CommandSequencer::setSensorCallback(void (*cb)()) { sensorCallback_ = cb; }

void CommandSequencer::setHeartbeatCallback(void (*cb)()) {
  heartbeatCallback_ = cb;
}

void CommandSequencer::update() {
  uint32_t now = millis();

  while (eventCount_ > 0 && events_[0].timeMs <= now) {
    Event event = events_[0];

    for (int i = 1; i < eventCount_; ++i) {
      events_[i - 1] = events_[i];
    }
    --eventCount_;

    switch (event.type) {
      case EventType::MotorStart: {
        MotorBase *motor = getMotor(event.motorId);
        if (motor) {
          motor->setSpeed(event.speed);
        }
      } break;
      case EventType::MotorStop: {
        MotorBase *motor = getMotor(event.motorId);
        if (motor) {
          motor->setSpeed(0.0f);
        }
      } break;
      case EventType::SensorRead: {
        if (sensorCallback_) {
          sensorCallback_();
        }
      } break;
      case EventType::Heartbeat: {
        if (heartbeatCallback_) {
          heartbeatCallback_();
        }
      } break;
    }
  }
}

void CommandSequencer::stopAll() {
  eventCount_ = 0;
  for (int i = 1; i <= MAX_MOTORS; ++i) {
    if (motors_[i]) {
      motors_[i]->stop();
    }
  }
}

bool CommandSequencer::parseLine(const char *line) {
  if (!line) {
    return false;
  }

  char buffer[192];
  strncpy(buffer, line, sizeof(buffer) - 1);
  buffer[sizeof(buffer) - 1] = '\0';

  uint32_t baseTime = millis();
  uint32_t offsetMs = 0;

  bool inMotorCmd = false;
  ComponentMotorId motorId = ComponentMotorId::MD1;
  bool motorIdSet = false;
  float speed = 0.0f;
  bool speedSet = false;
  uint32_t durationMs = 0;

  auto flushMotor = [&]() {
    if (inMotorCmd && motorIdSet && speedSet) {
      scheduleMotor(motorId, speed, baseTime, offsetMs, durationMs);
    }
    inMotorCmd = false;
    motorIdSet = false;
    speedSet = false;
    durationMs = 0;
  };

  char *token = strtok(buffer, " \t\r\n");
  while (token != nullptr) {
    if (strcmp(token, "STOP") == 0) {
      stopAll();
      return true;
    }

    if (strcmp(token, "SENS") == 0) {
      flushMotor();
      scheduleSensor(baseTime, offsetMs);
      token = strtok(nullptr, " \t\r\n");
      continue;
    }

    if (strcmp(token, "HB") == 0) {
      flushMotor();
      scheduleHeartbeat(baseTime, offsetMs);
      token = strtok(nullptr, " \t\r\n");
      continue;
    }

    if (token[0] == 'P') {
      uint32_t pauseMs = (uint32_t)strtoul(token + 1, nullptr, 10);
      flushMotor();
      schedulePause(&offsetMs, pauseMs);
      token = strtok(nullptr, " \t\r\n");
      continue;
    }

    if (token[0] == 'G') {
      flushMotor();
      inMotorCmd = true;
      token = strtok(nullptr, " \t\r\n");
      continue;
    }

    ComponentMotorId parsedMotorId;
    if (tryParseMotorToken(token, &parsedMotorId)) {
      motorId = parsedMotorId;
      motorIdSet = true;
      token = strtok(nullptr, " \t\r\n");
      continue;
    }

    if (token[0] == 'S') {
      speed = strtof(token + 1, nullptr);
      speedSet = true;
      token = strtok(nullptr, " \t\r\n");
      continue;
    }

    if (token[0] == 'T') {
      durationMs = (uint32_t)strtoul(token + 1, nullptr, 10);
      token = strtok(nullptr, " \t\r\n");
      continue;
    }

    token = strtok(nullptr, " \t\r\n");
  }

  flushMotor();
  return true;
}

void CommandSequencer::scheduleMotor(ComponentMotorId motorId,
                                     float speed,
                                     uint32_t baseTimeMs,
                                     uint32_t offsetMs,
                                     uint32_t durationMs) {
  Event startEvent;
  startEvent.timeMs = baseTimeMs + offsetMs;
  startEvent.type = EventType::MotorStart;
  startEvent.motorId = motorId;
  startEvent.speed = speed;
  addEvent(startEvent);

  if (durationMs > 0) {
    Event stopEvent;
    stopEvent.timeMs = baseTimeMs + offsetMs + durationMs;
    stopEvent.type = EventType::MotorStop;
    stopEvent.motorId = motorId;
    stopEvent.speed = 0.0f;
    addEvent(stopEvent);
  }
}

void CommandSequencer::schedulePause(uint32_t *offsetMs, uint32_t pauseMs) {
  if (offsetMs) {
    *offsetMs += pauseMs;
  }
}

void CommandSequencer::scheduleSensor(uint32_t baseTimeMs, uint32_t offsetMs) {
  Event event;
  event.timeMs = baseTimeMs + offsetMs;
  event.type = EventType::SensorRead;
  event.motorId = ComponentMotorId::MD1;
  event.speed = 0.0f;
  addEvent(event);
}

void CommandSequencer::scheduleHeartbeat(uint32_t baseTimeMs,
                                         uint32_t offsetMs) {
  Event event;
  event.timeMs = baseTimeMs + offsetMs;
  event.type = EventType::Heartbeat;
  event.motorId = ComponentMotorId::MD1;
  event.speed = 0.0f;
  addEvent(event);
}

bool CommandSequencer::addEvent(const Event &event) {
  if (eventCount_ >= MAX_EVENTS) {
    return false;
  }

  int insertIndex = eventCount_;
  while (insertIndex > 0 && events_[insertIndex - 1].timeMs > event.timeMs) {
    events_[insertIndex] = events_[insertIndex - 1];
    --insertIndex;
  }
  events_[insertIndex] = event;
  ++eventCount_;
  return true;
}

MotorBase *CommandSequencer::getMotor(ComponentMotorId motorId) {
  int index = static_cast<int>(motorId);
  if (index < 1 || index > MAX_MOTORS) {
    return nullptr;
  }
  return motors_[index];
}

bool CommandSequencer::tryParseMotorToken(const char *token,
                                          ComponentMotorId *outId) {
  if (!token || !outId) {
    return false;
  }

  if (token[0] == 'M' && token[1] >= '0' && token[1] <= '9') {
    GcodeMotorId gcodeId;
    if (tryParseGcodeId(token, &gcodeId)) {
      *outId = mapGcodeToComponent(gcodeId);
      return true;
    }
  }

  if ((token[0] == 'M') && (token[1] == 'A' || token[1] == 'D' || token[1] == 'S')) {
    int id = atoi(token + 2);
    switch (token[1]) {
      case 'A':
        if (id == 1) {
          *outId = ComponentMotorId::MA1;
          return true;
        }
        if (id == 2) {
          *outId = ComponentMotorId::MA2;
          return true;
        }
        break;
      case 'D':
        if (id == 1) {
          *outId = ComponentMotorId::MD1;
          return true;
        }
        if (id == 2) {
          *outId = ComponentMotorId::MD2;
          return true;
        }
        if (id == 3) {
          *outId = ComponentMotorId::MD3;
          return true;
        }
        break;
      case 'S':
        if (id == 1) {
          *outId = ComponentMotorId::MS1;
          return true;
        }
        if (id == 2) {
          *outId = ComponentMotorId::MS2;
          return true;
        }
        if (id == 3) {
          *outId = ComponentMotorId::MS3;
          return true;
        }
        if (id == 4) {
          *outId = ComponentMotorId::MS4;
          return true;
        }
        if (id == 5) {
          *outId = ComponentMotorId::MS5;
          return true;
        }
        if (id == 6) {
          *outId = ComponentMotorId::MS6;
          return true;
        }
        if (id == 7) {
          *outId = ComponentMotorId::MS7;
          return true;
        }
        if (id == 8) {
          *outId = ComponentMotorId::MS8;
          return true;
        }
        break;
    }
  }

  return false;
}

bool CommandSequencer::tryParseGcodeId(const char *token,
                                       GcodeMotorId *outId) {
  if (!token || !outId) {
    return false;
  }

  if (token[0] != 'M') {
    return false;
  }

  int id = atoi(token + 1);
  if (id < 1 || id > 13) {
    return false;
  }
  *outId = static_cast<GcodeMotorId>(id);
  return true;
}

ComponentMotorId CommandSequencer::mapGcodeToComponent(GcodeMotorId gcodeId) {
  for (unsigned int i = 0; i < (sizeof(MOTOR_ID_MAP) / sizeof(MotorIdMapEntry)); ++i) {
    if (MOTOR_ID_MAP[i].gcodeId == gcodeId) {
      return MOTOR_ID_MAP[i].componentId;
    }
  }
  return ComponentMotorId::MD1;
}
