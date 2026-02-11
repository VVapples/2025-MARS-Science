#include <Arduino.h>
#include <TeensyThreads.h>

#include "RoverConfig.h"
#include "MotorDrivers.h"
#include "SensorDrivers.h"
#include "CommandSequencer.h"
#include "SensorRelay.h"
#include "Thread_Logger.h"

void controlInit(CommandSequencer *sequencer);
void commInit(CommandSequencer *sequencer);
void commThread();
void safetyThread();
void safetySetHeartbeat();
void loggerThread();

static bool isMockEnabled(bool flag) {
  if (MOCK_MODE == MODE_REAL) {
    return false;
  }
  if (MOCK_MODE == MODE_FULL_MOCK) {
    return true;
  }
  return flag;
}

static const int GIMBAL_POLE_PAIRS = 7;
static const uint8_t MD3_CAN_ID = 0x201;

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> g_canBus;

MotorPWM g_motorMd1(PIN_MD1_PG, -1, isMockEnabled(MOCK_DRILL));
MotorPWM g_motorMd2(PIN_MD2_PG, -1, isMockEnabled(MOCK_DRILL));
MotorCAN g_motorMd3(g_canBus, MD3_CAN_ID, isMockEnabled(MOCK_VERTICAL));

MotorPWM g_motorMs1(E2_R3_CH, E2_R4_CH, isMockEnabled(MOCK_PUMP), true);
MotorPWM g_motorMs2(E2_R5_CH, E2_R6_CH, isMockEnabled(MOCK_PUMP), true);
MotorPWM g_motorMs3(E2_R7_CH, E2_R8_CH, isMockEnabled(MOCK_PUMP), true);
MotorPWM g_motorMs4(E2_R9_CH, E2_R10_CH, isMockEnabled(MOCK_PUMP), true);
MotorPWM g_motorMs5(E2_R18_CH, E2_R17_CH, isMockEnabled(MOCK_PUMP), true);
MotorPWM g_motorMs6(E2_R16_CH, E2_R15_CH, isMockEnabled(MOCK_PUMP), true);
MotorPWM g_motorMs7(E2_R14_CH, E2_R13_CH, isMockEnabled(MOCK_PUMP), true);
MotorPWM g_motorMs8(E1_R7_CH, -1, isMockEnabled(MOCK_SERVO));

MotorSimpleFOC g_motorMa1(GIMBAL_POLE_PAIRS,
                          PIN_MA1_IN1,
                          PIN_MA1_IN2,
                          PIN_MA1_IN3,
                          E1_R10_CH,
                          isMockEnabled(MOCK_GIMBAL));
MotorSimpleFOC g_motorMa2(GIMBAL_POLE_PAIRS,
                          PIN_MA2_IN1,
                          PIN_MA2_IN2,
                          PIN_MA2_IN3,
                          E1_R18_CH,
                          isMockEnabled(MOCK_GIMBAL));

MotorBase *g_motors[CommandSequencer::MAX_MOTORS + 1] = {
    nullptr,
    &g_motorMd1,
    &g_motorMd2,
    &g_motorMd3,
    &g_motorMs1,
    &g_motorMs2,
    &g_motorMs3,
    &g_motorMs4,
    &g_motorMs5,
    &g_motorMs6,
    &g_motorMs7,
    &g_motorMs8,
    &g_motorMa1,
    &g_motorMa2};

CommandSequencer g_sequencerInstance(g_motors);

SoilSensor g_soilSensor(PIN_S1_SOIL, isMockEnabled(MOCK_SENSORS));
BME280_Wrapper g_bme280(0x76, isMockEnabled(MOCK_SENSORS));
SensorRelay g_sensorRelay(&g_soilSensor, &g_bme280);

static void onSensorRead() {
  SensorSnapshot snapshot = g_sensorRelay.readAll();
  loggerLogf(LogLevel::DATA,
             LogTag::SENSOR,
             "SoilRaw=%d SoilPct=%.1f T=%.1fC P=%.0fPa H=%.1f%%",
             snapshot.soilRaw,
             snapshot.soilPercent,
             snapshot.bme.temperatureC,
             snapshot.bme.pressurePa,
             snapshot.bme.humidityPct);
}

static void onHeartbeat() { safetySetHeartbeat(); }

void setup() {
  Serial.begin(115200);
  delay(200);

  loggerInit();
  loggerLog(LogLevel::INFO, LogTag::SYS, "System boot");

  for (int i = 1; i <= CommandSequencer::MAX_MOTORS; ++i) {
    if (g_motors[i]) {
      g_motors[i]->begin();
    }
  }

  g_sensorRelay.begin();

  g_sequencerInstance.setSensorCallback(onSensorRead);
  g_sequencerInstance.setHeartbeatCallback(onHeartbeat);

  controlInit(&g_sequencerInstance);
  commInit(&g_sequencerInstance);

  threads.addThread(commThread);
  threads.addThread(safetyThread);
  threads.addThread(loggerThread);

  loggerLog(LogLevel::INFO, LogTag::SYS, "Threads started");
}

void loop() {
  threads.yield();
  delay(1);
}