#include "bldc_motor.h"

// Constructor: Just saves the pin numbers
BLDCMotor::BLDCMotor(int enablePin, int directionPin, int speedPin, int channel) {
  enPin = enablePin;
  frPin = directionPin;
  svPin = speedPin;
  pwmChannel = channel;
}

// Setup function
void BLDCMotor::begin() {
  pinMode(enPin, OUTPUT);
  pinMode(frPin, OUTPUT);

  // Configure PWM for ESP32
  ledcSetup(pwmChannel, pwmFreq, pwmResolution);
  ledcAttachPin(svPin, pwmChannel);

  // Start disabled
  stop();
}

// Drive function
// Input: -255 to 255
void BLDCMotor::setSpeed(int speed) {
  
  // 1. Handle STOP case
  if (speed == 0) {
    stop();
    return;
  }

  // 2. Handle Direction
  if (speed > 0) {
    digitalWrite(frPin, LOW); // Forward (Check if you need to swap this)
  } else {
    digitalWrite(frPin, HIGH); // Reverse
    speed = -speed;            // Make speed positive for PWM
  }

  // 3. Cap speed at 255
  if (speed > 255) speed = 255;

  // 4. Send PWM Signal
  ledcWrite(pwmChannel, speed);

  // 5. Enable Motor (Active HIGH based on your fix)
  digitalWrite(enPin, HIGH); 
}

// Stop function
void BLDCMotor::stop() {
  digitalWrite(enPin, LOW); // Disable motor (Active HIGH logic)
  ledcWrite(pwmChannel, 0);
}