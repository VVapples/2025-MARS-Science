#include "MotorDrivers.h"

#include <math.h>

#if __has_include(<FlexCAN_T4.h>)
#include <FlexCAN_T4.h>
#define ROVER_HAS_FLEXCAN 1
#else
#define ROVER_HAS_FLEXCAN 0
#endif

#if __has_include(<SimpleFOC.h>)
#include <SimpleFOC.h>
#define ROVER_HAS_SIMPLEFOC 1
#else
#define ROVER_HAS_SIMPLEFOC 0
#endif

namespace MotorDrivers {

namespace {

float clampNormalized(float value) {
  if (value > 1.0f) return 1.0f;
  if (value < -1.0f) return -1.0f;
  return value;
}

#if ROVER_HAS_FLEXCAN
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> g_canBus;
bool g_canStarted = false;
#endif

}  // namespace

MotorBase::MotorBase(const char* motorName, bool isMock)
    : name_(motorName), isMock_(isMock) {}

void MotorBase::stop() {
  setSpeed(0.0f);
}

void MotorBase::logAction(const char* action, float value) const {
  Serial.print("[MOTOR][");
  Serial.print(name_);
  Serial.print("] ");
  Serial.print(action);
  Serial.print(" value=");
  Serial.print(value, 4);
  Serial.print(" mode=");
  Serial.println(isMock_ ? "MOCK" : "REAL");
}

MotorPWM::MotorPWM(const char* motorName,
                   uint8_t in1Pin,
                   uint8_t in2Pin,
                   bool isMock,
                   uint16_t pwmMax)
    : MotorBase(motorName, isMock),
      in1Pin_(in1Pin),
      in2Pin_(in2Pin),
      pwmMax_(pwmMax) {}

bool MotorPWM::begin() {
  if (isMock_) {
    logAction("begin", 0.0f);
    return true;
  }

  pinMode(in1Pin_, OUTPUT);
  pinMode(in2Pin_, OUTPUT);
  analogWrite(in1Pin_, 0);
  analogWrite(in2Pin_, 0);
  logAction("begin", 0.0f);
  return true;
}

void MotorPWM::setSpeed(float normalizedSpeed) {
  const float s = clampNormalized(normalizedSpeed);
  lastSpeed_ = s;

  if (isMock_) {
    logAction("setSpeed", s);
    return;
  }

  const uint16_t pwm = static_cast<uint16_t>(fabsf(s) * static_cast<float>(pwmMax_));

  if (s > 0.0f) {
    analogWrite(in1Pin_, pwm);
    analogWrite(in2Pin_, 0);
  } else if (s < 0.0f) {
    analogWrite(in1Pin_, 0);
    analogWrite(in2Pin_, pwm);
  } else {
    analogWrite(in1Pin_, 0);
    analogWrite(in2Pin_, 0);
  }

  logAction("setSpeed", s);
}

void MotorPWM::stop() {
  setSpeed(0.0f);
}

MotorCAN::MotorCAN(const char* motorName,
                   uint8_t motorIndex,
                   bool isMock,
                   uint32_t canBaud,
                   uint16_t commandFrameId)
    : MotorBase(motorName, isMock),
      motorIndex_(motorIndex),
      canBaud_(canBaud),
      commandFrameId_(commandFrameId) {}

bool MotorCAN::begin() {
  if (isMock_) {
    logAction("begin", 0.0f);
    return true;
  }

#if ROVER_HAS_FLEXCAN
  if (!g_canStarted) {
    g_canBus.begin();
    g_canBus.setBaudRate(canBaud_);
    g_canStarted = true;
  }
  logAction("begin", 0.0f);
  return true;
#else
  Serial.println("[MOTOR][MotorCAN] FlexCAN_T4 not found; forcing mock behavior at runtime.");
  return false;
#endif
}

void MotorCAN::setSpeed(float normalizedSpeed) {
  const float s = clampNormalized(normalizedSpeed);
  lastSpeed_ = s;

  if (isMock_) {
    logAction("setSpeed", s);
    return;
  }

#if ROVER_HAS_FLEXCAN
  CAN_message_t txMsg;
  txMsg.id = commandFrameId_;
  txMsg.len = 8;

  for (uint8_t i = 0; i < 8; ++i) txMsg.buf[i] = 0;

  // C610/M2006 typical command range: [-16384, 16384]
  const int16_t currentCmd = static_cast<int16_t>(s * 16384.0f);

  // motorIndex_: 1..4 maps to bytes [0..7] as 2-byte signed values
  if (motorIndex_ >= 1 && motorIndex_ <= 4) {
    const uint8_t byteOffset = static_cast<uint8_t>((motorIndex_ - 1) * 2);
    txMsg.buf[byteOffset] = static_cast<uint8_t>((currentCmd >> 8) & 0xFF);
    txMsg.buf[byteOffset + 1] = static_cast<uint8_t>(currentCmd & 0xFF);
  }

  g_canBus.write(txMsg);
  logAction("setSpeed", s);
#else
  logAction("setSpeed(NO_FLEXCAN)", s);
#endif
}

void MotorCAN::stop() {
  setSpeed(0.0f);
}

MotorSimpleFOC::MotorSimpleFOC(const char* motorName,
                               uint8_t pinA,
                               uint8_t pinB,
                               uint8_t pinC,
                               int8_t enablePin,
                               bool isMock,
                               float voltageSupply,
                               float voltageLimit)
    : MotorBase(motorName, isMock),
      pinA_(pinA),
      pinB_(pinB),
      pinC_(pinC),
      enablePin_(enablePin),
      voltageSupply_(voltageSupply),
      voltageLimit_(voltageLimit) {}

MotorSimpleFOC::~MotorSimpleFOC() {
#if ROVER_HAS_SIMPLEFOC
  if (driverHandle_ != nullptr) {
    auto* driver = static_cast<BLDCDriver3PWM*>(driverHandle_);
    driver->disable();
    delete driver;
    driverHandle_ = nullptr;
  }
#endif
}

bool MotorSimpleFOC::begin() {
  if (isMock_) {
    logAction("begin", 0.0f);
    return true;
  }

#if ROVER_HAS_SIMPLEFOC
  auto* driver = new BLDCDriver3PWM(pinA_, pinB_, pinC_, enablePin_);
  driver->voltage_power_supply = voltageSupply_;
  driver->voltage_limit = voltageLimit_;
  driver->init();
  driver->enable();
  driverHandle_ = static_cast<void*>(driver);

  lastUpdateUs_ = micros();
  electricalAngle_ = 0.0f;
  logAction("begin", 0.0f);
  return true;
#else
  Serial.println("[MOTOR][MotorSimpleFOC] SimpleFOC not found; forcing mock behavior at runtime.");
  return false;
#endif
}

void MotorSimpleFOC::setSpeed(float normalizedSpeed) {
  const float s = clampNormalized(normalizedSpeed);
  lastSpeed_ = s;

  if (isMock_) {
    logAction("setSpeed", s);
    return;
  }

#if ROVER_HAS_SIMPLEFOC
  auto* driver = static_cast<BLDCDriver3PWM*>(driverHandle_);
  if (driver == nullptr) {
    logAction("setSpeed(NO_DRIVER)", s);
    return;
  }

  const uint32_t nowUs = micros();
  const float dt = static_cast<float>(nowUs - lastUpdateUs_) * 1e-6f;
  lastUpdateUs_ = nowUs;

  // open-loop electrical angle update (very simple placeholder)
  constexpr float maxElectricalRadPerSec = 40.0f;
  electricalAngle_ += s * maxElectricalRadPerSec * dt;
  if (electricalAngle_ > TWO_PI) electricalAngle_ -= TWO_PI;
  if (electricalAngle_ < 0.0f) electricalAngle_ += TWO_PI;

  const float v = fabsf(s) * voltageLimit_;
  const float ua = 0.5f * v * (sinf(electricalAngle_) + 1.0f);
  const float ub = 0.5f * v * (sinf(electricalAngle_ - 2.0943951f) + 1.0f);
  const float uc = 0.5f * v * (sinf(electricalAngle_ + 2.0943951f) + 1.0f);

  driver->setPwm(ua, ub, uc);
  logAction("setSpeed", s);
#else
  logAction("setSpeed(NO_SIMPLEFOC)", s);
#endif
}

void MotorSimpleFOC::stop() {
  setSpeed(0.0f);
}

}  // namespace MotorDrivers
