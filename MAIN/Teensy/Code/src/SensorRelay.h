#pragma once

#include <Arduino.h>

class SoilSensor;
class BME280_Wrapper;

class SensorRelay {
public:
    struct Snapshot {
        float soilMoisture;
        float temperatureC;
        float humidityPct;
        float pressurePa;
        uint32_t timestampMs;
    };

    SensorRelay();

    void attachSensors(SoilSensor *soil, BME280_Wrapper *bme);
    bool begin();
    void update();

    Snapshot latest() const;

private:
    SoilSensor *soil_;
    BME280_Wrapper *bme_;
    Snapshot last_;
};
