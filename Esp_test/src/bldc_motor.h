#ifndef BLDC_MOTOR_H
#define BLDC_MOTOR_H

#include <Arduino.h>

class BLDCMotor {
  private:
    int enPin;      // Enable Pin
    int frPin;      // Direction Pin
    int svPin;      // Speed Pin (PWM)
    int pwmChannel; // ESP32 PWM Channel (0-15)
    
    // PWM Configuration
    const int pwmFreq = 5000;
    const int pwmResolution = 8;

  public:
    // Constructor
    BLDCMotor(int enablePin, int directionPin, int speedPin, int channel);

    // Initialization (Call this in setup)
    void begin();

    // Main Control Function
    // speed: -255 (Max Reverse) to 255 (Max Forward)
    void setSpeed(int speed);

    // Emergency Stop
    void stop();
};

#endif