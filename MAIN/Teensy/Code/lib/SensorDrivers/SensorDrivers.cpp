#include "SensorDrivers.h"

#include <math.h>
#include <Wire.h>

#if __has_include(<Adafruit_BME280.h>)
#include <Adafruit_BME280.h>
#define ROVER_HAS_BME280 1
#else
#define ROVER_HAS_BME280 0
#endif

namespace SensorDrivers {

namespace {
float clamp01(float x) {
  if (x < 0.0f) return 0.0f;
  if (x > 1.0f) return 1.0f;
  return x;
}
}  // namespace

SoilMoistureSensor::SoilMoistureSensor(const char* sensorName,
                                       uint8_t analogPin,
                                       bool isMock,
                                       int dryRaw,
                                       int wetRaw)
    : name_(sensorName),
      analogPin_(analogPin),
      isMock_(isMock),
      dryRaw_(dryRaw),
      wetRaw_(wetRaw) {}

bool SoilMoistureSensor::begin() {
  if (isMock_) {
    Serial.print("[SENSOR][");
    Serial.print(name_);
    Serial.println("] begin mode=MOCK");
    return true;
  }

  pinMode(analogPin_, INPUT);
  Serial.print("[SENSOR][");
  Serial.print(name_);
  Serial.println("] begin mode=REAL");
  return true;
}

SoilReading SoilMoistureSensor::read() {
  SoilReading r;

  if (isMock_) {
    const float t = millis() * 0.001f;
    const float wave01 = 0.5f * (sinf(t * 0.65f) + 1.0f);
    r.percent = 15.0f + 70.0f * wave01;
    r.raw = static_cast<int>(dryRaw_ - ((dryRaw_ - wetRaw_) * (r.percent / 100.0f)));
    logRead(r);
    return r;
  }

  r.raw = analogRead(analogPin_);
  const float ratio = clamp01(static_cast<float>(dryRaw_ - r.raw) /
                              static_cast<float>(dryRaw_ - wetRaw_));
  r.percent = ratio * 100.0f;

  logRead(r);
  return r;
}

void SoilMoistureSensor::logRead(const SoilReading& r) const {
  Serial.print("[SENSOR][");
  Serial.print(name_);
  Serial.print("] raw=");
  Serial.print(r.raw);
  Serial.print(" moisture=");
  Serial.print(r.percent, 2);
  Serial.print(" mode=");
  Serial.println(isMock_ ? "MOCK" : "REAL");
}

BME280Wrapper::BME280Wrapper(const char* sensorName, bool isMock, uint8_t i2cAddr)
    : name_(sensorName), isMock_(isMock), i2cAddr_(i2cAddr) {}

bool BME280Wrapper::begin() {
  if (isMock_) {
    Serial.print("[SENSOR][");
    Serial.print(name_);
    Serial.println("] begin mode=MOCK");
    return true;
  }

#if ROVER_HAS_BME280
  auto* bme = new Adafruit_BME280();
  Wire.begin();

  if (!bme->begin(i2cAddr_, &Wire)) {
    delete bme;
    Serial.print("[SENSOR][");
    Serial.print(name_);
    Serial.println("] BME280 begin failed");
    return false;
  }

  bmeHandle_ = static_cast<void*>(bme);
  Serial.print("[SENSOR][");
  Serial.print(name_);
  Serial.println("] begin mode=REAL");
  return true;
#else
  Serial.println("[SENSOR][BME280Wrapper] Adafruit_BME280 not found; forcing mock behavior at runtime.");
  return false;
#endif
}

BMEReading BME280Wrapper::read() {
  BMEReading r;

  if (isMock_) {
    const float t = millis() * 0.001f;
    r.temperatureC = 19.0f + 4.5f * sinf(t * 0.20f);
    r.humidityPct = 45.0f + 18.0f * sinf(t * 0.13f + 0.6f);
    r.pressurePa = 101325.0f + 900.0f * sinf(t * 0.08f + 1.2f);
    r.valid = true;
    logRead(r);
    return r;
  }

#if ROVER_HAS_BME280
  auto* bme = static_cast<Adafruit_BME280*>(bmeHandle_);
  if (bme == nullptr) {
    logRead(r);
    return r;
  }

  r.temperatureC = bme->readTemperature();
  r.humidityPct = bme->readHumidity();
  r.pressurePa = bme->readPressure();
  r.valid = !isnan(r.temperatureC) && !isnan(r.humidityPct) && !isnan(r.pressurePa);
  logRead(r);
  return r;
#else
  logRead(r);
  return r;
#endif
}

void BME280Wrapper::logRead(const BMEReading& r) const {
  Serial.print("[SENSOR][");
  Serial.print(name_);
  Serial.print("] T=");
  Serial.print(r.temperatureC, 2);
  Serial.print("C H=");
  Serial.print(r.humidityPct, 2);
  Serial.print("% P=");
  Serial.print(r.pressurePa, 2);
  Serial.print("Pa valid=");
  Serial.print(r.valid ? "1" : "0");
  Serial.print(" mode=");
  Serial.println(isMock_ ? "MOCK" : "REAL");
}

}  // namespace SensorDrivers
