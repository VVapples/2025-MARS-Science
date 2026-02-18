#include <Arduino.h>
#include <unity.h>

#include "CommandSequencer.h"
#include "../../TestHarness.h"
#include "RoverConfig.h"
#include <MotorDrivers.h>

void test_sequencer_real_parse_and_execute() {
  Serial.println("[SEQ REAL] === test_sequencer_real_parse_and_execute START ===");
  CommandSequencer seq;
  MotorDrivers::MotorPWM realMotor("M1-REAL", RoverConfig::Pins::MS1_IN1, RoverConfig::Pins::MS1_IN2, false);

  Serial.println("[SEQ REAL] begin motor + register M1");
  TEST_ASSERT_TRUE(realMotor.begin());
  TEST_ASSERT_TRUE(seq.registerMotorByNumericId(RoverConfig::MotorNumericId::M1, &realMotor));

  const char* cmd = "G1 M1 S0.3 T300 P100 SENS HB";
  Serial.printf("[SEQ REAL] enqueue line: %s\n", cmd);
  auto r = seq.enqueueFromLine(cmd);
  Serial.printf("[SEQ REAL] enqueue result: ok=%d enqueued=%u\n", r.ok, r.enqueued);
  TEST_ASSERT_TRUE(r.ok);

  Serial.printf("[SEQ REAL] input: %s\n", cmd);
  Serial.println("[SEQ REAL] expected decode: MOTOR_SET(M1,0.3,300), PAUSE(100), SENS, HB");

  Serial.println("[SEQ REAL] timeline updates: t=0,350,500");
  seq.update(0);
  seq.update(350);
  seq.update(500);

  TEST_ASSERT_TRUE(seq.consumeSensorRequestFlag());
  TEST_ASSERT_TRUE(seq.consumeHeartbeatFlag());
  Serial.println("[SEQ REAL] flags consumed: SENS=1 HB=1");
  Serial.println("[SEQ REAL] === test_sequencer_real_parse_and_execute END ===");
}

void setup() {
  TestHarness::waitForSerialAttach("REAL SEQ");
  Serial.println("[SEQ REAL] setup() entered. Running Unity tests...");

  UNITY_BEGIN();
  RUN_TEST(test_sequencer_real_parse_and_execute);
  UNITY_END();

  Serial.println("[SEQ REAL] Unity run complete.");

  TestHarness::holdAfterTests("REAL SEQ");
}

void loop() {}
