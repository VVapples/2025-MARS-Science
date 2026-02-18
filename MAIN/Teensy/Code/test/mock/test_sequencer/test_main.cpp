#include <Arduino.h>
#include <unity.h>

#include "CommandSequencer.h"
#include "../../TestHarness.h"
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

void test_sequencer_mock_single_examples_from_gcode_md() {
  Serial.println("[SEQ MOCK] === test_sequencer_mock_single_examples_from_gcode_md START ===");

  // Example: G1 MS1 S0.5
  {
    CommandSequencer seq;
    SpyMotor ms1("MS1-SPY", true);
    TEST_ASSERT_TRUE(seq.registerMotorByToken("MS1", &ms1));

    auto r = seq.enqueueFromLine("G1 MS1 S0.5");
    Serial.printf("[SEQ MOCK] ex: G1 MS1 S0.5 -> ok=%d enq=%u\n", r.ok, r.enqueued);
    TEST_ASSERT_TRUE(r.ok);
    TEST_ASSERT_EQUAL_UINT16(1, r.enqueued);

    seq.update(0);
    TEST_ASSERT_EQUAL_INT(1, ms1.setCount);
    TEST_ASSERT_EQUAL_INT(0, ms1.stopCount);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.5f, ms1.last);
  }

  // Example: G1 MD1 S-0.25 T3000
  {
    CommandSequencer seq;
    SpyMotor md1("MD1-SPY", true);
    TEST_ASSERT_TRUE(seq.registerMotorByToken("MD1", &md1));

    auto r = seq.enqueueFromLine("G1 MD1 S-0.25 T3000");
    Serial.printf("[SEQ MOCK] ex: G1 MD1 S-0.25 T3000 -> ok=%d enq=%u\n", r.ok, r.enqueued);
    TEST_ASSERT_TRUE(r.ok);
    TEST_ASSERT_EQUAL_UINT16(1, r.enqueued);

    seq.update(0);
    TEST_ASSERT_EQUAL_INT(1, md1.setCount);
    TEST_ASSERT_EQUAL_INT(0, md1.stopCount);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, -0.25f, md1.last);

    seq.update(3100);
    TEST_ASSERT_TRUE(md1.stopCount >= 1);
  }

  // Example: G1 MA2 S0.75 T1000
  {
    CommandSequencer seq;
    SpyMotor ma2("MA2-SPY", true);
    TEST_ASSERT_TRUE(seq.registerMotorByToken("MA2", &ma2));

    auto r = seq.enqueueFromLine("G1 MA2 S0.75 T1000");
    Serial.printf("[SEQ MOCK] ex: G1 MA2 S0.75 T1000 -> ok=%d enq=%u\n", r.ok, r.enqueued);
    TEST_ASSERT_TRUE(r.ok);
    TEST_ASSERT_EQUAL_UINT16(1, r.enqueued);

    seq.update(0);
    TEST_ASSERT_EQUAL_INT(1, ma2.setCount);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.75f, ma2.last);

    seq.update(1100);
    TEST_ASSERT_TRUE(ma2.stopCount >= 1);
  }

  Serial.println("[SEQ MOCK] === test_sequencer_mock_single_examples_from_gcode_md END ===");
}

void test_sequencer_mock_control_tokens_from_gcode_md() {
  Serial.println("[SEQ MOCK] === test_sequencer_mock_control_tokens_from_gcode_md START ===");
  CommandSequencer seq;
  SpyMotor m1("M1-SPY", true);
  TEST_ASSERT_TRUE(seq.registerMotorByNumericId(RoverConfig::MotorNumericId::M1, &m1));

  // STOP token example
  auto rStop = seq.enqueueFromLine("G1 M1 S0.6 T1000 STOP");
  Serial.printf("[SEQ MOCK] ex: ... STOP -> ok=%d enq=%u\n", rStop.ok, rStop.enqueued);
  TEST_ASSERT_TRUE(rStop.ok);
  TEST_ASSERT_EQUAL_UINT16(2, rStop.enqueued);  // MOTOR_SET + STOP_ALL

  seq.update(0);
  TEST_ASSERT_TRUE(m1.setCount >= 1);
  TEST_ASSERT_EQUAL_INT(0, m1.stopCount);

  // STOP executes after the timed motor action elapses in current sequencer model.
  seq.update(1200);
  TEST_ASSERT_TRUE(m1.stopCount >= 1);
  TEST_ASSERT_EQUAL_UINT8(0, seq.queuedCount());

  // SENS / HB / P1000 examples
  auto rCtl = seq.enqueueFromLine("SENS HB P1000");
  Serial.printf("[SEQ MOCK] ex: SENS HB P1000 -> ok=%d enq=%u\n", rCtl.ok, rCtl.enqueued);
  TEST_ASSERT_TRUE(rCtl.ok);
  TEST_ASSERT_EQUAL_UINT16(3, rCtl.enqueued);

  seq.update(1300);
  TEST_ASSERT_TRUE(seq.consumeSensorRequestFlag());
  TEST_ASSERT_TRUE(seq.consumeHeartbeatFlag());
  TEST_ASSERT_EQUAL_UINT8(1, seq.queuedCount());  // PAUSE still active

  seq.update(2400);
  TEST_ASSERT_EQUAL_UINT8(0, seq.queuedCount());

  Serial.println("[SEQ MOCK] === test_sequencer_mock_control_tokens_from_gcode_md END ===");
}

void test_sequencer_mock_multiline_examples_from_gcode_md() {
  Serial.println("[SEQ MOCK] === test_sequencer_mock_multiline_examples_from_gcode_md START ===");
  CommandSequencer seq;
  SpyMotor ma1("MA1-SPY", true);
  SpyMotor ma2("MA2-SPY", true);

  TEST_ASSERT_TRUE(seq.registerMotorByToken("MA1", &ma1));
  TEST_ASSERT_TRUE(seq.registerMotorByToken("MA2", &ma2));

  // ex2 line 1
  auto r1 = seq.enqueueFromLine("G1 MA1 S-1.0 T1000 P1000 G1 MA1 S-1.0 T1000");
  Serial.printf("[SEQ MOCK] ex2 line1 -> ok=%d enq=%u\n", r1.ok, r1.enqueued);
  TEST_ASSERT_TRUE(r1.ok);
  TEST_ASSERT_EQUAL_UINT16(3, r1.enqueued);

  // ex2 line 2
  auto r2 = seq.enqueueFromLine("G2 MA2 S1 T1500 P500 G1 MA2 S-1 T1000");
  Serial.printf("[SEQ MOCK] ex2 line2 -> ok=%d enq=%u\n", r2.ok, r2.enqueued);
  TEST_ASSERT_TRUE(r2.ok);
  TEST_ASSERT_EQUAL_UINT16(3, r2.enqueued);

  // Sensor pulse line (shortened version of repeated pattern)
  auto r3 = seq.enqueueFromLine("SENS P1000 SENS P1000 SENS P1000");
  Serial.printf("[SEQ MOCK] sensor line -> ok=%d enq=%u\n", r3.ok, r3.enqueued);
  TEST_ASSERT_TRUE(r3.ok);
  TEST_ASSERT_EQUAL_UINT16(6, r3.enqueued);

  // Drive time forward so queued actions can execute
  const uint32_t timeline[] = {
      0, 1000, 2000, 3000, 4500, 6000, 7000,
      8000, 9000, 10000, 11000, 12000, 13000};
  for (uint8_t i = 0; i < (sizeof(timeline) / sizeof(timeline[0])); ++i) {
    Serial.printf("[SEQ MOCK] update(t=%lu)\n", (unsigned long)timeline[i]);
    seq.update(timeline[i]);
  }

  Serial.printf("[SEQ MOCK] MA1 set=%d stop=%d | MA2 set=%d stop=%d\n",
                ma1.setCount,
                ma1.stopCount,
                ma2.setCount,
                ma2.stopCount);

  TEST_ASSERT_TRUE(ma1.setCount >= 2);
  TEST_ASSERT_TRUE(ma1.stopCount >= 2);
  TEST_ASSERT_TRUE(ma2.setCount >= 2);
  TEST_ASSERT_TRUE(ma2.stopCount >= 2);
  TEST_ASSERT_EQUAL_UINT8(0, seq.queuedCount());

  Serial.println("[SEQ MOCK] === test_sequencer_mock_multiline_examples_from_gcode_md END ===");
}

void test_sequencer_mock_parse_and_execute() {
  Serial.println("[SEQ MOCK] === test_sequencer_mock_parse_and_execute START ===");
  CommandSequencer seq;
  SpyMotor m1("M1-SPY", true);

  Serial.println("[SEQ MOCK] register M1 spy motor");
  TEST_ASSERT_TRUE(seq.registerMotorByNumericId(RoverConfig::MotorNumericId::M1, &m1));

  const char* cmd = "G1 M1 S0.5 T200 P100 SENS HB";
  Serial.printf("[SEQ MOCK] enqueue line: %s\n", cmd);
  auto r = seq.enqueueFromLine(cmd);
  Serial.printf("[SEQ MOCK] enqueue result: ok=%d enqueued=%u\n", r.ok, r.enqueued);
  TEST_ASSERT_TRUE(r.ok);
  TEST_ASSERT_EQUAL_UINT16(4, r.enqueued);

  Serial.printf("[SEQ MOCK] input: %s\n", cmd);
  Serial.println("[SEQ MOCK] expected decode: MOTOR_SET(M1,0.5,200), PAUSE(100), SENS, HB");

  Serial.println("[SEQ MOCK] running sequencer timeline: t=0,100,220,350");
  seq.update(0);
  seq.update(100);
  seq.update(220);
  seq.update(350);

  Serial.printf("[SEQ MOCK] spy counts: set=%d stop=%d last=%.3f\n", m1.setCount, m1.stopCount, m1.last);

  TEST_ASSERT_TRUE(m1.setCount >= 1);
  TEST_ASSERT_TRUE(m1.stopCount >= 1);
  TEST_ASSERT_TRUE(seq.consumeSensorRequestFlag());
  TEST_ASSERT_TRUE(seq.consumeHeartbeatFlag());
  Serial.println("[SEQ MOCK] flags consumed: SENS=1 HB=1");
  Serial.println("[SEQ MOCK] === test_sequencer_mock_parse_and_execute END ===");
}

void setup() {
  TestHarness::waitForSerialAttach("MOCK SEQ");
  Serial.println("[SEQ MOCK] setup() entered. Running Unity tests...");

  UNITY_BEGIN();
  RUN_TEST(test_sequencer_mock_parse_and_execute);
  RUN_TEST(test_sequencer_mock_single_examples_from_gcode_md);
  RUN_TEST(test_sequencer_mock_control_tokens_from_gcode_md);
  RUN_TEST(test_sequencer_mock_multiline_examples_from_gcode_md);
  UNITY_END();

  Serial.println("[SEQ MOCK] Unity run complete.");

  TestHarness::holdAfterTests("MOCK SEQ");
}

void loop() {}
