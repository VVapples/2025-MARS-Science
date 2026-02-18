#include <Arduino.h>
#include <unity.h>

#include <MotorDrivers.h>
#include <SensorDrivers.h>
#include "RoverConfig.h"

using namespace MotorDrivers;
using namespace SensorDrivers;

void test_motor_drivers_mock_basic() {
  MotorPWM pwm("MOCK-PWM", RoverConfig::Pins::MS1_IN1, RoverConfig::Pins::MS1_IN2, true);
  MotorCAN can("MOCK-CAN", 1, true);
  MotorSimpleFOC foc("MOCK-FOC",
                     RoverConfig::Pins::MA1_IN1,
                     RoverConfig::Pins::MA1_IN2,
                     RoverConfig::Pins::MA1_IN3,
                     RoverConfig::Pins::MA1_MA2_EN,
                     true);

  TEST_ASSERT_TRUE(pwm.begin());
  TEST_ASSERT_TRUE(can.begin());
  TEST_ASSERT_TRUE(foc.begin());

  pwm.setSpeed(0.50f);
  pwm.stop();
  can.setSpeed(-0.25f);
  can.stop();
  foc.setSpeed(0.20f);
  foc.stop();
}

void test_sensor_drivers_mock_basic() {
  SoilMoistureSensor soil("S-1-MOCK", RoverConfig::Pins::S1_SOIL_SIG, true);
  BME280Wrapper bme("S-2-MOCK", true);

  TEST_ASSERT_TRUE(soil.begin());
  TEST_ASSERT_TRUE(bme.begin());

  auto s = soil.read();
  auto b = bme.read();

  TEST_ASSERT_TRUE(s.percent >= 0.0f && s.percent <= 100.0f);
  TEST_ASSERT_TRUE(b.valid);
}

void setup() {
  Serial.begin(115200);
  delay(500);

  UNITY_BEGIN();
  RUN_TEST(test_motor_drivers_mock_basic);
  RUN_TEST(test_sensor_drivers_mock_basic);
  UNITY_END();
}

void loop() {}
