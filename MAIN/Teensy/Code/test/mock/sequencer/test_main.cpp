#include <Arduino.h>
#include <unity.h>

#include "CommandSequencer.h"
#include "RoverConfig.h"
#include <MotorDrivers.h>

class SpyMotor : public MotorDrivers::MotorBase {
 public:
  SpyMotor(const char* name, bool isMock)
      : MotorBase(name, isMock) {}

  bool begin() override { return true; }

  void setSpeed(float s) override {
    ++setCount;
    last = s;
    Serial.printf("[SEQ MOCK] setSpeed motor=%s s=%.3f\n", name(), s);
  }

  void stop() override {
    ++stopCount;
    Serial.printf("[SEQ MOCK] stop motor=%s\n", name());
  }

  int setCount = 0;
  int stopCount = 0;
  float last = 0;
};

void test_sequencer_mock_parse_and_execute() {
  CommandSequencer seq;
  SpyMotor m1("M1-SPY", true);

  TEST_ASSERT_TRUE(seq.registerMotorByNumericId(RoverConfig::MotorNumericId::M1, &m1));

  const char* cmd = "G1 M1 S0.5 T200 P100 SENS HB";
  auto r = seq.enqueueFromLine(cmd);
  TEST_ASSERT_TRUE(r.ok);
  TEST_ASSERT_EQUAL_UINT16(4, r.enqueued);

  Serial.printf("[SEQ MOCK] input: %s\n", cmd);
  Serial.println("[SEQ MOCK] expected decode: MOTOR_SET(M1,0.5,200), PAUSE(100), SENS, HB");

  seq.update(0);
  seq.update(100);
  seq.update(220);
  seq.update(350);

  TEST_ASSERT_TRUE(m1.setCount >= 1);
  TEST_ASSERT_TRUE(m1.stopCount >= 1);
  TEST_ASSERT_TRUE(seq.consumeSensorRequestFlag());
  TEST_ASSERT_TRUE(seq.consumeHeartbeatFlag());
}

void setup() {
  Serial.begin(115200);
  delay(500);

  UNITY_BEGIN();
  RUN_TEST(test_sequencer_mock_parse_and_execute);
  UNITY_END();
}

void loop() {}
