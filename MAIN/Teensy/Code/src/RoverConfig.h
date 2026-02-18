#pragma once

#include <Arduino.h>

namespace RoverConfig {

// ============================================================
// Operation modes (Hybrid Mocking System)
// ============================================================
constexpr uint8_t MODE_REAL = 0;
constexpr uint8_t MODE_FULL_MOCK = 1;
constexpr uint8_t MODE_PARTIAL_MOCK = 2;

// Select active mode here.
constexpr uint8_t OPERATION_MODE = MODE_PARTIAL_MOCK;

// Subsystem-level mock flags (used only in MODE_PARTIAL_MOCK)
constexpr bool MOCK_DRILL = false;       // M-D1, M-D2
constexpr bool MOCK_VERTICAL = false;    // M-D3
constexpr bool MOCK_PUMP = true;         // M-S1..M-S8
constexpr bool MOCK_GIMBAL = true;       // M-A1, M-A2
constexpr bool MOCK_SENSORS = true;      // S-1, S-2, S-A1..S-A7, S-B1, S-B2
constexpr bool MOCK_CAN = true;
constexpr bool MOCK_HEATER = true;
constexpr bool MOCK_LIGHTS = true;

constexpr bool isMockEnabled(bool subsystemFlag) {
  return (OPERATION_MODE == MODE_FULL_MOCK) ||
         (OPERATION_MODE == MODE_PARTIAL_MOCK && subsystemFlag);
}

// ============================================================
// Logging constants
// ============================================================
enum class LogLevel : uint8_t {
  INFO = 0,
  DATA,
  WARN,
  ERROR
};

enum class LogTag : uint8_t {
  SYS = 0,
  MOTOR,
  SENSOR,
  CAN
};

// ============================================================
// Bus / expander addresses
// ============================================================
constexpr uint8_t E1_PCA9685_I2C_ADDR = 0x40;
constexpr uint8_t E2_TCA9548A_I2C_ADDR = 0x77;

// PCA9685 channel rule (updated): Channel = Label_Number
constexpr int8_t pcaChannelFromLabel(int8_t labelNumber) {
  return labelNumber;
}

// ============================================================
// Teensy pin mapping (from pinlayout.md)
// ============================================================
namespace Pins {
// M-S1..M-S7 (POLOLU-4035, PH/EN)
constexpr uint8_t MS1_IN1 = 0;
constexpr uint8_t MS1_IN2 = 1;
constexpr uint8_t MS2_IN1 = 2;
constexpr uint8_t MS2_IN2 = 3;
constexpr uint8_t MS3_IN1 = 4;
constexpr uint8_t MS3_IN2 = 5;
constexpr uint8_t MS4_IN1 = 6;
constexpr uint8_t MS4_IN2 = 7;
constexpr uint8_t MS5_IN1 = 8;
constexpr uint8_t MS5_IN2 = 9;
constexpr uint8_t MS6_IN1 = 10;
constexpr uint8_t MS6_IN2 = 11;
constexpr uint8_t MS7_IN1 = 28;
constexpr uint8_t MS7_IN2 = 29;

// Shared PH/EN mode pin for M-S1..M-S7
constexpr uint8_t MS1_TO_MS7_PMODE = 26;

// M-S8 (servo)
constexpr uint8_t MS8_SIG = 12;

// M-A1 / M-A2 (SimpleFOC style driver control)
constexpr uint8_t MA1_IN3 = 13;
constexpr uint8_t MA1_IN2 = 14;
constexpr uint8_t MA1_IN1 = 15;
constexpr uint8_t MA1_MA2_EN = 20;
constexpr uint8_t MA2_IN3 = 33;
constexpr uint8_t MA2_IN2 = 36;
constexpr uint8_t MA2_IN1 = 37;

// Expanders I2C
constexpr uint8_t E1_SCL = 16;
constexpr uint8_t E1_SDA = 17;
constexpr uint8_t E2_SDA = 18;
constexpr uint8_t E2_SCL = 19;

// Sensors
constexpr uint8_t S1_SOIL_SIG = 21;
constexpr uint8_t S2_BME280_SCL = 24;
constexpr uint8_t S2_BME280_SDA = 25;

// CAN
constexpr uint8_t CAN_RX = 22;
constexpr uint8_t CAN_TX = 23;

// Other outputs
constexpr uint8_t HEAT_CTRL = 32;
constexpr uint8_t LED_WHITE_PWM = 34;
constexpr uint8_t LED_DARK_UV_PWM = 35;

// Alarm feedback
constexpr uint8_t MD2_ALM = 38;
constexpr uint8_t MD1_ALM = 39;
}  // namespace Pins

// ============================================================
// E-1 PCA9685 signal mapping (from pinlayout.md)
// NOTE: pinlayout lists LED0 for all six lines. Channels are
// computed as direct label numbers per updated mapping.
// ============================================================
namespace E1 {
constexpr int8_t MD2_EN_LABEL = 1;
constexpr int8_t MD2_FR_LABEL = 2;
constexpr int8_t MD1_FR_LABEL = 3;
constexpr int8_t MD1_EN_LABEL = 4;
constexpr int8_t MD1_SV_LABEL = 14;
constexpr int8_t MD2_SV_LABEL = 15;

constexpr int8_t MD2_EN_CH = pcaChannelFromLabel(MD2_EN_LABEL);
constexpr int8_t MD2_FR_CH = pcaChannelFromLabel(MD2_FR_LABEL);
constexpr int8_t MD1_FR_CH = pcaChannelFromLabel(MD1_FR_LABEL);
constexpr int8_t MD1_EN_CH = pcaChannelFromLabel(MD1_EN_LABEL);
constexpr int8_t MD1_SV_CH = pcaChannelFromLabel(MD1_SV_LABEL);
constexpr int8_t MD2_SV_CH = pcaChannelFromLabel(MD2_SV_LABEL);
}  // namespace E1

// ============================================================
// E-2 TCA9548A lane mapping (from pinlayout.md)
// ============================================================
namespace E2 {
constexpr uint8_t SA1_CHANNEL = 0;  // SDA0/SCL0
constexpr uint8_t SA2_CHANNEL = 1;  // SDA1/SCL1
constexpr uint8_t SA3_CHANNEL = 2;  // SDA2/SCL2
constexpr uint8_t SA4_CHANNEL = 3;  // SDA3/SCL3
constexpr uint8_t SA5_CHANNEL = 4;  // SDA4/SCL4
constexpr uint8_t SA6_CHANNEL = 5;  // SDA5/SCL5
constexpr uint8_t SA7_CHANNEL = 6;  // SDA6/SCL6
}  // namespace E2

// ============================================================
// G-code numeric ID <-> Component ID mapping (crucial)
// ============================================================
enum class MotorNumericId : uint8_t {
  M1 = 1,
  M2,
  M3,
  M4,
  M5,
  M6,
  M7,
  M8,
  M9,
  M10,
  M11,
  M12,
  M13
};

struct MotorIdMapEntry {
  MotorNumericId numericId;   // M1, M2, ...
  const char* gcodeToken;     // MD1 / MS1 / MA1 (string token form)
  const char* componentId;    // M-D1 / M-S1 / M-A1 (hardware component ID)
};

constexpr MotorIdMapEntry MOTOR_ID_MAP[] = {
    {MotorNumericId::M1, "MD1", "M-D1"},
    {MotorNumericId::M2, "MD2", "M-D2"},
    {MotorNumericId::M3, "MD3", "M-D3"},
    {MotorNumericId::M4, "MS1", "M-S1"},
    {MotorNumericId::M5, "MS2", "M-S2"},
    {MotorNumericId::M6, "MS3", "M-S3"},
    {MotorNumericId::M7, "MS4", "M-S4"},
    {MotorNumericId::M8, "MS5", "M-S5"},
    {MotorNumericId::M9, "MS6", "M-S6"},
    {MotorNumericId::M10, "MS7", "M-S7"},
    {MotorNumericId::M11, "MS8", "M-S8"},
    {MotorNumericId::M12, "MA1", "M-A1"},
    {MotorNumericId::M13, "MA2", "M-A2"},
};

constexpr size_t MOTOR_ID_MAP_COUNT = sizeof(MOTOR_ID_MAP) / sizeof(MOTOR_ID_MAP[0]);

}  // namespace RoverConfig
