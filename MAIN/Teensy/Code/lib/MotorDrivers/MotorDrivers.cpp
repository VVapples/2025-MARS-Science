#include "MotorDrivers.h"

#include <math.h>

MotorBase::MotorBase(bool isMock) : isMock_(isMock) {}

void MotorBase::stop() {
    setSpeed(0.0f);
    engageBrake(); // only if available
}

void MotorBase::engageBrake() {}

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

    // BLD-510B (New Version) Logic
    // SV: Supplied PWM for speed control
    // FR: Direction (High/Low)
    // EN: Enable (High = Run, Low = Stop)
    // BK: Brake (Low = Run/Release, High = Brake)

    // brakes will be released when speed = 0 allowing for freespin, for hard brake use engageBrake()

    pca_->setPWM(chSv_, 0, pwm);
    pca_->setPWM(chFr_, 0, forward ? PCA9685_PWM_MAX : PCA9685_PWM_MIN);
    pca_->setPWM(chBk_, 0, PCA9685_PWM_MIN);

    if (pwm > 0) {
        pca_->setPWM(chEn_, 0, PCA9685_PWM_MAX);
    } else {
        pca_->setPWM(chEn_, 0, PCA9685_PWM_MIN);
    }
}

void DrillMotor::engageBrake() {
    // hard brake the motor
    // use setSpeed(0) to release brake and allow freespin (or just any values other than 0)
    if (isMockActive()) {
        Serial.println("Mock DrillMotor engageBrake");
        return;
    }
    if (!pca_) {
        return;
    }
    pca_->setPWM(chSv_, 0, PCA9685_PWM_MIN);
    pca_->setPWM(chEn_, 0, PCA9685_PWM_MAX);
    pca_->setPWM(chBk_, 0, PCA9685_PWM_MAX);
}

PumpMotor::PumpMotor(Adafruit_PWMServoDriver *pca, bool isMock)
    : MotorBase(isMock),
      pca_(pca),
      enChannels_{CH_E2_M_S1_EN,
                  CH_E2_M_S2_EN,
                  CH_E2_M_S3_EN,
                  CH_E2_M_S4_EN,
                  CH_E2_M_S5_EN,
                  CH_E2_M_S6_EN,
                  CH_E2_M_S7_EN},
      phChannels_{CH_E2_M_S1_PH,
                  CH_E2_M_S2_PH,
                  CH_E2_M_S3_PH,
                  CH_E2_M_S4_PH,
                  CH_E2_M_S5_PH,
                  CH_E2_M_S6_PH,
                  CH_E2_M_S7_PH} {}

bool PumpMotor::begin() {
    if (isMockActive()) {
        Serial.println("Mock PumpMotor begin");
        return true;
    }
    if (!pca_) {
        return false;
    }
    for (size_t i = 0; i < kPumpCount; ++i) {
        pca_->setPWM(enChannels_[i], 0, PCA9685_PWM_MIN);
        pca_->setPWM(phChannels_[i], 0, PCA9685_PWM_MIN);
    }
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

    for (size_t i = 0; i < kPumpCount; ++i) {
        pca_->setPWM(phChannels_[i], 0, forward ? PCA9685_PWM_MAX : PCA9685_PWM_MIN);
        pca_->setPWM(enChannels_[i], 0, pwm);
    }
}

namespace {
uint16_t ServoPulseToCounts(uint16_t pulseUs) {
    const float periodUs = 1000000.0f / static_cast<float>(PCA9685_PWM_FREQ_HZ);
    float counts = (static_cast<float>(pulseUs) / periodUs) * PCA9685_PWM_MAX;
    if (counts < PCA9685_PWM_MIN) {
        return PCA9685_PWM_MIN;
    }
    if (counts > PCA9685_PWM_MAX) {
        return PCA9685_PWM_MAX;
    }
    return static_cast<uint16_t>(counts);
}
}

GimbalMotor::GimbalMotor(Adafruit_PWMServoDriver *pca, uint8_t signalChannel, bool isMock)
    : MotorBase(isMock), pca_(pca), signalChannel_(signalChannel) {}

bool GimbalMotor::begin() {
    if (isMockActive()) {
        Serial.println("Mock GimbalMotor begin");
        return true;
    }
    if (!pca_) {
        return false;
    }

    const uint16_t neutral = ServoPulseToCounts(SERVO_PULSE_NEUTRAL_US);
    pca_->setPWM(signalChannel_, 0, neutral);
    return true;
}

void GimbalMotor::setSpeed(float speed) {
    if (isMockActive()) {
        Serial.print("Mock GimbalMotor setSpeed: ");
        Serial.println(speed, 4);
        return;
    }
    if (!pca_) {
        return;
    }

    float clamped = clampSpeed(speed);
    const float maxSpan = static_cast<float>(SERVO_PULSE_MAX_US - SERVO_PULSE_NEUTRAL_US);
    const float minSpan = static_cast<float>(SERVO_PULSE_NEUTRAL_US - SERVO_PULSE_MIN_US);
    float pulseUs = static_cast<float>(SERVO_PULSE_NEUTRAL_US);
    if (clamped >= 0.0f) {
        pulseUs += clamped * maxSpan;
    } else {
        pulseUs += clamped * minSpan;
    }

    pca_->setPWM(signalChannel_, 0, ServoPulseToCounts(static_cast<uint16_t>(pulseUs)));
}

VerticalMotor::VerticalMotor(uint8_t in1,
                             uint8_t in2,
                             uint8_t in3,
                             uint8_t polePairs,
                             Adafruit_PWMServoDriver *pca,
                             uint8_t enableChannel,
                             bool isMock)
    : MotorBase(isMock),
      motor_(polePairs),
      driver_(in1, in2, in3),
      pca_(pca),
      enableChannel_(enableChannel),
      initialized_(false) {}

bool VerticalMotor::begin() {
    if (isMockActive()) {
        Serial.println("Mock VerticalMotor begin");
        return true;
    }
    if (!pca_) {
        return false;
    }

    pca_->setPWM(enableChannel_, 0, PCA9685_PWM_MAX);

    driver_.voltage_power_supply = VERTICAL_SUPPLY_V;
    driver_.init();

    motor_.linkDriver(&driver_);
    motor_.controller = MotionControlType::velocity_openloop;
    motor_.init();

    initialized_ = true;
    return true;
}

void VerticalMotor::setSpeed(float speed) {
    if (isMockActive()) {
        Serial.print("Mock VerticalMotor setSpeed: ");
        Serial.println(speed, 4);
        return;
    }
    if (!initialized_) {
        return;
    }

    float clamped = clampSpeed(speed);
    motor_.move(clamped);
}
