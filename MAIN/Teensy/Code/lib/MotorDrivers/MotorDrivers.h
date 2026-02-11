#ifndef MOTOR_DRIVERS_H
#define MOTOR_DRIVERS_H

#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <SimpleFOC.h>

class MotorBase {
public:
  explicit MotorBase(bool isMock) : isMock_(isMock) {}
  virtual ~MotorBase() {}

  virtual void begin() = 0;
  virtual void setSpeed(float normalized) = 0;  // -1.0 to 1.0
  virtual void stop() = 0;

  bool isMock() const { return isMock_; }

protected:
  bool isMock_;
};

class MotorPWM : public MotorBase {
public:
  // Standard PWM + optional direction pin.
  MotorPWM(int pwmPin, int dirPin, bool isMock);
  // Phase/Enable style (Pololu 4035): EN + PH.
  MotorPWM(int enPin, int phPin, bool isMock, bool usePhaseEnable);

  void begin() override;
  void setSpeed(float normalized) override;
  void stop() override;

private:
  int pwmPin_;
  int dirOrPhPin_;
  bool usePhaseEnable_;
};

class MotorCAN : public MotorBase {
public:
  MotorCAN(FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> &bus,
           uint8_t canId,
           bool isMock);

  void begin() override;
  void setSpeed(float normalized) override;
  void stop() override;

private:
  FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> &bus_;
  uint8_t canId_;
};

class MotorSimpleFOC : public MotorBase {
public:
  MotorSimpleFOC(int polePairs,
                 int pinU,
                 int pinV,
                 int pinW,
                 int enablePin,
                 bool isMock);

  void begin() override;
  void setSpeed(float normalized) override;
  void stop() override;

private:
  int polePairs_;
  int pinU_;
  int pinV_;
  int pinW_;
  int enablePin_;

  BLDCMotor motor_;
  BLDCDriver3PWM driver_;
};

#endif  // MOTOR_DRIVERS_H
