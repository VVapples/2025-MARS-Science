#ifndef UNIT_TEST

#include <Arduino.h>
#include <TeensyThreads.h>

#include "AppContext.h"
#include "RoverConfig.h"

#include <MotorDrivers.h>
#include <SensorDrivers.h>

namespace RoverApp {

CommandSequencer gSequencer;
SensorRelay* gSensorRelay = nullptr;

volatile uint32_t gLastHeartbeatMs = 0;
volatile bool gEmergencyStopLatched = false;
volatile uint32_t gControlTickCounter = 0;

}  // namespace RoverApp

namespace {

constexpr bool kMockDrill = RoverConfig::isMockEnabled(RoverConfig::MOCK_DRILL);
constexpr bool kMockVerticalOrCan = RoverConfig::isMockEnabled(RoverConfig::MOCK_VERTICAL) ||
                                    RoverConfig::isMockEnabled(RoverConfig::MOCK_CAN);
constexpr bool kMockPump = RoverConfig::isMockEnabled(RoverConfig::MOCK_PUMP);
constexpr bool kMockGimbal = RoverConfig::isMockEnabled(RoverConfig::MOCK_GIMBAL);
constexpr bool kMockSensors = RoverConfig::isMockEnabled(RoverConfig::MOCK_SENSORS);

// Drill + vertical
MotorDrivers::MotorPWM gMd1("M-D1", RoverConfig::E1::MD1_EN_CH, RoverConfig::E1::MD1_FR_CH, kMockDrill);
MotorDrivers::MotorPWM gMd2("M-D2", RoverConfig::E1::MD2_EN_CH, RoverConfig::E1::MD2_FR_CH, kMockDrill);
MotorDrivers::MotorCAN gMd3("M-D3", 1, kMockVerticalOrCan);

// Pumps / service motors
MotorDrivers::MotorPWM gMs1("M-S1", RoverConfig::Pins::MS1_IN1, RoverConfig::Pins::MS1_IN2, kMockPump);
MotorDrivers::MotorPWM gMs2("M-S2", RoverConfig::Pins::MS2_IN1, RoverConfig::Pins::MS2_IN2, kMockPump);
MotorDrivers::MotorPWM gMs3("M-S3", RoverConfig::Pins::MS3_IN1, RoverConfig::Pins::MS3_IN2, kMockPump);
MotorDrivers::MotorPWM gMs4("M-S4", RoverConfig::Pins::MS4_IN1, RoverConfig::Pins::MS4_IN2, kMockPump);
MotorDrivers::MotorPWM gMs5("M-S5", RoverConfig::Pins::MS5_IN1, RoverConfig::Pins::MS5_IN2, kMockPump);
MotorDrivers::MotorPWM gMs6("M-S6", RoverConfig::Pins::MS6_IN1, RoverConfig::Pins::MS6_IN2, kMockPump);
MotorDrivers::MotorPWM gMs7("M-S7", RoverConfig::Pins::MS7_IN1, RoverConfig::Pins::MS7_IN2, kMockPump);
MotorDrivers::MotorPWM gMs8("M-S8", RoverConfig::Pins::MS8_SIG, RoverConfig::Pins::MS8_SIG, kMockPump);

// Gimbal
MotorDrivers::MotorSimpleFOC gMa1("M-A1",
                                  RoverConfig::Pins::MA1_IN1,
                                  RoverConfig::Pins::MA1_IN2,
                                  RoverConfig::Pins::MA1_IN3,
                                  RoverConfig::Pins::MA1_MA2_EN,
                                  kMockGimbal);
MotorDrivers::MotorSimpleFOC gMa2("M-A2",
                                  RoverConfig::Pins::MA2_IN1,
                                  RoverConfig::Pins::MA2_IN2,
                                  RoverConfig::Pins::MA2_IN3,
                                  RoverConfig::Pins::MA1_MA2_EN,
                                  kMockGimbal);

SensorDrivers::SoilMoistureSensor gSoil("S-1", RoverConfig::Pins::S1_SOIL_SIG, kMockSensors);
SensorDrivers::BME280Wrapper gBme("S-2", kMockSensors);
SensorRelay gSensorRelayInst(&gSoil, &gBme, 250);

void registerMotors() {
  RoverApp::gSequencer.registerMotorByNumericId(RoverConfig::MotorNumericId::M1, &gMd1);
  RoverApp::gSequencer.registerMotorByNumericId(RoverConfig::MotorNumericId::M2, &gMd2);
  RoverApp::gSequencer.registerMotorByNumericId(RoverConfig::MotorNumericId::M3, &gMd3);
  RoverApp::gSequencer.registerMotorByNumericId(RoverConfig::MotorNumericId::M4, &gMs1);
  RoverApp::gSequencer.registerMotorByNumericId(RoverConfig::MotorNumericId::M5, &gMs2);
  RoverApp::gSequencer.registerMotorByNumericId(RoverConfig::MotorNumericId::M6, &gMs3);
  RoverApp::gSequencer.registerMotorByNumericId(RoverConfig::MotorNumericId::M7, &gMs4);
  RoverApp::gSequencer.registerMotorByNumericId(RoverConfig::MotorNumericId::M8, &gMs5);
  RoverApp::gSequencer.registerMotorByNumericId(RoverConfig::MotorNumericId::M9, &gMs6);
  RoverApp::gSequencer.registerMotorByNumericId(RoverConfig::MotorNumericId::M10, &gMs7);
  RoverApp::gSequencer.registerMotorByNumericId(RoverConfig::MotorNumericId::M11, &gMs8);
  RoverApp::gSequencer.registerMotorByNumericId(RoverConfig::MotorNumericId::M12, &gMa1);
  RoverApp::gSequencer.registerMotorByNumericId(RoverConfig::MotorNumericId::M13, &gMa2);
}

void beginMotors() {
  gMd1.begin();
  gMd2.begin();
  gMd3.begin();
  gMs1.begin();
  gMs2.begin();
  gMs3.begin();
  gMs4.begin();
  gMs5.begin();
  gMs6.begin();
  gMs7.begin();
  gMs8.begin();
  gMa1.begin();
  gMa2.begin();
}

}  // namespace

void setup() {
  Serial.begin(115200);
  delay(300);

  RoverApp::gSensorRelay = &gSensorRelayInst;

  RoverApp::loggerInit();
  RoverApp::appLog(RoverConfig::LogLevel::INFO, RoverConfig::LogTag::SYS, "System boot");

  beginMotors();
  registerMotors();

  if (!RoverApp::gSensorRelay->begin()) {
    RoverApp::appLog(RoverConfig::LogLevel::WARN, RoverConfig::LogTag::SENSOR, "Sensor relay init partial/fail");
  }

  RoverApp::updateHeartbeat(millis());

  if (!RoverApp::startControlLoop1kHz()) {
    RoverApp::appLog(RoverConfig::LogLevel::ERROR, RoverConfig::LogTag::SYS, "Control loop timer start failed");
  }

  threads.addThread(RoverApp::threadLogger);
  threads.addThread(RoverApp::threadComm);
  threads.addThread(RoverApp::threadSafety);

  RoverApp::appLog(RoverConfig::LogLevel::INFO, RoverConfig::LogTag::SYS, "Threads started");
}

void loop() {
  RoverApp::processControlTicks();
  threads.yield();
}

#endif  // UNIT_TEST