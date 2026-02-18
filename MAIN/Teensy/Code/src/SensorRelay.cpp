#include "SensorRelay.h"

#include <math.h>

SensorRelay::SensorRelay(SensorDrivers::SoilMoistureSensor* soil,
                         SensorDrivers::BME280Wrapper* bme,
                         uint32_t pollPeriodMs)
    : soil_(soil), bme_(bme), pollPeriodMs_(pollPeriodMs) {
  for (uint8_t i = 0; i < kAveragingWindow; ++i) {
    soilHistory_[i] = NAN;
    tempHistory_[i] = NAN;
    humHistory_[i] = NAN;
    presHistory_[i] = NAN;
  }
}

bool SensorRelay::begin() {
  bool ok = true;

  if (soil_ != nullptr) {
    ok = soil_->begin() && ok;
  }

  if (bme_ != nullptr) {
    ok = bme_->begin() && ok;
  }

  initialized_ = ok;
  return ok;
}

void SensorRelay::update(uint32_t nowMs) {
  if (!initialized_) return;
  if ((nowMs - lastPollMs_) < pollPeriodMs_) return;
  refreshNow(nowMs);
}

void SensorRelay::refreshNow(uint32_t nowMs) {
  if (!initialized_) return;

  if (soil_ != nullptr) {
    snapshot_.soil = soil_->read();
  }
  if (bme_ != nullptr) {
    snapshot_.bme = bme_->read();
  }

  snapshot_.timeMs = nowMs;
  snapshot_.valid = snapshot_.bme.valid;

  pushHistory(snapshot_.soil.percent,
              snapshot_.bme.temperatureC,
              snapshot_.bme.humidityPct,
              snapshot_.bme.pressurePa);

  snapshot_.avgSoilPercent = averageOf(soilHistory_, histCount_);
  snapshot_.avgTempC = averageOf(tempHistory_, histCount_);
  snapshot_.avgHumidityPct = averageOf(humHistory_, histCount_);
  snapshot_.avgPressurePa = averageOf(presHistory_, histCount_);

  lastPollMs_ = nowMs;
  newDataFlag_ = true;
}

const SensorRelay::Snapshot& SensorRelay::latest() const {
  return snapshot_;
}

bool SensorRelay::hasNewData() {
  const bool v = newDataFlag_;
  newDataFlag_ = false;
  return v;
}

String SensorRelay::latestCsv() const {
  String s;
  s.reserve(96);

  s += String(snapshot_.timeMs);
  s += ",";
  s += String(snapshot_.soil.percent, 2);
  s += ",";
  s += String(snapshot_.bme.temperatureC, 2);
  s += ",";
  s += String(snapshot_.bme.humidityPct, 2);
  s += ",";
  s += String(snapshot_.bme.pressurePa, 2);
  return s;
}

void SensorRelay::pushHistory(float soilPct, float tC, float hPct, float pPa) {
  soilHistory_[histIndex_] = soilPct;
  tempHistory_[histIndex_] = tC;
  humHistory_[histIndex_] = hPct;
  presHistory_[histIndex_] = pPa;

  histIndex_ = static_cast<uint8_t>((histIndex_ + 1) % kAveragingWindow);
  if (histCount_ < kAveragingWindow) ++histCount_;
}

float SensorRelay::averageOf(const float* arr, uint8_t count) const {
  if (count == 0) return NAN;

  float sum = 0.0f;
  uint8_t validCount = 0;
  for (uint8_t i = 0; i < count; ++i) {
    if (!isnan(arr[i])) {
      sum += arr[i];
      ++validCount;
    }
  }

  if (validCount == 0) return NAN;
  return sum / static_cast<float>(validCount);
}
