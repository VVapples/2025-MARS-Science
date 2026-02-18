#include <Arduino.h>
#include <unity.h>

#include "CommandSequencer.h"
#include "RoverConfig.h"
#include <MotorDrivers.h>

void test_sequencer_real_parse_and_execute() {
  CommandSequencer seq;
  MotorDrivers::MotorPWM realMotor("M1-REAL", RoverConfig::Pins::MS1_IN1, RoverConfig::Pins::MS1_IN2, false);

  TEST_ASSERT_TRUE(realMotor.begin());
  TEST_ASSERT_TRUE(seq.registerMotorByNumericId(RoverConfig::MotorNumericId::M1, &realMotor));

  const char* cmd = "G1 M1 S0.3 T300 P100 SENS HB";
  auto r = seq.enqueueFromLine(cmd);
  TEST_ASSERT_TRUE(r.ok);

  Serial.printf("[SEQ REAL] input: %s\n", cmd);
  Serial.println("[SEQ REAL] expected decode: MOTOR_SET(M1,0.3,300), PAUSE(100), SENS, HB");

  seq.update(0);
  seq.update(350);
  seq.update(500);

  TEST_ASSERT_TRUE(seq.consumeSensorRequestFlag());
  TEST_ASSERT_TRUE(seq.consumeHeartbeatFlag());
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  UNITY_BEGIN();
  RUN_TEST(test_sequencer_real_parse_and_execute);
  UNITY_END();
}

void loop() {}
