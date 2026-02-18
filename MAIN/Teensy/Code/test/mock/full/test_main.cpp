#include <Arduino.h>
#include <unity.h>

#include "CommandSequencer.h"
#include "SensorRelay.h"
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
  CommandSequencer seq;
  SpyMotor m1("M1-SPY", true);
  TEST_ASSERT_TRUE(seq.registerMotorByNumericId(RoverConfig::MotorNumericId::M1, &m1));

  SensorDrivers::SoilMoistureSensor soil("S-1", RoverConfig::Pins::S1_SOIL_SIG, true);
  SensorDrivers::BME280Wrapper bme("S-2", true);
  SensorRelay relay(&soil, &bme, 100);

  TEST_ASSERT_TRUE(relay.begin());

  auto r = seq.enqueueFromLine("G1 M1 S0.7 T100 SENS");
  TEST_ASSERT_TRUE(r.ok);

  seq.update(0);
  seq.update(120);
  seq.update(130);

  if (seq.consumeSensorRequestFlag()) {
    relay.refreshNow(130);
    Serial.print("[FULL MOCK] SENSOR CSV: ");
    Serial.println(relay.latestCsv());
  }

  TEST_ASSERT_TRUE(m1.setCount >= 1);
  TEST_ASSERT_TRUE(m1.stopCount >= 1);
  TEST_ASSERT_TRUE(relay.latest().bme.valid);
}

void setup() {
  Serial.begin(115200);
  delay(500);

  UNITY_BEGIN();
  RUN_TEST(test_full_mock_flow);
  UNITY_END();
}

void loop() {}
