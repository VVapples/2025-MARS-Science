#include "SensorDrivers.h"

#include <math.h>

SensorBase::SensorBase(bool isMock) : isMock_(isMock) {}

bool SensorBase::begin() {
    if (isMockActive()) {
        Serial.println("Mock SensorBase begin");
    }
    return true;
}

bool SensorBase::isMockActive() const {
    if (ROVER_MODE == MODE_FULL_MOCK) {
        return true;
    }
    if (ROVER_MODE == MODE_PARTIAL_MOCK) {
        return isMock_;
    }
    return false;
}

float SensorBase::mockSine(float minValue, float maxValue, float phase) const {
    float span = maxValue - minValue;
    float t = static_cast<float>(millis() % MOCK_WAVE_PERIOD_MS) / MOCK_WAVE_PERIOD_MS;
    float wave = 0.5f * (sinf((t + phase) * 2.0f * PI) + 1.0f);
    return minValue + span * wave;
}

SoilSensor::SoilSensor(uint8_t analogPin, bool isMock)
    : SensorBase(isMock), analogPin_(analogPin) {}

bool SoilSensor::begin() {
    if (isMockActive()) {
        Serial.println("Mock SoilSensor begin");
        return true;
    }
    pinMode(analogPin_, INPUT);
    return true;
}

float SoilSensor::readMoisture() {
    if (isMockActive()) {
        return mockSine(MOCK_SOIL_MIN, MOCK_SOIL_MAX, 0.0f);
    }
    uint16_t raw = analogRead(analogPin_);
    return static_cast<float>(raw) / static_cast<float>(ADC_MAX);
}

BME280_Wrapper::BME280_Wrapper(TwoWire *wire, uint8_t address, bool isMock)
    : SensorBase(isMock), wire_(wire), address_(address) {}

bool BME280_Wrapper::begin() {
    if (isMockActive()) {
        Serial.println("Mock BME280 begin");
        return true;
    }
    if (!wire_) {
        return false;
    }
    return bme_.begin(address_, wire_);
}

float BME280_Wrapper::readTemperatureC() {
    if (isMockActive()) {
        return mockSine(MOCK_TEMP_MIN_C, MOCK_TEMP_MAX_C, 0.0f);
    }
    return bme_.readTemperature();
}

float BME280_Wrapper::readHumidityPct() {
    if (isMockActive()) {
        return mockSine(MOCK_HUMIDITY_MIN_PCT, MOCK_HUMIDITY_MAX_PCT, 0.33f);
    }
    return bme_.readHumidity();
}

float BME280_Wrapper::readPressurePa() {
    if (isMockActive()) {
        return mockSine(MOCK_PRESSURE_MIN_PA, MOCK_PRESSURE_MAX_PA, 0.66f);
    }
    return bme_.readPressure();
}
