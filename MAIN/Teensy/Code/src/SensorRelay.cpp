#include "SensorRelay.h"

SensorRelay::SensorRelay(SoilSensor *soil, BME280_Wrapper *bme)
    : soil_(soil), bme_(bme) {}

void SensorRelay::begin() {
  if (soil_) {
    soil_->begin();
  }
  if (bme_) {
    bme_->begin();
  }
}

SensorSnapshot SensorRelay::readAll() {
  SensorSnapshot snapshot;
  snapshot.soilRaw = -1;
  snapshot.soilPercent = 0.0f;
  snapshot.bme.temperatureC = 0.0f;
  snapshot.bme.pressurePa = 0.0f;
  snapshot.bme.humidityPct = 0.0f;

  if (soil_) {
    snapshot.soilRaw = soil_->readRaw();
    snapshot.soilPercent = soil_->readPercent();
  }

  if (bme_) {
    snapshot.bme = bme_->read();
  }

  return snapshot;
}
