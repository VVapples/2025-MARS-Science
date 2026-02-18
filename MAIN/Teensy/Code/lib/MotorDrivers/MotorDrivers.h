#pragma once

#include <Arduino.h>

namespace MotorDrivers {

class MotorBase {
 public:
  MotorBase(const char* motorName, bool isMock);
  virtual ~MotorBase() = default;

  virtual bool begin() = 0;
  virtual void setSpeed(float normalizedSpeed) = 0;  // [-1.0, 1.0]
  virtual void stop();

  bool isMock() const { return isMock_; }
  const char* name() const { return name_; }

 protected:
  const char* name_;
  bool isMock_;
  float lastSpeed_ = 0.0f;

  void logAction(const char* action, float value) const;
};

// Generic PWM motor (Pololu/standard H-bridge style)
class MotorPWM : public MotorBase {
 public:
  MotorPWM(const char* motorName, uint8_t in1Pin, uint8_t in2Pin, bool isMock, uint16_t pwmMax = 255);

  bool begin() override;
  void setSpeed(float normalizedSpeed) override;
  void stop() override;

 private:
  uint8_t in1Pin_;
  uint8_t in2Pin_;
  uint16_t pwmMax_;
};

// CAN motor for C610/M2006 class control (M-D3)
class MotorCAN : public MotorBase {
 public:
  MotorCAN(const char* motorName,
           uint8_t motorIndex,
           bool isMock,
           uint32_t canBaud = 1000000,
           uint16_t commandFrameId = 0x200);

  bool begin() override;
  void setSpeed(float normalizedSpeed) override;
  void stop() override;

 private:
  uint8_t motorIndex_;      // 1..4 -> current field position in 0x200 frame
  uint32_t canBaud_;
  uint16_t commandFrameId_;
};

// 3PWM BLDC driver wrapper (SimpleFOC BLDCDriver3PWM)
class MotorSimpleFOC : public MotorBase {
 public:
  MotorSimpleFOC(const char* motorName,
                 uint8_t pinA,
                 uint8_t pinB,
                 uint8_t pinC,
                 int8_t enablePin,
                 bool isMock,
                 float voltageSupply = 12.0f,
                 float voltageLimit = 6.0f);

  ~MotorSimpleFOC() override;

  bool begin() override;
  void setSpeed(float normalizedSpeed) override;
  void stop() override;

 private:
  uint8_t pinA_;
  uint8_t pinB_;
  uint8_t pinC_;
  int8_t enablePin_;
  float voltageSupply_;
  float voltageLimit_;

  float electricalAngle_ = 0.0f;
  uint32_t lastUpdateUs_ = 0;

  void* driverHandle_ = nullptr;  // Opaque pointer to BLDCDriver3PWM when available
};

}  // namespace MotorDrivers
