#include <Arduino.h>
#include "servo.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32 with Servo!");

  if (ServoSetup()) {
    Serial.println("Servo setup failed");
  }
}


void loop() {
  if (ServoActive) {
    try {
      while (true) {
        ServoWrite(1);  // Move servo to one extreme
        delay(1000);    // Wait for a second
        ServoWrite(0);  // Move servo to the center
        delay(1000);    // Wait for a second
        ServoWrite(-1); // Move servo to the other extreme
        delay(1000);    // Wait for a second
      }
    } catch (...) {
      Serial.println("Error during servo operation");
      ServoActive = false;
    }
  }
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}