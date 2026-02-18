#pragma once

#include <Arduino.h>

#include <SensorDrivers.h>

class SensorRelay {
 public:
  struct Snapshot {
    uint32_t timeMs = 0;
    SensorDrivers::SoilReading soil;
    SensorDrivers::BMEReading bme;

    // Rolling averages over latest window
    float avgSoilPercent = NAN;
    float avgTempC = NAN;
    float avgHumidityPct = NAN;
    float avgPressurePa = NAN;

    bool valid = false;
  };

  static constexpr uint8_t kAveragingWindow = 8;

  SensorRelay(SensorDrivers::SoilMoistureSensor* soil,
              SensorDrivers::BME280Wrapper* bme,
              uint32_t pollPeriodMs = 250);

  bool begin();

  // Non-blocking periodic update; reads sensors when period elapsed.
  void update(uint32_t nowMs);

  // Force immediate refresh (e.g., on SENS command)
  void refreshNow(uint32_t nowMs);

  const Snapshot& latest() const;
  bool hasNewData();

  // CSV helper for comm thread: time,soil,temperature,humidity,pressure
  String latestCsv() const;

 private:
  SensorDrivers::SoilMoistureSensor* soil_;
  SensorDrivers::BME280Wrapper* bme_;

  uint32_t pollPeriodMs_;
  uint32_t lastPollMs_ = 0;
  bool initialized_ = false;
  bool newDataFlag_ = false;

  Snapshot snapshot_;

  float soilHistory_[kAveragingWindow];
  float tempHistory_[kAveragingWindow];
  float humHistory_[kAveragingWindow];
  float presHistory_[kAveragingWindow];
  uint8_t histIndex_ = 0;
  uint8_t histCount_ = 0;

  void pushHistory(float soilPct, float tC, float hPct, float pPa);
  float averageOf(const float* arr, uint8_t count) const;
};
