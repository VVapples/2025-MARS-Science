#include <Arduino.h>
#include "bme280.h"
#include "servo.h"

// Timing variables
unsigned long lastSensorUpdate = 0;
const unsigned long sensorUpdateInterval = 2000; // Update sensor every 2 seconds

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(1000); // Wait for serial to stabilize
  Serial.println("\n=== ESP32 Sensor & Servo System ===");
  
  // Initialize BME280 sensor
  if (SensorSetup()) {
    Serial.println("✓ Sensor initialized successfully");
  } else {
    Serial.println("✗ Sensor initialization failed");
  }
  
  // Initialize servo
  if (!ServoSetup()) {
    Serial.println("✓ Servo initialized successfully");
  } else {
    Serial.println("✗ Servo initialization failed");
  }
  
  Serial.println("===================================\n");
}

void loop() {
  // Update sensor data periodically
  unsigned long currentMillis = millis();
  
  if (currentMillis - lastSensorUpdate >= sensorUpdateInterval) {
    lastSensorUpdate = currentMillis;
    
    if (UpdateSensor()) {
      // Print sensor data to serial
      printf("Temperature: %.2f °C, Humidity: %.2f %%, Pressure: %.2f hPa\n",
             GetTemperature(),
             GetHumidity(),
             GetPressure());
      
      // Example: Control servo based on temperature
      float temp = GetTemperature();
      
      if (temp < 20.0) {
        ServoWrite(-1); // Move servo in one direction (full speed reverse)
        Serial.println("Temp < 20°C: Servo moving reverse");
      } else if (temp > 25.0) {
        ServoWrite(1);  // Move servo in other direction (full speed forward)
        Serial.println("Temp > 25°C: Servo moving forward");
      } else {
        ServoWrite(0);  // Stop servo
        Serial.println("Temp 20-25°C: Servo stopped");
      }
    }
  }
  
  delay(10); // Small delay to prevent overwhelming the CPU
}