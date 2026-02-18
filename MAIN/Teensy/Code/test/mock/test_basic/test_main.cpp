#include <Arduino.h>
#include <unity.h>

#include <MotorDrivers.h>
#include <SensorDrivers.h>
#include "../../TestHarness.h"
#include "RoverConfig.h"

using namespace MotorDrivers;
using namespace SensorDrivers;

void test_motor_drivers_mock_basic() {
  Serial.println("[MOCK BASIC] === test_motor_drivers_mock_basic START ===");
  MotorPWM pwm("MOCK-PWM", RoverConfig::Pins::MS1_IN1, RoverConfig::Pins::MS1_IN2, true);
  MotorCAN can("MOCK-CAN", 1, true);
  MotorSimpleFOC foc("MOCK-FOC",
                     RoverConfig::Pins::MA1_IN1,
                     RoverConfig::Pins::MA1_IN2,
                     RoverConfig::Pins::MA1_IN3,
                     RoverConfig::Pins::MA1_MA2_EN,
                     true);

  Serial.println("[MOCK BASIC] begin() all motor drivers");
  TEST_ASSERT_TRUE(pwm.begin());
  TEST_ASSERT_TRUE(can.begin());
  TEST_ASSERT_TRUE(foc.begin());

  Serial.println("[MOCK BASIC] drive PWM motor: +0.50 then stop");
  pwm.setSpeed(0.50f);
  pwm.stop();
  Serial.println("[MOCK BASIC] drive CAN motor: -0.25 then stop");
  can.setSpeed(-0.25f);
  can.stop();
  Serial.println("[MOCK BASIC] drive FOC motor: +0.20 then stop");
  foc.setSpeed(0.20f);
  foc.stop();
  Serial.println("[MOCK BASIC] === test_motor_drivers_mock_basic END ===");
}

void test_sensor_drivers_mock_basic() {
  Serial.println("[MOCK BASIC] === test_sensor_drivers_mock_basic START ===");
  SoilMoistureSensor soil("S-1-MOCK", RoverConfig::Pins::S1_SOIL_SIG, true);
  BME280Wrapper bme("S-2-MOCK", true);

  Serial.println("[MOCK BASIC] begin() soil + bme sensors");
  TEST_ASSERT_TRUE(soil.begin());
  TEST_ASSERT_TRUE(bme.begin());

  Serial.println("[MOCK BASIC] reading sensors...");
  auto s = soil.read();
  auto b = bme.read();
  Serial.printf("[MOCK BASIC] soil: raw=%d percent=%.2f | bme.valid=%d temp=%.2f hum=%.2f press=%.2f\n",
                s.raw,
                s.percent,
                b.valid,
                b.temperatureC,
                b.humidityPct,
                b.pressurePa);

  TEST_ASSERT_TRUE(s.percent >= 0.0f && s.percent <= 100.0f);
  TEST_ASSERT_TRUE(b.valid);
  Serial.println("[MOCK BASIC] === test_sensor_drivers_mock_basic END ===");
}

void setup() {
  TestHarness::waitForSerialAttach("MOCK BASIC");
  Serial.println("[MOCK BASIC] setup() entered. Running Unity tests...");

  UNITY_BEGIN();
  RUN_TEST(test_motor_drivers_mock_basic);
  RUN_TEST(test_sensor_drivers_mock_basic);
  UNITY_END();

  Serial.println("[MOCK BASIC] Unity run complete.");

  TestHarness::holdAfterTests("MOCK BASIC");
}

void loop() {}
