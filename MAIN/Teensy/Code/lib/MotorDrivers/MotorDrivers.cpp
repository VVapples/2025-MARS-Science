#include "MotorDrivers.h"

#include "Thread_Logger.h"

// ----------------------------
// MotorPWM
// ----------------------------
MotorPWM::MotorPWM(int pwmPin, int dirPin, bool isMock)
    : MotorBase(isMock),
      pwmPin_(pwmPin),
      dirOrPhPin_(dirPin),
      usePhaseEnable_(false) {}

MotorPWM::MotorPWM(int enPin, int phPin, bool isMock, bool usePhaseEnable)
    : MotorBase(isMock),
      pwmPin_(enPin),
      dirOrPhPin_(phPin),
      usePhaseEnable_(usePhaseEnable) {}

void MotorPWM::begin() {
  if (isMock_) {
    Serial.println("[MOCK] MotorPWM begin");
    loggerLog(LogLevel::DATA, LogTag::MOTOR, "MotorPWM begin (mock)");
    return;
  }

  pinMode(pwmPin_, OUTPUT);
  if (dirOrPhPin_ >= 0) {
    pinMode(dirOrPhPin_, OUTPUT);
  }
  loggerLog(LogLevel::DATA, LogTag::MOTOR, "MotorPWM begin");
}

void MotorPWM::setSpeed(float normalized) {
  if (normalized > 1.0f) normalized = 1.0f;
  if (normalized < -1.0f) normalized = -1.0f;

  if (isMock_) {
    Serial.print("[MOCK] MotorPWM setSpeed: ");
    Serial.println(normalized, 3);
    loggerLogf(LogLevel::DATA, LogTag::MOTOR, "MotorPWM setSpeed %.3f (mock)", normalized);
    return;
  }

  int duty = (int)(fabsf(normalized) * 255.0f);
  if (dirOrPhPin_ >= 0) {
    if (usePhaseEnable_) {
      digitalWrite(dirOrPhPin_, (normalized >= 0.0f) ? HIGH : LOW);
      analogWrite(pwmPin_, duty);
    } else {
      digitalWrite(dirOrPhPin_, (normalized >= 0.0f) ? HIGH : LOW);
      analogWrite(pwmPin_, duty);
    }
  } else {
    analogWrite(pwmPin_, duty);
  }
  loggerLogf(LogLevel::DATA, LogTag::MOTOR, "MotorPWM setSpeed %.3f", normalized);
}

void MotorPWM::stop() {
  if (isMock_) {
    Serial.println("[MOCK] MotorPWM stop");
    loggerLog(LogLevel::DATA, LogTag::MOTOR, "MotorPWM stop (mock)");
    return;
  }
  analogWrite(pwmPin_, 0);
  loggerLog(LogLevel::DATA, LogTag::MOTOR, "MotorPWM stop");
}

// ----------------------------
// MotorCAN (C610)
// ----------------------------
MotorCAN::MotorCAN(FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> &bus,
                   uint8_t canId,
                   bool isMock)
    : MotorBase(isMock), bus_(bus), canId_(canId) {}

void MotorCAN::begin() {
  if (isMock_) {
    Serial.println("[MOCK] MotorCAN begin");
    loggerLog(LogLevel::DATA, LogTag::CAN, "MotorCAN begin (mock)");
    return;
  }

  bus_.begin();
  bus_.setBaudRate(1000000);
  loggerLog(LogLevel::DATA, LogTag::CAN, "MotorCAN begin");
}

void MotorCAN::setSpeed(float normalized) {
  if (normalized > 1.0f) normalized = 1.0f;
  if (normalized < -1.0f) normalized = -1.0f;

  if (isMock_) {
    Serial.print("[MOCK] MotorCAN setSpeed: ");
    Serial.println(normalized, 3);
    loggerLogf(LogLevel::DATA, LogTag::CAN, "MotorCAN setSpeed %.3f (mock)", normalized);
    return;
  }

  // Placeholder scaling for C610 speed command.
  int16_t cmd = (int16_t)(normalized * 16384.0f);

  CAN_message_t msg;
  msg.id = canId_;
  msg.len = 8;
  msg.buf[0] = (uint8_t)((cmd >> 8) & 0xFF);
  msg.buf[1] = (uint8_t)(cmd & 0xFF);
  msg.buf[2] = 0;
  msg.buf[3] = 0;
  msg.buf[4] = 0;
  msg.buf[5] = 0;
  msg.buf[6] = 0;
  msg.buf[7] = 0;

  bus_.write(msg);
  loggerLogf(LogLevel::DATA, LogTag::CAN, "MotorCAN setSpeed %.3f", normalized);
}

void MotorCAN::stop() {
  setSpeed(0.0f);
}

// ----------------------------
// MotorSimpleFOC
// ----------------------------
MotorSimpleFOC::MotorSimpleFOC(int polePairs,
                               int pinU,
                               int pinV,
                               int pinW,
                               int enablePin,
                               bool isMock)
    : MotorBase(isMock),
      polePairs_(polePairs),
      pinU_(pinU),
      pinV_(pinV),
      pinW_(pinW),
      enablePin_(enablePin),
      motor_(polePairs_),
      driver_(pinU_, pinV_, pinW_, enablePin_) {}

void MotorSimpleFOC::begin() {
  if (isMock_) {
    Serial.println("[MOCK] MotorSimpleFOC begin");
    loggerLog(LogLevel::DATA, LogTag::MOTOR, "MotorSimpleFOC begin (mock)");
    return;
  }

  driver_.voltage_power_supply = 12.0f;
  driver_.init();
  motor_.linkDriver(&driver_);
  motor_.controller = MotionControlType::velocity;
  motor_.init();
  loggerLog(LogLevel::DATA, LogTag::MOTOR, "MotorSimpleFOC begin");
}

void MotorSimpleFOC::setSpeed(float normalized) {
  if (normalized > 1.0f) normalized = 1.0f;
  if (normalized < -1.0f) normalized = -1.0f;

  if (isMock_) {
    Serial.print("[MOCK] MotorSimpleFOC setSpeed: ");
    Serial.println(normalized, 3);
    loggerLogf(LogLevel::DATA, LogTag::MOTOR, "MotorSimpleFOC setSpeed %.3f (mock)", normalized);
    return;
  }

  motor_.move(normalized);
  loggerLogf(LogLevel::DATA, LogTag::MOTOR, "MotorSimpleFOC setSpeed %.3f", normalized);
}

void MotorSimpleFOC::stop() {
  if (isMock_) {
    Serial.println("[MOCK] MotorSimpleFOC stop");
    loggerLog(LogLevel::DATA, LogTag::MOTOR, "MotorSimpleFOC stop (mock)");
    return;
  }
  motor_.move(0.0f);
  loggerLog(LogLevel::DATA, LogTag::MOTOR, "MotorSimpleFOC stop");
}
