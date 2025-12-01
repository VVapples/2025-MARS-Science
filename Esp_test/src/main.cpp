#include <Arduino.h>
#include "bldc_motor.h"

// --- PIN DEFINITIONS ---
const int EN_PIN = 23;
const int FR_PIN = 22;
const int SV_PIN = 21;
const int PWM_CH = 0;

// Create the motor object
BLDCMotor myMotor(EN_PIN, FR_PIN, SV_PIN, PWM_CH);

void setup() {
  Serial.begin(115200);
  
  // Initialize the motor
  myMotor.begin();
  
  Serial.println("Motor Initialized.");
  delay(1000);
}

void loop() {
  // 1. Forward at ~60% speed (remember ESP32 3.3V limit)
  Serial.println("Forward...");
  myMotor.setSpeed(255); 
  delay(4000);

  // 2. Stop
  Serial.println("Stopping...");
  myMotor.setSpeed(0);
  delay(2000);

  // 3. Reverse at ~40% speed
  Serial.println("Reverse...");
  myMotor.setSpeed(-150); 
  delay(4000);

  // 4. Stop
  Serial.println("Stopping...");
  myMotor.setSpeed(0);
  delay(2000);
}