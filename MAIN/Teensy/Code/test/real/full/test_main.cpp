#include <Arduino.h>
#include <unity.h>

#include "CommandSequencer.h"
#include "SensorRelay.h"
#include "RoverConfig.h"
#include <MotorDrivers.h>
#include <SensorDrivers.h>

void test_full_real_flow() {
  CommandSequencer seq;
  MotorDrivers::MotorPWM m1("M1-REAL", RoverConfig::Pins::MS1_IN1, RoverConfig::Pins::MS1_IN2, false);
  TEST_ASSERT_TRUE(m1.begin());
  TEST_ASSERT_TRUE(seq.registerMotorByNumericId(RoverConfig::MotorNumericId::M1, &m1));

  SensorDrivers::SoilMoistureSensor soil("S-1", RoverConfig::Pins::S1_SOIL_SIG, false);
  SensorDrivers::BME280Wrapper bme("S-2", false);
  SensorRelay relay(&soil, &bme, 100);

  bool sensorsReady = relay.begin();
  Serial.printf("[FULL REAL] sensorsReady=%d\n", sensorsReady);

  auto r = seq.enqueueFromLine("G1 M1 S0.4 T200 SENS P100 SENS");
  TEST_ASSERT_TRUE(r.ok);

  seq.update(0);
  seq.update(220);
  seq.update(260);
  if (seq.consumeSensorRequestFlag() && sensorsReady) {
    relay.refreshNow(260);
    Serial.print("[FULL REAL] SENSOR CSV #1: ");
    Serial.println(relay.latestCsv());
  }

  seq.update(380);
  if (seq.consumeSensorRequestFlag() && sensorsReady) {
    relay.refreshNow(380);
    Serial.print("[FULL REAL] SENSOR CSV #2: ");
    Serial.println(relay.latestCsv());
  }

  TEST_ASSERT_TRUE(true);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  UNITY_BEGIN();
  RUN_TEST(test_full_real_flow);
  UNITY_END();
}

void loop() {}
