#include "CommandSequencer.h"

#include <stdlib.h>

CommandSequencer::CommandSequencer() {
  for (uint8_t i = 0; i < kMaxMotors; ++i) {
    motorsByNumber_[i] = nullptr;
  }
}

bool CommandSequencer::registerMotorByNumericId(RoverConfig::MotorNumericId id,
                                                MotorDrivers::MotorBase* motor) {
  const uint8_t idx = static_cast<uint8_t>(id);
  if (idx >= kMaxMotors || motor == nullptr) return false;
  motorsByNumber_[idx] = motor;
  return true;
}

bool CommandSequencer::registerMotorByToken(const char* token, MotorDrivers::MotorBase* motor) {
  if (token == nullptr || motor == nullptr) return false;

  String t = String(token);
  t.trim();
  t.toUpperCase();

  // Direct numeric style M1, M2, ...
  if (isNumericMotorToken(t)) {
    const int m = parseMotorNumberFromToken(t);
    if (m > 0 && m < kMaxMotors) {
      motorsByNumber_[m] = motor;
      return true;
    }
    return false;
  }

  // Token style MD1/MS1/MA1 mapped via RoverConfig::MOTOR_ID_MAP
  for (size_t i = 0; i < RoverConfig::MOTOR_ID_MAP_COUNT; ++i) {
    if (t.equalsIgnoreCase(RoverConfig::MOTOR_ID_MAP[i].gcodeToken)) {
      const uint8_t idx = static_cast<uint8_t>(RoverConfig::MOTOR_ID_MAP[i].numericId);
      if (idx < kMaxMotors) {
        motorsByNumber_[idx] = motor;
        return true;
      }
    }
  }

  return false;
}

CommandSequencer::ParseResult CommandSequencer::enqueueFromLine(const char* line) {
  if (line == nullptr) return {false, 0, "null line"};

  String normalized(line);
  normalized.trim();
  if (normalized.length() == 0) return {false, 0, "empty line"};

  return parseTokensToActions(normalized);
}

void CommandSequencer::update(uint32_t nowMs) {
  // Run multiple immediate actions in one call, but limit to avoid starvation.
  uint8_t safetySteps = 0;
  while (size_ > 0 && safetySteps < kMaxQueuedActions) {
    ++safetySteps;

    Action current;
    if (!dequeueAction(current)) return;

    if (!current.started) {
      current.started = true;
      current.startMs = nowMs;

      switch (current.type) {
        case ActionType::MOTOR_SET:
          if (current.motor != nullptr) {
            current.motor->setSpeed(current.speed);
          }
          break;

        case ActionType::PAUSE:
          // Wait only by time check below.
          break;

        case ActionType::SENS:
          sensorRequestFlag_ = true;
          break;

        case ActionType::HEARTBEAT:
          heartbeatFlag_ = true;
          break;

        case ActionType::STOP_ALL:
          stopAll();
          continue;  // queue already changed by stopAll
      }
    }

    bool done = false;
    switch (current.type) {
      case ActionType::MOTOR_SET:
        if (current.durationMs == 0) {
          done = true;
        } else if ((nowMs - current.startMs) >= current.durationMs) {
          if (current.motor != nullptr) {
            current.motor->stop();
          }
          done = true;
        }
        break;

      case ActionType::PAUSE:
        done = ((nowMs - current.startMs) >= current.durationMs);
        break;

      case ActionType::SENS:
      case ActionType::HEARTBEAT:
      case ActionType::STOP_ALL:
        done = true;
        break;
    }

    if (!done) {
      // Put it back at front by rewinding head
      head_ = (head_ == 0) ? (kMaxQueuedActions - 1) : (head_ - 1);
      queue_[head_] = current;
      ++size_;
      break;  // waiting on elapsed time
    }
  }
}

void CommandSequencer::stopAll() {
  for (uint8_t i = 1; i < kMaxMotors; ++i) {
    if (motorsByNumber_[i] != nullptr) {
      motorsByNumber_[i]->stop();
    }
  }
  clearQueue();
}

void CommandSequencer::clearQueue() {
  head_ = 0;
  tail_ = 0;
  size_ = 0;
}

uint8_t CommandSequencer::queuedCount() const {
  return size_;
}

bool CommandSequencer::consumeSensorRequestFlag() {
  const bool v = sensorRequestFlag_;
  sensorRequestFlag_ = false;
  return v;
}

bool CommandSequencer::consumeHeartbeatFlag() {
  const bool v = heartbeatFlag_;
  heartbeatFlag_ = false;
  return v;
}

bool CommandSequencer::enqueueAction(const Action& a) {
  if (size_ >= kMaxQueuedActions) return false;
  queue_[tail_] = a;
  tail_ = static_cast<uint8_t>((tail_ + 1) % kMaxQueuedActions);
  ++size_;
  return true;
}

bool CommandSequencer::dequeueAction(Action& out) {
  if (size_ == 0) return false;
  out = queue_[head_];
  head_ = static_cast<uint8_t>((head_ + 1) % kMaxQueuedActions);
  --size_;
  return true;
}

MotorDrivers::MotorBase* CommandSequencer::resolveMotorToken(const String& token) const {
  String t = token;
  t.trim();
  t.toUpperCase();

  if (isNumericMotorToken(t)) {
    const int m = parseMotorNumberFromToken(t);
    if (m > 0 && m < kMaxMotors) {
      return motorsByNumber_[m];
    }
    return nullptr;
  }

  for (size_t i = 0; i < RoverConfig::MOTOR_ID_MAP_COUNT; ++i) {
    if (t.equalsIgnoreCase(RoverConfig::MOTOR_ID_MAP[i].gcodeToken)) {
      const uint8_t idx = static_cast<uint8_t>(RoverConfig::MOTOR_ID_MAP[i].numericId);
      if (idx < kMaxMotors) return motorsByNumber_[idx];
      return nullptr;
    }
  }

  return nullptr;
}

bool CommandSequencer::isNumericMotorToken(const String& token) {
  if (token.length() < 2) return false;
  if (token.charAt(0) != 'M') return false;
  for (uint16_t i = 1; i < token.length(); ++i) {
    if (!isDigit(token.charAt(i))) return false;
  }
  return true;
}

int CommandSequencer::parseMotorNumberFromToken(const String& token) {
  if (!isNumericMotorToken(token)) return -1;
  return atoi(token.substring(1).c_str());
}

CommandSequencer::ParseResult CommandSequencer::parseTokensToActions(const String& normalizedLine) {
  String line = normalizedLine;
  line.replace('\t', ' ');

  uint16_t enq = 0;

  uint16_t pos = 0;
  const uint16_t n = line.length();

  while (pos < n) {
    while (pos < n && line.charAt(pos) == ' ') ++pos;
    if (pos >= n) break;

    // Read one token
    uint16_t start = pos;
    while (pos < n && line.charAt(pos) != ' ') ++pos;
    String token = line.substring(start, pos);
    token.toUpperCase();

    if (token.length() == 0) continue;

    if (token == "STOP") {
      Action a;
      a.type = ActionType::STOP_ALL;
      if (!enqueueAction(a)) return {false, enq, "queue full"};
      ++enq;
      continue;
    }

    if (token == "SENS") {
      Action a;
      a.type = ActionType::SENS;
      if (!enqueueAction(a)) return {false, enq, "queue full"};
      ++enq;
      continue;
    }

    if (token == "HB") {
      Action a;
      a.type = ActionType::HEARTBEAT;
      if (!enqueueAction(a)) return {false, enq, "queue full"};
      ++enq;
      continue;
    }

    // Pause token style: P1000
    uint32_t pauseMs = 0;
    if (parseUnsignedAfterPrefix(token, 'P', pauseMs) && token.length() > 1) {
      Action a;
      a.type = ActionType::PAUSE;
      a.durationMs = pauseMs;
      if (!enqueueAction(a)) return {false, enq, "queue full"};
      ++enq;
      continue;
    }

    // Start of motion command: Gx ... then expect one motor token + params
    if (token.length() >= 2 && token.charAt(0) == 'G' && isDigit(token.charAt(1))) {
      // Read subsequent tokens until next standalone command start
      String motorToken;
      float speed = 0.0f;
      bool hasSpeed = false;
      uint32_t durationMs = 0;
      bool hasDuration = false;

      uint16_t scan = pos;
      while (scan < n) {
        while (scan < n && line.charAt(scan) == ' ') ++scan;
        if (scan >= n) break;

        uint16_t s0 = scan;
        while (scan < n && line.charAt(scan) != ' ') ++scan;
        String t = line.substring(s0, scan);
        t.toUpperCase();

        // Break on next command starters
        if (t == "STOP" || t == "SENS" || t == "HB" ||
            (t.length() >= 2 && t.charAt(0) == 'G' && isDigit(t.charAt(1)))) {
          // Rewind to token start so outer parser can process this token.
          scan = s0;
          break;
        }

        // Break if pause token appears (belongs to outer loop)
        uint32_t tmpPause = 0;
        if (parseUnsignedAfterPrefix(t, 'P', tmpPause) && t.length() > 1 && hasDuration) {
          // Rewind to token start so outer parser can process pause token.
          scan = s0;
          break;
        }

        // Parse motor token (M1/MD1/MS1/MA1...)
        if (t.length() >= 2 && t.charAt(0) == 'M') {
          motorToken = t;
          continue;
        }

        float fs = 0.0f;
        if (parseFloatAfterPrefix(t, 'S', fs)) {
          speed = normalizeSpeedToken(fs);
          hasSpeed = true;
          continue;
        }

        uint32_t dt = 0;
        if (parseUnsignedAfterPrefix(t, 'T', dt)) {
          durationMs = dt;
          hasDuration = true;
          continue;
        }

        // Backward compatibility: allow Pxxx as duration inside G command when T absent
        if (!hasDuration && parseUnsignedAfterPrefix(t, 'P', dt)) {
          durationMs = dt;
          hasDuration = true;
          continue;
        }
      }

      if (motorToken.length() == 0) {
        return {false, enq, "missing motor token after G-code"};
      }

      MotorDrivers::MotorBase* m = resolveMotorToken(motorToken);
      if (m == nullptr) {
        return {false, enq, "motor token not registered"};
      }

      Action a;
      a.type = ActionType::MOTOR_SET;
      a.motor = m;
      a.speed = hasSpeed ? speed : 0.0f;
      a.durationMs = hasDuration ? durationMs : 0;

      if (!enqueueAction(a)) return {false, enq, "queue full"};
      ++enq;

      // Continue outer token loop from where we scanned.
      pos = scan;
      continue;
    }

    // Unknown token, ignore but keep parsing.
  }

  return {true, enq, "ok"};
}

bool CommandSequencer::startsWithIgnoreCase(const String& s, const char* prefix) {
  String p(prefix);
  return s.substring(0, p.length()).equalsIgnoreCase(p);
}

bool CommandSequencer::parseUnsignedAfterPrefix(const String& token,
                                                char prefix,
                                                uint32_t& outValue) {
  if (token.length() < 2) return false;
  if (token.charAt(0) != prefix) return false;
  for (uint16_t i = 1; i < token.length(); ++i) {
    if (!isDigit(token.charAt(i))) return false;
  }
  outValue = static_cast<uint32_t>(strtoul(token.substring(1).c_str(), nullptr, 10));
  return true;
}

bool CommandSequencer::parseFloatAfterPrefix(const String& token,
                                             char prefix,
                                             float& outValue) {
  if (token.length() < 2) return false;
  if (token.charAt(0) != prefix) return false;

  char* endPtr = nullptr;
  outValue = strtof(token.substring(1).c_str(), &endPtr);
  return (endPtr != nullptr && *endPtr == '\0');
}

float CommandSequencer::normalizeSpeedToken(float rawS) {
  // Accept either normalized input [-1..1] or percentage-like [-100..100]
  if (rawS > 1.0f || rawS < -1.0f) {
    rawS *= 0.01f;
  }

  if (rawS > 1.0f) return 1.0f;
  if (rawS < -1.0f) return -1.0f;
  return rawS;
}
