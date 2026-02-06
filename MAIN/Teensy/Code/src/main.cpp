#include <Arduino.h>
#include <Wire.h>

#include <Adafruit_PWMServoDriver.h>

#include "CommandSequencer.h"
#include "MotorDrivers.h"
#include "RoverConfig.h"
#include "SensorDrivers.h"
#include "SensorRelay.h"

void ThreadControl_Init(CommandSequencer *sequencer);
void ThreadControl_Update();
void ThreadComm_Init();
void ThreadComm_Update(CommandSequencer *sequencer, SensorRelay *relay);
void Safety_Init();
void Safety_Update();
bool Safety_EStopActive();

namespace {
Adafruit_PWMServoDriver g_pcaE1(PCA9685_E1_ADDR);
Adafruit_PWMServoDriver g_pcaE2(PCA9685_E2_ADDR);

FlexCanBus g_canBus;

DrillMotor g_drill(&g_pcaE1, MOCK_DRILL);
PumpMotor g_pump(&g_pcaE1, MOCK_PUMP);
GimbalMotor g_gimbal(PIN_M_A1_IN1, PIN_M_A1_IN2, PIN_M_A1_IN3, GIMBAL_POLE_PAIRS, MOCK_GIMBAL);
VerticalMotor g_vertical(&g_canBus, VERTICAL_CAN_ID, MOCK_VERTICAL);

SoilSensor g_soil(PIN_S_1_SIGNAL, MOCK_SENSORS);
BME280_Wrapper g_bme(&Wire, BME280_I2C_ADDR, MOCK_SENSORS);

CommandSequencer g_sequencer;
SensorRelay g_sensorRelay;

uint32_t g_lastSensorMs = 0;
bool g_estopHandled = false;
}

void setup() {
    Serial.begin(115200);

    Wire.setSDA(PIN_E1_SDA);
    Wire.setSCL(PIN_E1_SCL);
    Wire.begin();

    Wire1.setSDA(PIN_E2_SDA);
    Wire1.setSCL(PIN_E2_SCL);
    Wire1.begin();

    g_pcaE1.begin();
    g_pcaE1.setPWMFreq(PCA9685_PWM_FREQ_HZ);
    g_pcaE2.begin();
    g_pcaE2.setPWMFreq(PCA9685_PWM_FREQ_HZ);

    g_sequencer.attachMotors(&g_drill, &g_pump, &g_gimbal, &g_vertical);
    g_sequencer.begin();

    g_sensorRelay.attachSensors(&g_soil, &g_bme);
    g_sensorRelay.begin();

    ThreadControl_Init(&g_sequencer);
    ThreadComm_Init();
    Safety_Init();
}

void loop() {
    ThreadComm_Update(&g_sequencer, &g_sensorRelay);
    ThreadControl_Update();
    Safety_Update();

    if (Safety_EStopActive()) {
        if (!g_estopHandled) {
            g_estopHandled = true;
            g_sequencer.clear();
            g_drill.stop();
            g_pump.stop();
            g_gimbal.stop();
            g_vertical.stop();
            Serial.println("ESTOP");
        }
    }

    uint32_t now = millis();
    if (now - g_lastSensorMs >= 100) {
        g_lastSensorMs = now;
        g_sensorRelay.update();
    }
}