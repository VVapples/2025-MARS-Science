#ifndef SENSOR_RELAY_H
#define SENSOR_RELAY_H

#include <Arduino.h>

#include "SensorDrivers.h"

struct SensorSnapshot {
  int soilRaw;
  float soilPercent;
  BME280Reading bme;
};

class SensorRelay {
public:
  SensorRelay(SoilSensor *soil, BME280_Wrapper *bme);

  void begin();
  SensorSnapshot readAll();

private:
  SoilSensor *soil_;
  BME280_Wrapper *bme_;
};

#endif  // SENSOR_RELAY_H
