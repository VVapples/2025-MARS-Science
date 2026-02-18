#pragma once

#include <Arduino.h>

#include "RoverConfig.h"
#include <MotorDrivers.h>

class CommandSequencer {
 public:
  enum class ActionType : uint8_t {
    MOTOR_SET = 0,
    PAUSE,
    SENS,
    HEARTBEAT,
    STOP_ALL
  };

  struct Action {
    ActionType type = ActionType::PAUSE;
    MotorDrivers::MotorBase* motor = nullptr;
    float speed = 0.0f;
    uint32_t durationMs = 0;
    bool started = false;
    uint32_t startMs = 0;
  };

  struct ParseResult {
    bool ok = false;
    uint16_t enqueued = 0;
    const char* message = nullptr;
  };

  static constexpr uint8_t kMaxQueuedActions = 48;
  static constexpr uint8_t kMaxMotors = 16;

  CommandSequencer();

  // Register motor pointers (typically once in setup)
  bool registerMotorByNumericId(RoverConfig::MotorNumericId id, MotorDrivers::MotorBase* motor);
  bool registerMotorByToken(const char* token, MotorDrivers::MotorBase* motor);

  // Parse and enqueue one full command line (G-code style)
  ParseResult enqueueFromLine(const char* line);

  // Non-blocking executor: call often from loop/thread.
  void update(uint32_t nowMs);

  // Controls
  void stopAll();
  void clearQueue();

  // Status
  uint8_t queuedCount() const;

  // Lightweight event flags consumed by higher layer
  bool consumeSensorRequestFlag();
  bool consumeHeartbeatFlag();

 private:
  MotorDrivers::MotorBase* motorsByNumber_[kMaxMotors];  // index 1..13 used

  Action queue_[kMaxQueuedActions];
  uint8_t head_ = 0;
  uint8_t tail_ = 0;
  uint8_t size_ = 0;

  bool sensorRequestFlag_ = false;
  bool heartbeatFlag_ = false;

  bool enqueueAction(const Action& a);
  bool dequeueAction(Action& out);

  MotorDrivers::MotorBase* resolveMotorToken(const String& token) const;
  static bool isNumericMotorToken(const String& token);
  static int parseMotorNumberFromToken(const String& token);

  ParseResult parseTokensToActions(const String& normalizedLine);

  static bool startsWithIgnoreCase(const String& s, const char* prefix);
  static bool parseUnsignedAfterPrefix(const String& token, char prefix, uint32_t& outValue);
  static bool parseFloatAfterPrefix(const String& token, char prefix, float& outValue);
  static float normalizeSpeedToken(float rawS);
};
