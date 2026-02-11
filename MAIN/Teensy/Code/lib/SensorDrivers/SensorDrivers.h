#ifndef SENSOR_DRIVERS_H
#define SENSOR_DRIVERS_H

#include <Arduino.h>
#include <Adafruit_BME280.h>

struct BME280Reading {
  float temperatureC;
  float pressurePa;
  float humidityPct;
};

class SoilSensor {
public:
  SoilSensor(int analogPin, bool isMock);

  void begin();
  int readRaw();
  float readPercent();

private:
  int analogPin_;
  bool isMock_;
};

class BME280_Wrapper {
public:
  BME280_Wrapper(uint8_t i2cAddr, bool isMock);

  bool begin();
  BME280Reading read();

private:
  uint8_t i2cAddr_;
  bool isMock_;
  Adafruit_BME280 bme_;
};

#endif  // SENSOR_DRIVERS_H
