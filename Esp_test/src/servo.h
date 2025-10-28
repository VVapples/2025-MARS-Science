#ifndef SERVO_H
#define SERVO_H

#include <Arduino.h>
#include <ESP32Servo.h>

// External servo object declaration
extern Servo myServo;
extern bool ServoActive;

// Function declarations
bool ServoSetup();
void ServoWrite(int value);

#endif // SERVO_H
