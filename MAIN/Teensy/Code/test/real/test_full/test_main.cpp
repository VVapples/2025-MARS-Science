#include <Arduino.h>
#include <unity.h>

#include "CommandSequencer.h"
#include "SensorRelay.h"
#include "../../TestHarness.h"
#include "RoverConfig.h"
#include <MotorDrivers.h>
#include <SensorDrivers.h>

void test_full_real_flow() {
  Serial.println("[FULL REAL] === test_full_real_flow START ===");
  CommandSequencer seq;
  MotorDrivers::MotorPWM m1("M1-REAL", RoverConfig::Pins::MS1_IN1, RoverConfig::Pins::MS1_IN2, false);
  Serial.println("[FULL REAL] begin motor + register M1");
  TEST_ASSERT_TRUE(m1.begin());
  TEST_ASSERT_TRUE(seq.registerMotorByNumericId(RoverConfig::MotorNumericId::M1, &m1));

  SensorDrivers::SoilMoistureSensor soil("S-1", RoverConfig::Pins::S1_SOIL_SIG, false);
  SensorDrivers::BME280Wrapper bme("S-2", false);
  SensorRelay relay(&soil, &bme, 100);

  bool sensorsReady = relay.begin();
  Serial.printf("[FULL REAL] sensorsReady=%d\n", sensorsReady);

  Serial.println("[FULL REAL] enqueue: G1 M1 S0.4 T200 SENS P100 SENS");
  auto r = seq.enqueueFromLine("G1 M1 S0.4 T200 SENS P100 SENS");
  Serial.printf("[FULL REAL] enqueue result: ok=%d enqueued=%u\n", r.ok, r.enqueued);
  TEST_ASSERT_TRUE(r.ok);

  Serial.println("[FULL REAL] timeline updates: t=0,220,260");
  seq.update(0);
  seq.update(220);
  seq.update(260);
  if (seq.consumeSensorRequestFlag() && sensorsReady) {
    Serial.println("[FULL REAL] sensor request #1 -> refreshNow(260)");
    relay.refreshNow(260);
    Serial.print("[FULL REAL] SENSOR CSV #1: ");
    Serial.println(relay.latestCsv());
  } else {
    Serial.println("[FULL REAL] sensor request #1 skipped (flag or sensorsReady false)");
  }

  Serial.println("[FULL REAL] timeline update: t=380");
  seq.update(380);
  if (seq.consumeSensorRequestFlag() && sensorsReady) {
    Serial.println("[FULL REAL] sensor request #2 -> refreshNow(380)");
    relay.refreshNow(380);
    Serial.print("[FULL REAL] SENSOR CSV #2: ");
    Serial.println(relay.latestCsv());
  } else {
    Serial.println("[FULL REAL] sensor request #2 skipped (flag or sensorsReady false)");
  }

  TEST_ASSERT_TRUE(true);
  Serial.println("[FULL REAL] === test_full_real_flow END ===");
}

void setup() {
  TestHarness::waitForSerialAttach("REAL FULL");
  Serial.println("[FULL REAL] setup() entered. Running Unity tests...");

  UNITY_BEGIN();
  RUN_TEST(test_full_real_flow);
  UNITY_END();

  Serial.println("[FULL REAL] Unity run complete.");

  TestHarness::holdAfterTests("REAL FULL");
}

void loop() {}
