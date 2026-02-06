#include "SensorRelay.h"

#include "SensorDrivers.h"

SensorRelay::SensorRelay() : soil_(nullptr), bme_(nullptr), last_{0.0f, 0.0f, 0.0f, 0.0f, 0} {}

void SensorRelay::attachSensors(SoilSensor *soil, BME280_Wrapper *bme) {
    soil_ = soil;
    bme_ = bme;
}

bool SensorRelay::begin() {
    bool ok = true;
    if (soil_) {
        ok = soil_->begin() && ok;
    }
    if (bme_) {
        ok = bme_->begin() && ok;
    }
    return ok;
}

void SensorRelay::update() {
    if (soil_) {
        last_.soilMoisture = soil_->readMoisture();
    }
    if (bme_) {
        last_.temperatureC = bme_->readTemperatureC();
        last_.humidityPct = bme_->readHumidityPct();
        last_.pressurePa = bme_->readPressurePa();
    }
    last_.timestampMs = millis();
}

SensorRelay::Snapshot SensorRelay::latest() const {
    return last_;
}
