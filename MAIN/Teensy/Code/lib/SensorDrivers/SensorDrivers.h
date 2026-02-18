#pragma once

#include <Arduino.h>

namespace SensorDrivers {

struct SoilReading {
  int raw = 0;             // ADC counts
  float percent = 0.0f;    // 0..100% mapped estimate
};

struct BMEReading {
  float temperatureC = NAN;
  float humidityPct = NAN;
  float pressurePa = NAN;
  bool valid = false;
};

class SoilMoistureSensor {
 public:
  SoilMoistureSensor(const char* sensorName,
                     uint8_t analogPin,
                     bool isMock,
                     int dryRaw = 1023,
                     int wetRaw = 250);

  bool begin();
  SoilReading read();

 private:
  const char* name_;
  uint8_t analogPin_;
  bool isMock_;
  int dryRaw_;
  int wetRaw_;

  void logRead(const SoilReading& r) const;
};

class BME280Wrapper {
 public:
  BME280Wrapper(const char* sensorName, bool isMock, uint8_t i2cAddr = 0x76);

  bool begin();
  BMEReading read();

 private:
  const char* name_;
  bool isMock_;
  uint8_t i2cAddr_;
  void* bmeHandle_ = nullptr;  // Opaque pointer to Adafruit_BME280 when available

  void logRead(const BMEReading& r) const;
};

}  // namespace SensorDrivers
