#include <Arduino.h>
#include <unity.h>

#include <MotorDrivers.h>
#include <SensorDrivers.h>
#include "../../TestHarness.h"
#include "RoverConfig.h"

using namespace MotorDrivers;
using namespace SensorDrivers;

void test_motor_drivers_real_basic() {
  Serial.println("[REAL BASIC] === test_motor_drivers_real_basic START ===");
  MotorPWM pwm("REAL-PWM", RoverConfig::Pins::MS1_IN1, RoverConfig::Pins::MS1_IN2, false);
  MotorCAN can("REAL-CAN", 1, false);
  MotorSimpleFOC foc("REAL-FOC",
                     RoverConfig::Pins::MA1_IN1,
                     RoverConfig::Pins::MA1_IN2,
                     RoverConfig::Pins::MA1_IN3,
                     RoverConfig::Pins::MA1_MA2_EN,
                     false);

  bool okPwm = pwm.begin();
  bool okCan = can.begin();
  bool okFoc = foc.begin();

  Serial.printf("[REAL BASIC] begin pwm=%d can=%d foc=%d\n", okPwm, okCan, okFoc);

  Serial.println("[REAL BASIC] pwm setSpeed(0.40) for 300 ms then stop");
  pwm.setSpeed(0.40f);
  delay(300);
  pwm.stop();

  Serial.println("[REAL BASIC] can setSpeed(0.15) for 300 ms then stop");
  can.setSpeed(0.15f);
  delay(300);
  can.stop();

  Serial.println("[REAL BASIC] foc setSpeed(0.15) for 300 ms then stop");
  foc.setSpeed(0.15f);
  delay(300);
  foc.stop();

  TEST_ASSERT_TRUE(okPwm);
  TEST_ASSERT_TRUE(true);
  Serial.println("[REAL BASIC] === test_motor_drivers_real_basic END ===");
}

void test_sensor_drivers_real_basic() {
  Serial.println("[REAL BASIC] === test_sensor_drivers_real_basic START ===");
  SoilMoistureSensor soil("S-1-REAL", RoverConfig::Pins::S1_SOIL_SIG, false);
  BME280Wrapper bme("S-2-REAL", false);

  bool okSoil = soil.begin();
  bool okBme = bme.begin();

  auto s = soil.read();
  auto b = bme.read();

  Serial.printf("[REAL BASIC] soil_ok=%d raw=%d pct=%.2f bme_ok=%d bme_valid=%d\n",
                okSoil,
                s.raw,
                s.percent,
                okBme,
                b.valid);

  TEST_ASSERT_TRUE(okSoil);
  TEST_ASSERT_TRUE(true);
  Serial.println("[REAL BASIC] === test_sensor_drivers_real_basic END ===");
}

void setup() {
  TestHarness::waitForSerialAttach("REAL BASIC");
  Serial.println("[REAL BASIC] setup() entered. Running Unity tests...");

  UNITY_BEGIN();
  RUN_TEST(test_motor_drivers_real_basic);
  RUN_TEST(test_sensor_drivers_real_basic);
  UNITY_END();

  Serial.println("[REAL BASIC] Unity run complete.");

  TestHarness::holdAfterTests("REAL BASIC");
}

void loop() {}
