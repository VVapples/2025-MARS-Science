#include <Arduino.h>
#include <unity.h>

#include "CommandSequencer.h"
#include "SensorRelay.h"
#include "../../TestHarness.h"
#include "RoverConfig.h"
#include <MotorDrivers.h>
#include <SensorDrivers.h>

class SpyMotor : public MotorDrivers::MotorBase {
 public:
  SpyMotor(const char* name, bool isMock)
      : MotorBase(name, isMock) {}

  bool begin() override { return true; }

  void setSpeed(float s) override {
    ++setCount;
    Serial.printf("[FULL MOCK] setSpeed %s %.3f\n", name(), s);
  }

  void stop() override {
    ++stopCount;
    Serial.printf("[FULL MOCK] stop %s\n", name());
  }

  int setCount = 0;
  int stopCount = 0;
};

void test_full_mock_flow() {
  Serial.println("[FULL MOCK] === test_full_mock_flow START ===");
  CommandSequencer seq;
  SpyMotor m1("M1-SPY", true);
  Serial.println("[FULL MOCK] register M1 spy motor");
  TEST_ASSERT_TRUE(seq.registerMotorByNumericId(RoverConfig::MotorNumericId::M1, &m1));

  SensorDrivers::SoilMoistureSensor soil("S-1", RoverConfig::Pins::S1_SOIL_SIG, true);
  SensorDrivers::BME280Wrapper bme("S-2", true);
  SensorRelay relay(&soil, &bme, 100);

  Serial.println("[FULL MOCK] relay.begin()");
  TEST_ASSERT_TRUE(relay.begin());

  Serial.println("[FULL MOCK] enqueue: G1 M1 S0.7 T100 SENS");
  auto r = seq.enqueueFromLine("G1 M1 S0.7 T100 SENS");
  Serial.printf("[FULL MOCK] enqueue result: ok=%d enqueued=%u\n", r.ok, r.enqueued);
  TEST_ASSERT_TRUE(r.ok);

  Serial.println("[FULL MOCK] timeline updates: t=0,120,130");
  seq.update(0);
  seq.update(120);
  seq.update(130);

  if (seq.consumeSensorRequestFlag()) {
    Serial.println("[FULL MOCK] sensor request flag set -> refreshNow(130)");
    relay.refreshNow(130);
    Serial.print("[FULL MOCK] SENSOR CSV: ");
    Serial.println(relay.latestCsv());
  } else {
    Serial.println("[FULL MOCK] sensor request flag NOT set");
  }

  Serial.printf("[FULL MOCK] motor counts: set=%d stop=%d\n", m1.setCount, m1.stopCount);
  TEST_ASSERT_TRUE(m1.setCount >= 1);
  TEST_ASSERT_TRUE(m1.stopCount >= 1);
  TEST_ASSERT_TRUE(relay.latest().bme.valid);
  Serial.println("[FULL MOCK] latest BME valid=1");
  Serial.println("[FULL MOCK] === test_full_mock_flow END ===");
}

void setup() {
  TestHarness::waitForSerialAttach("MOCK FULL");
  Serial.println("[FULL MOCK] setup() entered. Running Unity tests...");

  UNITY_BEGIN();
  RUN_TEST(test_full_mock_flow);
  UNITY_END();

  Serial.println("[FULL MOCK] Unity run complete.");

  TestHarness::holdAfterTests("MOCK FULL");
}

void loop() {}
