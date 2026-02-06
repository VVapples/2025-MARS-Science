#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>

#include "RoverConfig.h"

class SensorBase {
public:
    explicit SensorBase(bool isMock);
    virtual ~SensorBase() = default;

    virtual bool begin();

protected:
    bool isMockActive() const;
    float mockSine(float minValue, float maxValue, float phase) const;

    bool isMock_;
};

class SoilSensor : public SensorBase {
public:
    SoilSensor(uint8_t analogPin, bool isMock);

    bool begin() override;
    float readMoisture();

private:
    uint8_t analogPin_;
};

class BME280_Wrapper : public SensorBase {
public:
    BME280_Wrapper(TwoWire *wire, uint8_t address, bool isMock);

    bool begin();
    float readTemperatureC();
    float readHumidityPct();
    float readPressurePa();

private:
    Adafruit_BME280 bme_;
    TwoWire *wire_;
    uint8_t address_;
};
