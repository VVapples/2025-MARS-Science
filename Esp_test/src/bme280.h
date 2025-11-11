#ifndef BME280_H
#define BME280_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// External sensor object declaration
extern Adafruit_BME280 bme;
extern bool SensorActive;

// Sensor data structure
struct SensorData {
    float temperature;  // Temperature in Celsius
    float humidity;     // Humidity in %
    float pressure;     // Pressure in hPa
    unsigned long timestamp; // Time of reading
};

extern SensorData currentSensorData;

// Function declarations
bool SensorSetup();
bool UpdateSensor();
SensorData GetSensorData();
float GetTemperature();
float GetHumidity();
float GetPressure();

#endif // BME280_H