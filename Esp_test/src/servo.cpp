#include <Arduino.h>
#include <ESP32Servo.h>

Servo myServo;
bool ServoActive = false;

bool ServoSetup() {
    try {
        myServo.attach(13, 500, 2400); // Attach servo to GPIO 13
        myServo.write(90);  // servo speed to 0
        Serial.println("Servo is Active");
        ServoActive = true;
        return false;
    } catch (...) {
        Serial.println("Failed to initialize Servo");
        return true;
    }
}

void ServoWrite(int value) {
    // value : -1 to 1 (speed)
    if (ServoActive) {
        try {
            myServo.write(value * 90 + 90);   // Set Servo Speed
        } catch (...) {
            Serial.println("Error during servo operation");
            ServoActive = false;
        }
    }
}