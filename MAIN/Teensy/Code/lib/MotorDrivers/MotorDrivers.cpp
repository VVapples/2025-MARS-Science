#include "MotorDrivers.h"

#include <math.h>

MotorBase::MotorBase(bool isMock) : isMock_(isMock) {}

void MotorBase::stop() {
    setSpeed(0.0f);
}

bool MotorBase::isMockActive() const {
    if (ROVER_MODE == MODE_FULL_MOCK) {
        return true;
    }
    if (ROVER_MODE == MODE_PARTIAL_MOCK) {
        return isMock_;
    }
    return false;
}

float MotorBase::clampSpeed(float speed) {
    if (speed > 1.0f) {
        return 1.0f;
    }
    if (speed < -1.0f) {
        return -1.0f;
    }
    return speed;
}

DrillMotor::DrillMotor(Adafruit_PWMServoDriver *pca, bool isMock)
    : MotorBase(isMock),
      pca_(pca),
      chEn_(CH_E1_M_D1_EN),
      chFr_(CH_E1_M_D1_FR),
      chBk_(CH_E1_M_D1_BK),
      chSv_(CH_E1_M_D1_SV) {}

bool DrillMotor::begin() {
    if (isMockActive()) {
        Serial.println("Mock DrillMotor begin");
        return true;
    }
    if (!pca_) {
        return false;
    }
    pca_->setPWM(chEn_, 0, PCA9685_PWM_MIN);
    pca_->setPWM(chFr_, 0, PCA9685_PWM_MIN);
    pca_->setPWM(chBk_, 0, PCA9685_PWM_MIN);
    pca_->setPWM(chSv_, 0, PCA9685_PWM_MIN);
    return true;
}

void DrillMotor::setSpeed(float speed) {
    if (isMockActive()) {
        Serial.print("Mock DrillMotor setSpeed: ");
        Serial.println(speed, 4);
        return;
    }
    if (!pca_) {
        return;
    }

    float clamped = clampSpeed(speed);
    bool forward = clamped >= 0.0f;
    uint16_t pwm = static_cast<uint16_t>(fabsf(clamped) * PCA9685_PWM_MAX);

    pca_->setPWM(chEn_, 0, pwm);
    pca_->setPWM(chSv_, 0, pwm);
    pca_->setPWM(chFr_, 0, forward ? PCA9685_PWM_MAX : PCA9685_PWM_MIN);
    pca_->setPWM(chBk_, 0, forward ? PCA9685_PWM_MIN : PCA9685_PWM_MAX);
}

PumpMotor::PumpMotor(Adafruit_PWMServoDriver *pca, bool isMock)
    : MotorBase(isMock),
      pca_(pca),
      chEn_(CH_E1_M_D2_EN),
      chFr_(CH_E1_M_D2_FR),
      chBk_(CH_E1_M_D2_BK),
      chSv_(CH_E1_M_D2_SV) {}

bool PumpMotor::begin() {
    if (isMockActive()) {
        Serial.println("Mock PumpMotor begin");
        return true;
    }
    if (!pca_) {
        return false;
    }
    pca_->setPWM(chEn_, 0, PCA9685_PWM_MIN);
    pca_->setPWM(chFr_, 0, PCA9685_PWM_MIN);
    pca_->setPWM(chBk_, 0, PCA9685_PWM_MIN);
    pca_->setPWM(chSv_, 0, PCA9685_PWM_MIN);
    return true;
}

void PumpMotor::setSpeed(float speed) {
    if (isMockActive()) {
        Serial.print("Mock PumpMotor setSpeed: ");
        Serial.println(speed, 4);
        return;
    }
    if (!pca_) {
        return;
    }

    float clamped = clampSpeed(speed);
    bool forward = clamped >= 0.0f;
    uint16_t pwm = static_cast<uint16_t>(fabsf(clamped) * PCA9685_PWM_MAX);

    pca_->setPWM(chEn_, 0, pwm);
    pca_->setPWM(chSv_, 0, pwm);
    pca_->setPWM(chFr_, 0, forward ? PCA9685_PWM_MAX : PCA9685_PWM_MIN);
    pca_->setPWM(chBk_, 0, forward ? PCA9685_PWM_MIN : PCA9685_PWM_MAX);
}

GimbalMotor::GimbalMotor(uint8_t in1, uint8_t in2, uint8_t in3, uint8_t polePairs, bool isMock)
    : MotorBase(isMock),
      motor_(polePairs),
      driver_(in1, in2, in3),
      initialized_(false) {}

bool GimbalMotor::begin() {
    if (isMockActive()) {
        Serial.println("Mock GimbalMotor begin");
        return true;
    }

    driver_.voltage_power_supply = GIMBAL_SUPPLY_V;
    driver_.init();

    motor_.linkDriver(&driver_);
    motor_.controller = MotionControlType::velocity_openloop;
    motor_.init();

    initialized_ = true;
    return true;
}

void GimbalMotor::setSpeed(float speed) {
    if (isMockActive()) {
        Serial.print("Mock GimbalMotor setSpeed: ");
        Serial.println(speed, 4);
        return;
    }
    if (!initialized_) {
        return;
    }

    float clamped = clampSpeed(speed);
    motor_.move(clamped);
}

VerticalMotor::VerticalMotor(FlexCanBus *bus, uint32_t commandId, bool isMock)
    : MotorBase(isMock), bus_(bus), commandId_(commandId) {}

bool VerticalMotor::begin() {
    if (isMockActive()) {
        Serial.println("Mock VerticalMotor begin");
        return true;
    }
    if (!bus_) {
        return false;
    }
    bus_->begin();
    return true;
}

void VerticalMotor::setSpeed(float speed) {
    if (isMockActive()) {
        Serial.print("Mock VerticalMotor setSpeed: ");
        Serial.println(speed, 4);
        return;
    }
    if (!bus_) {
        return;
    }

    float clamped = clampSpeed(speed);
    int16_t scaled = static_cast<int16_t>(clamped * VERTICAL_SPEED_SCALE);

    CAN_message_t msg;
    msg.id = commandId_;
    msg.len = 2;
    msg.buf[0] = static_cast<uint8_t>(scaled & 0xFF);
    msg.buf[1] = static_cast<uint8_t>((scaled >> 8) & 0xFF);

    bus_->write(msg);
}
