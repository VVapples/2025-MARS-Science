#pragma once

#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>
#include <SimpleFOC.h>

#include "RoverConfig.h"

class MotorBase {
public:
    explicit MotorBase(bool isMock);
    virtual ~MotorBase() = default;

    virtual bool begin() = 0;
    virtual void setSpeed(float speed) = 0;
    virtual void stop();
    virtual void engageBrake();
protected:
    bool isMockActive() const;
    static float clampSpeed(float speed);

    bool isMock_;
};

class DrillMotor : public MotorBase {
public:
    DrillMotor(Adafruit_PWMServoDriver *pca, bool isMock);

    bool begin() override;
    void setSpeed(float speed) override;
    void engageBrake() override;

private:
    Adafruit_PWMServoDriver *pca_;
    uint8_t chEn_;
    uint8_t chFr_;
    uint8_t chBk_;
    uint8_t chSv_;
};

class PumpMotor : public MotorBase {
public:
    PumpMotor(Adafruit_PWMServoDriver *pca, bool isMock);

    bool begin() override;
    void setSpeed(float speed) override;

private:
    static constexpr size_t kPumpCount = 7;
    Adafruit_PWMServoDriver *pca_;
    uint8_t enChannels_[kPumpCount];
    uint8_t phChannels_[kPumpCount];
};

class GimbalMotor : public MotorBase {
public:
    GimbalMotor(Adafruit_PWMServoDriver *pca, uint8_t signalChannel, bool isMock);

    bool begin() override;
    void setSpeed(float speed) override;

private:
    Adafruit_PWMServoDriver *pca_;
    uint8_t signalChannel_;
};

class VerticalMotor : public MotorBase {
public:
    VerticalMotor(uint8_t in1,
                  uint8_t in2,
                  uint8_t in3,
                  uint8_t polePairs,
                  Adafruit_PWMServoDriver *pca,
                  uint8_t enableChannel,
                  bool isMock);

    bool begin() override;
    void setSpeed(float speed) override;

private:
    BLDCMotor motor_;
    BLDCDriver3PWM driver_;
    Adafruit_PWMServoDriver *pca_;
    uint8_t enableChannel_;
    bool initialized_;
};
