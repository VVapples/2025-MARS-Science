#include "SensorDrivers.h"

#include <math.h>

// ----------------------------
// SoilSensor
// ----------------------------
SoilSensor::SoilSensor(int analogPin, bool isMock)
    : analogPin_(analogPin), isMock_(isMock) {}

void SoilSensor::begin() {
  if (isMock_) {
    Serial.println("[MOCK] SoilSensor begin");
    return;
  }
  pinMode(analogPin_, INPUT);
}

int SoilSensor::readRaw() {
  if (isMock_) {
    float t = (float)(millis() % 5000) / 5000.0f;
    float val = 512.0f + 400.0f * sinf(2.0f * 3.1415926f * t);
    return (int)val;
  }
  return analogRead(analogPin_);
}

float SoilSensor::readPercent() {
  int raw = readRaw();
  float pct = (float)raw / 1023.0f * 100.0f;
  if (pct < 0.0f) pct = 0.0f;
  if (pct > 100.0f) pct = 100.0f;
  return pct;
}

// ----------------------------
// BME280_Wrapper
// ----------------------------
BME280_Wrapper::BME280_Wrapper(uint8_t i2cAddr, bool isMock)
    : i2cAddr_(i2cAddr), isMock_(isMock), bme_() {}

bool BME280_Wrapper::begin() {
  if (isMock_) {
    Serial.println("[MOCK] BME280 begin");
    return true;
  }
  return bme_.begin(i2cAddr_);
}

BME280Reading BME280_Wrapper::read() {
  if (isMock_) {
    float t = (float)(millis() % 10000) / 10000.0f;
    BME280Reading mock;
    mock.temperatureC = 20.0f + 5.0f * sinf(2.0f * 3.1415926f * t);
    mock.pressurePa = 101325.0f + 800.0f * sinf(2.0f * 3.1415926f * t);
    mock.humidityPct = 50.0f + 20.0f * sinf(2.0f * 3.1415926f * t);
    return mock;
  }

  BME280Reading reading;
  reading.temperatureC = bme_.readTemperature();
  reading.pressurePa = bme_.readPressure();
  reading.humidityPct = bme_.readHumidity();
  return reading;
}
